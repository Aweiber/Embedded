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
#include <linux/slab.h>
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
#include "crc.h"

#include "apollo_sensorhub.h"

#define CWMCU_INTERRUPT
#define BEGIN_UPDATE_SENSOR 100
#define UPDATE_SENSOR_FINISH 104
#define RE_POWER_ON 166
#define POWER_ON 188
#define POWER_OFF 189
#define POWER_RESET 
#define READ_PACKAGE_SIZE 1024
#define READ_BUFFER_SIZE READ_PACKAGE_SIZE * 16

#define SPI_MAGIC 0xAA
#define SPI_HEAD_SIZE 4
#define COMMAND_ID_OLD_PROTOCOL 0x20
#define SPI_ADDR_WRITE 0x80
// #define KERNER_ID ("026e5fd301354c7db55d8bc60eff4d4c")

#define SENSOR_HUB 0x90
#define SENSOR_HUB_IOCTL_CLOCK_SYNC _IOW(SENSOR_HUB, 0x01, char)
#define SENSOR_HUB_IOCTL_GET_BOOT_TO_WALL _IOR(SENSOR_HUB, 0x02, char)
#define SENSOR_HUB_IOCTL_PPG_RESET _IOW(SENSOR_HUB, 0x03, char)
#define SENSOR_HUB_IOCTL_RESET_SENSOR _IOW(SENSOR_HUB, 0x04, char)
// #define SENSOR_HUB_IOCTL_GET_KERNEL_ID _IOR(SENSOR_HUB, 0x05, char)
#define SENSOR_HUB_IOCTL_WRITE_DATA _IOW(SENSOR_HUB, 0x08, char[sizeof(struct user_ioc_transfer)])
#define SENSOR_HUB_IOCTL_READ_DATA _IOR(SENSOR_HUB, 0x09, char[sizeof(struct user_ioc_transfer)])
#define SENSOR_HUB_IOCTL_GET_KERNEL_ID _IOR(SENSOR_HUB, 0x25, char[34])

static const char kernel_id_buf[32] = {"jkw0233c1e13468a8710c9c102system"};

extern struct mutex apollo_ctrl_mutex;
extern struct list_head real_driver_procs;
extern struct list_head driver_procs;
extern struct sensor_kfifo real_sensorDataFifo;
extern struct sensor_kfifo sensorDataFifo;
extern unsigned char *apollo_spi_read_buffer_all;
extern unsigned char *apollo_spi_write_buffer_all;
struct wake_lock data_sync_wakelock;//定义锁

/* turn on gpio interrupt if defined */
#define COMMAD_HEAD_1 0x1b
#define COMMAD_HEAD_2 0xdf
#define COMMAD_TAIL_1 0x9b
#define COMMAD_TAIL_2 0xdf

#define PACKET_HEADER 0xdf1b

#define     HEADER_OFFSET     0               //以帧头做偏移
#define     LEN_OFFSET        2
#define     HEADER_SIZE       2
#define     LEN_SIZE          2
#define     CHECKSUM_SIZE     1
#define     TAIL_SIZE         2


#define     HEADER_H_OFFSET     0               //以帧头做偏移
#define     HEADER_L_OFFSET     1
#define     LEN_H_OFFSET        2
#define     LEN_L_OFFSET        3
#define     TYPE_OFFSET         4
#define     DATA_OFFSET         5

#define     COMMAND_HEAD_LEN      5
#define     COMMAND_CLOCK_SYNC_DATA_LEN 8
#define     COMMAND_PPG_RESET_DATA_LEN 9
#define     COMMAND_CLOCK_SYNC_ID    40
#define     COMMAND_PPG_RESET_ID    44
#define     COMMAND_PPG_RESET_TYPE    1

static char real_fifo_copy_buf[17408];
static char fifo_copy_buf[17408];
char send_start_flag = 0;
static int command_check(uint8_t *data,size_t count)
{
    uint16_t *head_ptr;
    uint16_t len;
    uint8_t checksum = 0,i;
    head_ptr = (uint16_t *)&data[HEADER_OFFSET];
    if(*head_ptr != PACKET_HEADER)
    {
        printk(KERN_ALERT"------------PACKET_HEADER err ----------\n");
        return -1;
    }

    len = data[LEN_L_OFFSET] | (data[LEN_H_OFFSET] << 8);

    if(len + HEADER_SIZE + LEN_SIZE + CHECKSUM_SIZE + TAIL_SIZE  > count)
    {
        printk(KERN_ALERT"------------len err ----------\n");
        return -1;
    }

    for(i = 0;i< len + HEADER_SIZE + LEN_SIZE;i++)
    {
        checksum += data[i];
    }

    if ( checksum != data[len + HEADER_SIZE + LEN_SIZE])
    {
        printk(KERN_ALERT"------------checksum err %02x %02x %02x %02x %02x %02x %02x %02x %02x ----------\n",
        data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],checksum);
        return -1;
    }

    return 0;
}

static void parcel_data(uint8_t *dst,uint8_t * data,size_t count,uint32_t *packet_length)
{
    uint16_t crc_length,crc16;
    uint16_t *crc_ptr;
    struct spi_package_head *head = (struct spi_package_head *)dst;
    head->magic = SPI_MAGIC;
    head->cmd_id = COMMAND_ID_OLD_PROTOCOL;
    head->data_length = count;
    memcpy(head->data, data, head->data_length);
    crc_length = head->data_length + SPI_HEAD_SIZE;
    crc16 = crc16_ccitt(dst, crc_length);
    crc_ptr = (uint16_t *)&dst[crc_length];
    *crc_ptr = crc16;
    *packet_length = head->data_length + 5;
}

bool is_packet_ok(uint8_t *data,uint16_t count) {
    uint16_t crc_length,crc16;
    uint16_t *crc_ptr;
    struct spi_package_head *head = (struct spi_package_head *)data;
    if (head->magic != SPI_MAGIC) {
        printk(KERN_ALERT"!!!!!!!!!SPI_MAGIC is error = 0x%x\n", head->magic);
        return false;       
    }
    crc_length = head->data_length + SPI_HEAD_SIZE;
    if(crc_length + 2 > count)
    {
        printk(KERN_ALERT"!!!!!!crc_length is overflow\n, crc_length = %d, head_lenth=%d, count=%d\n", crc_length, head->data_length, count);
        return false;
    }
    crc16 = crc16_ccitt(data, crc_length);
    crc_ptr = (uint16_t *)&data[crc_length];
    printk(KERN_ALERT"crc16=0x%x---*crcptr=0x%x\n", crc16, *crc_ptr);
    return *crc_ptr == crc16;
}

uint16_t packet_size(uint8_t *data)
{
    struct spi_package_head *head = (struct spi_package_head *)data;
    return head->data_length + SPI_HEAD_SIZE + 2;
}

static int sensor_open(struct inode *inode, struct file *file)
{
    int rc = 0;
    struct proc_data *proc;
    proc = kzalloc(sizeof(*proc), GFP_KERNEL);
    if (proc == NULL)
        return -ENOMEM;
    proc->dataBuffer = &sensorDataFifo;
    init_waitqueue_head(&proc->wait);
    INIT_LIST_HEAD(&proc->list);
    list_add_tail(&proc->list, &driver_procs);
    file->private_data = proc;
    send_start_flag = 1;
    // if(send_start_flag == false)
    // {
    //     uint8_t test_buff[15] = {0x80, 0xAA, 0x20, 0x08, 0x1B, 0xDF, 0x00, 0x01, 0x14, 0x0F, 0x9B, 0xDF, 0x15, 0x86};
    //     printk(KERN_ALERT"spi_write_bytes_seria\r\n");
    //     spi_write_bytes_serial(test_buff, 15);
    //     send_start_flag = true;
    // }
    return rc;
}

static int sensor_release(struct inode *inode, struct file *file)
{
    int rc = 0;
    struct proc_data *proc = file->private_data;
    send_start_flag = 1;
    if (proc == NULL)
    {
        return rc;
    }
    list_del(&proc->list);
    proc->dataBuffer = NULL;
    kfree(proc);
    return rc;
}

static int sensor_write(struct file *file, const unsigned char *buf, size_t count)
{
    unsigned char wbuf[READ_PACKAGE_SIZE] = {0};
    uint32_t package_size;
    uint16_t write_i=0;
    if(count > READ_PACKAGE_SIZE)
    {
        printk(KERN_ALERT"------------sensor_write count error count=%d----------\n", count);
        return 0;
    }
    printk(KERN_ALERT"------------sensor_write----------\n");
    uint8_t temp[READ_PACKAGE_SIZE];
    uint16_t data_size,command_size;
    copy_from_user(wbuf, buf, count);

    if (command_check(wbuf,count))
    {
        return count;
    }
    printk(KERN_ALERT"------------sensor_write cmd is ----------%d\n",wbuf[4]);
    switch (wbuf[4])
    {
    case BEGIN_UPDATE_SENSOR:
        mutex_lock(&apollo_ctrl_mutex);
        mt_set_gpio_out(GPIO_SENSORHUB_POWER_EN, 0);
        msleep(100);
        mt_set_gpio_out(GPIO_SENSORHUB_POWER_EN, 1);
        mutex_unlock(&apollo_ctrl_mutex);
        //readSize=15;
        break;
    case RE_POWER_ON:
        printk(KERN_ALERT"Apollo_Sensor reset from app spi\n");   
        mutex_lock(&apollo_ctrl_mutex);
        mt_set_gpio_out(GPIO_SENSORHUB_POWER_EN, 0);
        msleep(50);
        mt_set_gpio_out(GPIO_SENSORHUB_POWER_EN, 1);
        // msleep(200);
        // spi_init(spi);
        mutex_unlock(&apollo_ctrl_mutex);
        break;
    case POWER_ON:
        printk(KERN_ALERT"Apollo_Sensor power on from app spi\n");   
        mutex_lock(&apollo_ctrl_mutex);
        mt_set_gpio_out(GPIO_SENSORHUB_POWER_EN,1);
        mutex_unlock(&apollo_ctrl_mutex);
        break;
    case POWER_OFF:
        printk(KERN_ALERT"Apollo_Sensor power off from app spi\n");   
        mutex_lock(&apollo_ctrl_mutex);
        mt_set_gpio_out(GPIO_SENSORHUB_POWER_EN,0);
        mutex_unlock(&apollo_ctrl_mutex);
        break;         
    default:
        data_size = wbuf[LEN_L_OFFSET] | (wbuf[LEN_H_OFFSET] << 8);
        command_size = data_size + HEADER_SIZE + LEN_SIZE + CHECKSUM_SIZE + TAIL_SIZE;
        temp[0] = SPI_ADDR_WRITE;
        parcel_data(&temp[1],wbuf,command_size,&package_size);
        printk(KERN_ALERT"apollo write data:");
        for(; write_i<package_size+2; write_i++)
        {
            printk(KERN_ALERT"0x%x ", temp[write_i]);
        }
        printk(KERN_ALERT"\r\n");

        mutex_lock(&apollo_ctrl_mutex);
        int ret = spi_write_bytes_serial(temp, package_size + 2);
        printk(KERN_ALERT"------------sensor_write spi_write_bytes_serial ret %d----------\n",ret);
        mutex_unlock(&apollo_ctrl_mutex);
        break;
    }
    return count;
}

static int sensor_read(struct file *file, unsigned char *buf, size_t count)
{
    struct proc_data *proc = file->private_data;
    uint16_t i=0;   
    uint16_t test_len = 0; 
    //等待数据到来
    int re = wait_event_interruptible(proc->wait, !kfifo_is_empty(proc->dataBuffer));
    if (re)
    {
        //异常
        return -ERESTARTSYS;
    }
    //重新获取队列element长度
    int len = kfifo_len(proc->dataBuffer);
    //一次最对返回READ_BUFFER_SIZE字节
    // len = len > READ_BUFFER_SIZE ? READ_BUFFER_SIZE : len;
    len = len > 16 ? 16 : len;
    //判断上层读取buffer长度
    printk(KERN_ALERT"app read len=%d, count=%d\n", len, count);
    if ((count/1024) < len)
    {
        len = count/1024;//count - count % READ_PACKAGE_SIZE;
    }
    //推出len长度到writebuff中
    unsigned int copied = 0;
    mutex_lock(&apollo_ctrl_mutex);
    // kfifo_to_user(proc->dataBuffer, buf, len, &copied);
    // kfifo_out(proc->dataBuffer, buf, len);
    kfifo_out(proc->dataBuffer, fifo_copy_buf, len);
    copy_to_user(buf, fifo_copy_buf, len*READ_PACKAGE_SIZE);
    printk(KERN_ALERT"to user data:len=%d, copied=%d\n", len, copied);
    for(test_len=0; test_len<len; test_len++)
    {
        for(i=0; i<32; i++)
        {
            printk(KERN_ALERT"0x%x ", fifo_copy_buf[i+test_len*READ_PACKAGE_SIZE]);
        }
    }
    printk(KERN_ALERT"\r\n");
    mutex_unlock(&apollo_ctrl_mutex);

    printk(KERN_ALERT"return app read len = %d, READ_PACKAGE_SIZE=%d\r\n", len*READ_PACKAGE_SIZE, READ_PACKAGE_SIZE);
    
    len = (len*READ_PACKAGE_SIZE>count)?count:len*READ_PACKAGE_SIZE;
    return len;
}

static int sensor_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
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
    case SENSOR_HUB_IOCTL_CLOCK_SYNC:
    {
        uint8_t temp[READ_PACKAGE_SIZE];
        uint32_t package_size;
        uint8_t wbuffer[255] = {0};
        int64_t send_time;
        uint8_t checksum_size = COMMAND_HEAD_LEN + COMMAND_CLOCK_SYNC_DATA_LEN;
        uint8_t checksum = 0;
        uint8_t i = 0;
        mutex_lock(&apollo_ctrl_mutex);
        wake_lock(&data_sync_wakelock);
        wbuffer[HEADER_H_OFFSET] = COMMAD_HEAD_1;
        wbuffer[HEADER_L_OFFSET] = COMMAD_HEAD_2;
        wbuffer[LEN_H_OFFSET] = 0;
        wbuffer[LEN_L_OFFSET] = COMMAND_CLOCK_SYNC_DATA_LEN + 1;
        wbuffer[TYPE_OFFSET] = COMMAND_CLOCK_SYNC_ID; 
        //spi传输时存在执行延时600us,再加上传输时间和求模，这里把时间加1ms 
        send_time = ktime_to_ms(ktime_get_boottime()) + 1;
        wbuffer[DATA_OFFSET] = send_time >> 56 & 0xff;
        wbuffer[DATA_OFFSET + 1] = send_time >> 48 & 0xff;
        wbuffer[DATA_OFFSET + 2] = send_time >> 40 & 0xff;
        wbuffer[DATA_OFFSET + 3] = send_time >> 32 & 0xff;
        wbuffer[DATA_OFFSET + 4] = send_time >> 24 & 0xff;
        wbuffer[DATA_OFFSET + 5] = send_time >> 16 & 0xff;
        wbuffer[DATA_OFFSET + 6] = send_time >> 8 & 0xff;
        wbuffer[DATA_OFFSET + 7] = send_time  & 0xff;
        for(i = 0;i < checksum_size;i++)
        {
            checksum += wbuffer[i];
        }
        wbuffer[checksum_size] = checksum;
        wbuffer[checksum_size + 1] = COMMAD_TAIL_1;
        wbuffer[checksum_size + 2] = COMMAD_TAIL_2;
        temp[0] = SPI_ADDR_WRITE;
        parcel_data(&temp[1],wbuffer,16,&package_size);    
        err = spi_write_bytes_serial(temp, package_size + 1);
        wake_unlock(&data_sync_wakelock);
        mutex_unlock(&apollo_ctrl_mutex);
    }
    break;
    case SENSOR_HUB_IOCTL_GET_BOOT_TO_WALL:
    {
        int64_t boot_to_wall;
        boot_to_wall = ktime_to_ms(ktime_get_boot_to_wall()); 
        // boot_to_wall = ktime_to_ms(ktime_get_real());
        // printk(KERN_ALERT"apollo boot_to_wall=%lld\n", boot_to_wall);
        err = copy_to_user(data, &boot_to_wall, sizeof(boot_to_wall));       
    }       
    break;
    case SENSOR_HUB_IOCTL_GET_KERNEL_ID:
    {
        // uint8_t kernel_id[32] = KERNER_ID;
        err = copy_to_user(data, &kernel_id_buf, sizeof(kernel_id_buf));       
    } 
    case SENSOR_HUB_IOCTL_PPG_RESET:
    {
         uint8_t temp[READ_PACKAGE_SIZE];
        uint32_t package_size;
        uint8_t wbuffer[255] = {0};
        int64_t send_time;
        uint8_t checksum_size = COMMAND_HEAD_LEN + COMMAND_PPG_RESET_DATA_LEN;
        uint8_t checksum = 0;
        uint8_t i = 0;
        mutex_lock(&apollo_ctrl_mutex);
        wbuffer[HEADER_H_OFFSET] = COMMAD_HEAD_1;
        wbuffer[HEADER_L_OFFSET] = COMMAD_HEAD_2;
        wbuffer[LEN_H_OFFSET] = 0;
        wbuffer[LEN_L_OFFSET] = COMMAND_PPG_RESET_DATA_LEN + 1;
        wbuffer[TYPE_OFFSET] = COMMAND_PPG_RESET_ID; 
        //spi传输时存在执行延时600us,再加上传输时间和求模，这里把时间加1ms 
        send_time = ktime_to_ms(ktime_get_boottime()) + 1;
        wbuffer[DATA_OFFSET] = send_time >> 56 & 0xff;
        wbuffer[DATA_OFFSET + 1] = send_time >> 48 & 0xff;
        wbuffer[DATA_OFFSET + 2] = send_time >> 40 & 0xff;
        wbuffer[DATA_OFFSET + 3] = send_time >> 32 & 0xff;
        wbuffer[DATA_OFFSET + 4] = send_time >> 24 & 0xff;
        wbuffer[DATA_OFFSET + 5] = send_time >> 16 & 0xff;
        wbuffer[DATA_OFFSET + 6] = send_time >> 8 & 0xff;
        wbuffer[DATA_OFFSET + 7] = send_time  & 0xff;
        wbuffer[DATA_OFFSET + 8] = COMMAND_PPG_RESET_TYPE;
        for(i = 0;i < checksum_size;i++)
        {
            checksum += wbuffer[i];
        }
        wbuffer[checksum_size] = checksum;
        wbuffer[checksum_size + 1] = COMMAD_TAIL_1;
        wbuffer[checksum_size + 2] = COMMAD_TAIL_2;
        temp[0] = SPI_ADDR_WRITE;
        parcel_data(&temp[1],wbuffer,17,&package_size);    
        err = spi_write_bytes_serial(temp, package_size + 1);
        mutex_unlock(&apollo_ctrl_mutex);
    }
    break;
    case SENSOR_HUB_IOCTL_READ_DATA:
    {
        struct user_ioc_transfer user_ioc;
        err = copy_from_user(&user_ioc, data,sizeof(struct user_ioc_transfer));
        if(err)
        {
            break;
        }       
        err = user_read_ioc(&user_ioc);
    }
    break;
    case SENSOR_HUB_IOCTL_WRITE_DATA:
    {
        struct user_ioc_transfer user_ioc;
        err = copy_from_user(&user_ioc, data,sizeof(struct user_ioc_transfer));
        if(err)
        {
            break;
        }
        err = user_write_ioc(&user_ioc);
    }
    break;
    default:
        break;
    }

    return err;
}

static const struct file_operations sensor_fops = {
    .owner = THIS_MODULE,
    .open = sensor_open,
    .release = sensor_release,
    .write = sensor_write,
    .read = sensor_read,
    .unlocked_ioctl = sensor_ioctl

};

static struct miscdevice sensorhub_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "xjkbuffer",
    .fops = &sensor_fops};


static int real_sensor_open(struct inode *inode, struct file *file)
{
    int rc = 0;
    struct proc_data *proc;
    proc = kzalloc(sizeof(*proc), GFP_KERNEL);
    if (proc == NULL)
        return -ENOMEM;
    proc->dataBuffer = &real_sensorDataFifo;
    init_waitqueue_head(&proc->wait);
    INIT_LIST_HEAD(&proc->list);
    list_add_tail(&proc->list, &real_driver_procs);
    file->private_data = proc;
    return rc;
}

static int real_sensor_release(struct inode *inode, struct file *file)
{
    int rc = 0;
    struct proc_data *proc = file->private_data;
    if (proc == NULL)
    {
        return rc;
    }
    list_del(&proc->list);
    proc->dataBuffer = NULL;
    kfree(proc);
    return rc;
}

static int real_sensor_read(struct file *file, unsigned char *buf, size_t count)
{
    struct proc_data *proc = file->private_data;
    uint16_t i=0;
    uint16_t test_len = 0;
    //等待数据到来
    int re = wait_event_interruptible(proc->wait, !kfifo_is_empty(proc->dataBuffer));
    if (re)
    {
        //异常
        return -ERESTARTSYS;
    }
    //重新获取队列element长度
    int len = kfifo_len(proc->dataBuffer);
    len = len > 16 ? 16 : len;
    //判断上层读取buffer长度
    printk(KERN_ALERT"real time app read len=%d, count=%d\n", len, count);
    if ((count/1024) < len)
    {
        len = count/1024;//count - count % READ_PACKAGE_SIZE;
    }
    //推出len长度到writebuff中
    unsigned int copied = 0;
    mutex_lock(&apollo_ctrl_mutex);
    kfifo_out(proc->dataBuffer, real_fifo_copy_buf, len);
    copy_to_user(buf, real_fifo_copy_buf, len*READ_PACKAGE_SIZE);
    printk(KERN_ALERT"real buf to user data:len=%d, copied=%d\n", len, copied);
    for(test_len=0; test_len<len; test_len++)
    {
        for(i=0; i<32; i++)
        {
            printk(KERN_ALERT"0x%x ", real_fifo_copy_buf[i+test_len*READ_PACKAGE_SIZE]);
        }
    }
    printk(KERN_ALERT"\r\n");
    mutex_unlock(&apollo_ctrl_mutex);

    printk(KERN_ALERT"real buf return app read len = %d, READ_PACKAGE_SIZE=%d\r\n", len*READ_PACKAGE_SIZE, READ_PACKAGE_SIZE);
    
    len = (len*READ_PACKAGE_SIZE>count)?count:len*READ_PACKAGE_SIZE;
    return len;
}

static const struct file_operations real_sensor_fops = {
    .owner = THIS_MODULE,
    .open = real_sensor_open,
    .release = real_sensor_release,
    .read = real_sensor_read,
};

static struct miscdevice real_sensorhub_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "xjkrealtime",
    .fops = &real_sensor_fops};
/**
 *
 *
 */
int xjktest_init(void *bus_dev)
{
    int error;
    //注册字符设备
    send_start_flag = 0;
    error = misc_register(&real_sensorhub_misc);
    error = misc_register(&sensorhub_misc);
    wake_lock_init(&data_sync_wakelock, WAKE_LOCK_SUSPEND, "sensorh_hub_lock");//初始化锁
    return error;
}
