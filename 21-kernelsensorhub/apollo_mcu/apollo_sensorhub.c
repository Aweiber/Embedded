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
#include "apollo_sensorhub.h"
#include "crc.h"

#define CWMCU_INTERRUPT
#define BEGIN_UPDATE_SENSOR 100
#define UPDATE_SENSOR_FINISH 104
#define RE_POWER_ON 166
#define JK_KERNEL_VERSION "0.0.0.30"
#define SUPPORT_REPLY_SENSORHUB
DEFINE_MUTEX(apollo_ctrl_mutex);
LIST_HEAD(driver_procs);
LIST_HEAD(real_driver_procs);

extern struct workqueue_struct *system_highpri_wq;
extern char send_start_flag;

static struct work_struct read_sensorhub_work;

struct sensor_kfifo real_sensorDataFifo;
struct sensor_kfifo sensorDataFifo;
unsigned char *apollo_spi_read_buffer_all = NULL;
unsigned char *apollo_spi_write_buffer_all = NULL;
static uint8_t temp_arry[1024];
/* turn on gpio interrupt if defined */

uint32_t get_enough_page_size(uint32_t size)/*PAGE_SIZE = 4096*/
{
    return size % PAGE_SIZE == 0 ? size : (size / PAGE_SIZE + 1) * PAGE_SIZE;
}

static void apollo_work_report(struct work_struct *work)
{
    local_irq_disable();
    mutex_lock(&apollo_ctrl_mutex);
    static uint32_t fifo_full=0;
    uint16_t fifo_len = 0,last_fifo_len,read_size = 0;
    uint8_t fifo_len_addr,fifo_addr;
    uint8_t *temp;
    struct proc_data *real_proc = NULL;
    struct proc_data *proc = NULL;
    fifo_len_addr = 0x7c;
    uint8_t i=0;
    uint8_t reapet_times = 128;
    uint8_t wake_up_module = 0;

    APOLLO_INFO("apollo_inter\n");
    memset(temp_arry, 0, sizeof(temp_arry));

    do{
        last_fifo_len = fifo_len;
        apollo_write_then_read(&fifo_len_addr,1,&fifo_len,2);
        reapet_times--;
    }while((fifo_len != 0) && (fifo_len != last_fifo_len) && (reapet_times));
    // }while(fifo_len != 0 && fifo_len != last_fifo_len);
    printk(KERN_ALERT"apollo_lenth=%d\r\n", fifo_len);
    if(fifo_len >= 1024) fifo_len = 0;
    if(fifo_len != 0)
    {    
        //temp = kmalloc(1024,GFP_KERNEL);
        temp = temp_arry;
        fifo_addr = 0x7F;
        apollo_write_then_read(&fifo_addr,1,temp,fifo_len);       
        // printk(KERN_ALERT"recv_len = %d\r\n", fifo_len);
//#if SUPPORT_REPLY_SENSORHUB                
        uint8_t recv_ok_buf[2]={0xFB, 1};
        spi_write_bytes_serial(recv_ok_buf, 2);     
//#endif
        // for(i=0; i<fifo_len; i++)
        // {
        //     if((i)%16 == 0) printk(KERN_ALERT"\r\nRECV: ");
        //     printk(KERN_ALERT"%x ", temp[i]);
        // }
        // printk(KERN_ALERT"\r\n");
    }
    printk(KERN_ALERT"apollo_in_while\n");
    while(read_size < fifo_len)
    {
        struct spi_package_head* header;
        printk(KERN_ALERT"fifo_len=%d, read_size=%d\n", fifo_len, read_size);
        if(!is_packet_ok(&temp[read_size],fifo_len - read_size))
        {
            read_size++;
            continue;
        }
        header = (struct spi_package_head*)&temp[read_size];
        printk(KERN_ALERT"apollo_crc_ok, cmdid=%d\n", header->cmd_id);

        if(header->cmd_id == 0x20)
        {
            uint8_t data_temp[READ_PACKAGE_SIZE];
            memset(data_temp,0,READ_PACKAGE_SIZE);
            memcpy(&data_temp[2],header->data,header->data_length);
            data_temp[0] = (header->data_length>>8)&0xFF;
            data_temp[1] = header->data_length&0xFF;
            printk(KERN_ALERT"apllo_page_num=%d\r\n", (header->data[4]<<8)|header->data[5]);

            if (header->data[8] == 0xFF)  //内核单独处理的sensor指令
            {
                //#if SUPPORT_REPLY_SENSORHUB
                    uint8_t ask_init_buf[2]={0xF8, 1};
                    spi_write_bytes_serial(ask_init_buf, 2);  
                //#endif 
                // printk(KERN_ALERT"sensor ask app start\n");
                uint8_t i=0;
                uint8_t test_buff[16] = {0x80, 0xAA, 0x20, 0x09, 0x00, 0x1B, 0xDF, 0x00, 0x02, 0xFF, 0x01, 0x0F, 0x9B, 0xDF, 0x15, 0x86};
                test_buff[10]=send_start_flag;
                //send_start_flag = 0;
                test_buff[11]=0;
                for(i=5; i<11; i++)
                {
                    test_buff[11] += test_buff[i];
                }
                uint16_t crc16_data = 0;
                crc16_data = crc16_ccitt(test_buff+1, 13);
                test_buff[14] = crc16_data&0xFF;
                test_buff[15] = (crc16_data>>8)&0xFF;
                // printk(KERN_ALERT"write app start state = %d\r\n", send_start_flag);
                spi_write_bytes_serial(test_buff, 16);                

            }
            else if (header->data[0] == 0x2B)
            {
                printk(KERN_ALERT"apollo real time data\n");
                if (kfifo_is_full(&real_sensorDataFifo))
                {
                    printk(KERN_ALERT"real buffer is full\n");
                    kfifo_skip(&real_sensorDataFifo);
                }
                kfifo_in(&real_sensorDataFifo, (struct SPI_PACKET *)data_temp, 1);
                list_for_each_entry(real_proc, &real_driver_procs, list)
                {
                    real_proc->dataBuffer->kfifo.in = real_sensorDataFifo.kfifo.in;
                }                
                wake_up_module = 1;
            }
            else
            {
                printk(KERN_ALERT"sensor_pack_num = 0x%x, sensor_state = 0x%x, sensor_staues = %d\n", \
                (header->data[5]<<8)|header->data[4], (header->data[7]<<8)|header->data[6], header->data[8]);
                if (kfifo_is_full(&sensorDataFifo))                //如果队列已满 推出头
                {
                    printk("apollo data full number=%d\r\n", fifo_full++);
                    kfifo_skip(&sensorDataFifo);
                }
                //将数据放入队列
                kfifo_in(&sensorDataFifo, (struct SPI_PACKET *)data_temp, 1);
                list_for_each_entry(proc, &driver_procs, list)
                {
                    proc->dataBuffer->kfifo.in = sensorDataFifo.kfifo.in;
                }
                wake_up_module = 2;
                /*LIAOGUANGWEI*/
            }

        }
        read_size += packet_size(&temp[read_size]);    
    }
    printk(KERN_ALERT"apollo_out_while\n");
    // if((fifo_len != 0)&&(temp != NULL))
    // {
    //    kfree(temp);
    // }
    // temp = NULL;
    mutex_unlock(&apollo_ctrl_mutex);
    //释放锁
    mt_eint_unmask(CUST_EINT_SENSORHUB_NUM);
    //唤醒读取等待队列
    if (wake_up_module == 1)
    {
        list_for_each_entry(real_proc, &real_driver_procs, list)
        {
            wake_up_all(&real_proc->wait);
        }        
    }
    else if (wake_up_module == 2)
    {
        list_for_each_entry(proc, &driver_procs, list)
        {
            wake_up_all(&proc->wait);
        }
    }

    local_irq_enable(); 
    /*
    //读取数据
    int read_err = spi_read_bytes_serial(apollo_spi_read_buffer_all, READ_PACKAGE_SIZE);
    if (read_err)
    {
        //如果读取失败释放锁，并返回
        mutex_unlock(&apollo_ctrl_mutex);
        //释放中断
        mt_eint_unmask(CUST_EINT_SENSORHUB_NUM);
        return;
    }
    //如果队列已满 推出头
    if (kfifo_is_full(&sensorDataFifo))
    {
        kfifo_skip(&sensorDataFifo);
    }
    struct proc_data *proc = NULL;
    //将数据放入队列
    kfifo_in(&sensorDataFifo, (struct SPI_PACKET *) apollo_spi_read_buffer_all, 1);
    list_for_each_entry(proc, &driver_procs, list)
    {
        proc->dataBuffer->kfifo.in = sensorDataFifo.kfifo.in;
    }
    mutex_unlock(&apollo_ctrl_mutex);
    //释放锁
    mt_eint_unmask(CUST_EINT_SENSORHUB_NUM);
    //唤醒读取等待队列
    list_for_each_entry(proc, &driver_procs, list)
    {
        wake_up_all(&proc->wait);
    }
    */
}

static void stm32_interrupt_callback(void)
{
    //schedule_work(&sensor->work);
    queue_work(system_highpri_wq, &read_sensorhub_work);
}

/**
 *
 *
 */
int apollo_probe(void *bus_dev)
{
    int error;
    int i = 0;

    APOLLO_INFO("%s", __func__);

    apollo_spi_read_buffer_all = kmalloc(APOLLO_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES, GFP_KERNEL);
    if (apollo_spi_read_buffer_all == NULL)
    {
        APOLLO_ERROR("error kmalloc fail apollo_spi_read_buffer_all");
        return -ENOMEM;
    }
    apollo_spi_write_buffer_all = kmalloc(APOLLO_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES, GFP_KERNEL);
    if (apollo_spi_write_buffer_all == NULL)
    {
        APOLLO_ERROR("error kmalloc fail apollo_spi_write_buffer_all");
        return -ENOMEM;
    }
    uint32_t real_fifo_page_size = get_enough_page_size(REAL_SENSOR_FIFO_DATA_SIZE);
    uint32_t fifo_page_size = get_enough_page_size(SENSOR_FIFO_DATA_SIZE);

    void *real_fifoBuffer = kzalloc(real_fifo_page_size, GFP_KERNEL);    
    void *fifoBuffer = kzalloc(fifo_page_size, GFP_KERNEL);
    if ((fifoBuffer == NULL) || (real_fifoBuffer == NULL))
    {
        APOLLO_ERROR("error kmalloc fail fifoBuffer\n");
        return -ENOMEM;
    }
    kfifo_init(&real_sensorDataFifo, real_fifoBuffer, REAL_SENSOR_FIFO_DATA_SIZE);
    kfifo_init(&sensorDataFifo, fifoBuffer, SENSOR_FIFO_DATA_SIZE);
    void *startAddr = fifoBuffer;
    void *endAddr = startAddr + fifo_page_size;
    while (startAddr != endAddr)
    {
        SetPageReserved(virt_to_page(startAddr));
        startAddr += PAGE_SIZE;
    }
    startAddr = real_fifoBuffer;
    endAddr = real_fifoBuffer + real_fifo_page_size;
    while (startAddr != endAddr)
    {
        SetPageReserved(virt_to_page(startAddr));
        startAddr += PAGE_SIZE;
    }    

    sync_file_init(bus_dev);
    xjktest_init(bus_dev);
    version_file_init(bus_dev);

    /**
    *注册中断
    */
    if (CUST_EINT_SENSORHUB_NUM > 0)
    {
        mt_set_gpio_mode(GPIO_SENSORHUB_EINT_PIN, GPIO_SENSORHUB_EINT_PIN_M_EINT);
        mt_set_gpio_dir(GPIO_SENSORHUB_EINT_PIN, GPIO_DIR_IN);
        mt_set_gpio_pull_enable(GPIO_SENSORHUB_EINT_PIN, GPIO_PULL_ENABLE);
        mt_set_gpio_pull_select(GPIO_SENSORHUB_EINT_PIN, GPIO_PULL_UP);

        mt_eint_registration(CUST_EINT_SENSORHUB_NUM, EINTF_TRIGGER_RISING, stm32_interrupt_callback, 0);
        mt_eint_unmask(CUST_EINT_SENSORHUB_NUM);
        INIT_WORK(&read_sensorhub_work, apollo_work_report);
    }
    return 0;
err_free_mem:
    //todo free mem
    return error;
}
int apollo_shutdown(void *bus_dev)
{
    free_irq(CUST_EINT_SENSORHUB_NUM, 0);
}

static int __init apollo_init(void)
{

    int ret = 0;
    APOLLO_INFO("APOLLO_init apollo_spi_register");
    ret = apollo_spi_register();
    if (ret < 0)
    {
        APOLLO_ERROR("apollo_init spi bus register error");
    }
    
    return ret;
}

static void __exit apollo_exit(void)
{
    apollo_spi_unregister();
}

module_init(apollo_init);
module_exit(apollo_exit);

MODULE_DESCRIPTION("xjk sensor Driver");
MODULE_AUTHOR("ncy");
MODULE_LICENSE("GPL");
