/*
 * Author: ncy
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input-polldev.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/vmalloc.h>
#include <linux/pm.h>
#include <linux/pm_runtime.h>
#include <linux/string.h>
#include <linux/dma-mapping.h>
#include <linux/earlysuspend.h>
#include <linux/gpio.h>
#include <mach/mt_gpio.h>
#include <cust_gpio_usage.h>
#include <cust_eint.h>
#include <mach/eint.h>
#include <mach/mt_pm_ldo.h>
#include <linux/wakelock.h>
#include <linux/spi/spi.h>
#include <mach/mt_spi.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/kfifo.h>
#include <linux/wait.h>
#include <linux/list.h>
#include "apollo_sensorhub.h"

struct version_data
{
    uint8_t data[128];
};

struct spi_write_data
{
    uint8_t data[255];
};

#define SENSOR_HUB 0x90
#define SENSOR_HUB_IOCTL_SET_POWER_ON _IOW(SENSOR_HUB, 0x01, char)
#define SENSOR_HUB_IOCTL_SET_POWER_OFF _IOW(SENSOR_HUB, 0x02, char)
#define SENSOR_HUB_IOCTL_SET_POWER_RESTART _IOW(SENSOR_HUB, 0x03, char)
#define SENSOR_HUB_IOCTL_SET_EXCHANGE_SIZE _IOW(SENSOR_HUB, 0x04, int)
#define SENSOR_HUB_IOCTL_WAIT_DATA _IOR(SENSOR_HUB, 0x05, char)
#define SENSOR_HUB_IOCTL_WRITE_DATA _IOW(SENSOR_HUB, 0x06, struct spi_write_data)
#define SENSOR_HUB_IOCTL_READ_VERSION _IOR(SENSOR_HUB, 0x07, struct version_data)

extern struct mutex apollo_ctrl_mutex;
extern struct list_head driver_procs;
extern struct sensor_kfifo sensorDataFifo;
extern unsigned char *apollo_spi_write_buffer_all;

static int sensorhub_open(struct inode *inode, struct file *file)
{
    int rc = 0;
    struct proc_data *proc;
    proc = kzalloc(sizeof(*proc), GFP_KERNEL);
    if (proc == NULL)
        return -ENOMEM;
    init_waitqueue_head(&proc->wait);
    INIT_LIST_HEAD(&proc->list);
    list_add_tail(&proc->list, &driver_procs);
    file->private_data = proc;
    return rc;
}

static int sensorhub_mmap(struct file *filp, struct vm_area_struct *vma)
{
    int map_size = vma->vm_end - vma->vm_start;
    APOLLO_DEBUG("sensorhub_mmap vma->vm_pgoff %lu,map_size %d,vma->vm_end %lu,vma->vm_start %lu",(unsigned long)vma->vm_pgoff,map_size,vma->vm_end,vma->vm_start);
    if (vma->vm_pgoff > 1)
    {
        return -EINVAL;
    }
    if (vma->vm_pgoff == 0 && map_size != 4096)
    {
        return -EINVAL;
    }
    uint32_t fifo_mmap_size = get_enough_page_size(SENSOR_FIFO_DATA_SIZE);
    if (vma->vm_pgoff == 1 && map_size != fifo_mmap_size)
    {
        return -EINVAL;
    }
    vma->vm_flags |= VM_IO;       //表示对设备IO空间的映射
    vma->vm_flags |= VM_RESERVED; //标志该内存区不能被换出，在设备驱动中虚拟页和物理页的关系应该是长期的，应该保留起来，不能随便被别的虚拟页换出

    if (vma->vm_pgoff == 1)
    {

        if (remap_pfn_range(vma,                                                   //虚拟内存区域，即设备地址将要映射到这里
                            vma->vm_start,                                         //虚拟空间的起始地址
                            virt_to_phys(sensorDataFifo.kfifo.data) >> PAGE_SHIFT, //与物理内存对应的页帧号，物理地址右移12位
                            map_size,                                              //映射区域大小，一般是页大小的整数倍
                            vma->vm_page_prot))                                              //保护属性，
        {
            return -EAGAIN;
        }
        return 0;
    }
    struct sensor_kfifo *buffer_mirror;
    buffer_mirror = kzalloc(PAGE_SIZE, GFP_KERNEL);
    if (buffer_mirror == NULL)
    {
        return -ENOMEM;
    }
    SetPageReserved(virt_to_page(buffer_mirror));
    if (remap_pfn_range(vma,                                       //虚拟内存区域，即设备地址将要映射到这里
                        vma->vm_start,                             //虚拟空间的起始地址
                        virt_to_phys(buffer_mirror) >> PAGE_SHIFT, //与物理内存对应的页帧号，物理地址右移12位
                        map_size,                                  //映射区域大小，一般是页大小的整数倍
                        vma->vm_page_prot))                        //保护属性，
    {
        kfree(buffer_mirror);
        return -EAGAIN;
    }
    memcpy(buffer_mirror, &sensorDataFifo, sizeof(*buffer_mirror));
    buffer_mirror->kfifo.out = buffer_mirror->kfifo.in;
    struct proc_data *proc = filp->private_data;
    proc->dataBuffer = buffer_mirror;
    return 0;
}

static int sensorhub_release(struct inode *inode, struct file *file)
{
    int rc = 0;
    struct proc_data *proc = file->private_data;
    if (proc == NULL)
    {
        return rc;
    }
    list_del(&proc->list);
    if (proc->dataBuffer != NULL)
    {
        struct kfifo *dataBuffer = proc->dataBuffer;
        ClearPageReserved(virt_to_page(dataBuffer));
        kfree(dataBuffer);
        proc->dataBuffer = NULL;
    }
    kfree(proc);
    return rc;
}

static int sensorhub_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    void __user *data;
    int err = 0;

    if (_IOC_DIR(cmd) & _IOC_READ)
    {
        err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    }
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
    {
        err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    }

    if (err)
    {
        return -EFAULT;
    }

    data = (void __user *)arg;

    if (data == NULL)
    {
        return -EINVAL;
    }
    switch (cmd)
    {
    case SENSOR_HUB_IOCTL_SET_POWER_ON:
        mutex_lock(&apollo_ctrl_mutex);
        mt_set_gpio_out(GPIO_SENSORHUB_POWER_EN, 1);
        mutex_unlock(&apollo_ctrl_mutex);
        break;

    case SENSOR_HUB_IOCTL_SET_POWER_OFF:
        mutex_lock(&apollo_ctrl_mutex);
        mt_set_gpio_out(GPIO_SENSORHUB_POWER_EN, 0);
        mutex_unlock(&apollo_ctrl_mutex);
        break;
    case SENSOR_HUB_IOCTL_SET_POWER_RESTART:
        mutex_lock(&apollo_ctrl_mutex);
        mt_set_gpio_out(GPIO_SENSORHUB_POWER_EN, 0);
        msleep(100);
        mt_set_gpio_out(GPIO_SENSORHUB_POWER_EN, 1);
        mutex_unlock(&apollo_ctrl_mutex);
        break;
    case SENSOR_HUB_IOCTL_SET_EXCHANGE_SIZE:
        break;
    case SENSOR_HUB_IOCTL_WAIT_DATA:
    {
        struct proc_data *proc = file->private_data;
        if (proc->dataBuffer == NULL)
        {
            err = -EINVAL;
            break;
        }
        int wait_err = wait_event_interruptible(proc->wait, proc->dataBuffer->kfifo.in != proc->dataBuffer->kfifo.out);
        if (wait_err)
        {
            //异常
            err = -ERESTARTSYS;
            break;
        }
    }
    break;
    case SENSOR_HUB_IOCTL_WRITE_DATA:
    {
        mutex_lock(&apollo_ctrl_mutex);
        copy_from_user(apollo_spi_write_buffer_all, data, 255);
        int write_err = spi_write_bytes_serial(apollo_spi_write_buffer_all, 255);
        mutex_unlock(&apollo_ctrl_mutex);
        if (write_err)
        {
            err = -EIO;
        }
    }
    break;
    case SENSOR_HUB_IOCTL_READ_VERSION:
    {
        char jk_version[128] = {0};
        char *ptr_pl, *ptr_lk, *ptr_sn;
        ptr_pl = strstr(saved_command_line, "pl_version=");
        ptr_lk = strstr(saved_command_line, "lk_version=");
        ptr_sn = strstr(saved_command_line, "sn_version=");
        if (ptr_pl == NULL || ptr_lk == NULL || ptr_sn == NULL)
        {
            err = -EIO;
            break;
        }
        strncpy(jk_version, ptr_pl, ptr_sn - ptr_pl);
        sprintf(jk_version + strlen(jk_version), "%s%s%s", "kernel_version=", JK_KERNEL_VERSION, "end");
        APOLLO_INFO("jk_version=%s,len=%d\n", jk_version, strlen(jk_version));
        copy_to_user(data, jk_version, strlen(jk_version) + 1);
    }
    break;

    default:
        APOLLO_INFO("[sensor hub]unknown IOCTL.\n");
        break;
    }

    return err;
}

static const struct file_operations sensorhub_fops = {
    .owner = THIS_MODULE,
    .open = sensorhub_open,
    .mmap = sensorhub_mmap,
    .release = sensorhub_release,
    .unlocked_ioctl = sensorhub_ioctl};

static struct miscdevice sensorhub_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "xjksensorhub",
    .fops = &sensorhub_fops};

/**
 *
 *
 */
int sync_file_init(void *bus_dev)
{
    int error;
    //注册心吉康版本信息字符设备
    error = misc_register(&sensorhub_misc);
    return error;
}