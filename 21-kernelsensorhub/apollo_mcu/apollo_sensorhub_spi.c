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
#include <linux/gpio.h>
#include <mach/mt_gpio.h>
#include <cust_gpio_usage.h>
#include <cust_eint.h>
#include <mach/eint.h>
#include <mach/mt_pm_ldo.h>
#include <linux/spi/spi.h>
#include <mach/mt_spi.h>
#include <mach/mt_sleep.h>
#include <linux/kfifo.h>
#include "apollo_sensorhub.h"
#include <asm/uaccess.h>



static struct mt_chip_conf spi_conf;
static struct spi_device *spidev;
#define SPI_BUFFER_SIZE (4*1024)
struct spidev_data
{
	spinlock_t spi_lock;
	struct spi_device *spi;
	struct mutex buf_lock;
	u8 *buffer;
	u8 *bufferrx;
};

static struct spidev_data *spi_dev;


void spi_io_enable(int enable)
{
	if (enable)
	{
		mt_set_gpio_mode(GPIO_SPI_CS_PIN, GPIO_SPI_CS_PIN_M_SPI_CS);
		mt_set_gpio_mode(GPIO_SPI_SCK_PIN, GPIO_SPI_SCK_PIN_M_SPI_SCK);
		mt_set_gpio_mode(GPIO_SPI_MISO_PIN, GPIO_SPI_MISO_PIN_M_SPI_MISO);
		mt_set_gpio_mode(GPIO_SPI_MOSI_PIN, GPIO_SPI_MOSI_PIN_M_SPI_MOSI);
	}
	else
	{
		//set dir pull to save power
		mt_set_gpio_mode(GPIO_SPI_CS_PIN, GPIO_SPI_CS_PIN_M_GPIO);
		mt_set_gpio_dir(GPIO_SPI_CS_PIN, GPIO_DIR_IN);
		mt_set_gpio_pull_enable(GPIO_SPI_CS_PIN, GPIO_PULL_DISABLE);

		mt_set_gpio_mode(GPIO_SPI_SCK_PIN, GPIO_SPI_SCK_PIN_M_GPIO);
		mt_set_gpio_dir(GPIO_SPI_SCK_PIN, GPIO_DIR_IN);
		mt_set_gpio_pull_enable(GPIO_SPI_SCK_PIN, GPIO_PULL_DISABLE);

		mt_set_gpio_mode(GPIO_SPI_MISO_PIN, GPIO_SPI_MISO_PIN_M_GPIO);
		mt_set_gpio_dir(GPIO_SPI_MISO_PIN, GPIO_DIR_IN);
		mt_set_gpio_pull_enable(GPIO_SPI_MISO_PIN, GPIO_PULL_DISABLE);

		mt_set_gpio_mode(GPIO_SPI_MOSI_PIN, GPIO_SPI_MOSI_PIN_M_GPIO);
		mt_set_gpio_dir(GPIO_SPI_MOSI_PIN, GPIO_DIR_IN);
		mt_set_gpio_pull_enable(GPIO_SPI_MOSI_PIN, GPIO_PULL_DISABLE);
	}
}

static int spi_xfer(unsigned char *txbuf, unsigned char *rxbuf, int len)
{
	// mutex_lock(&spi_dev->buf_lock);
	int ret;
	struct spi_transfer transfer_1[2];
	int const pkt_count = len / APOLLO_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES;
	int const remainder = len % APOLLO_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES;
	struct spi_message msg;
	spi_message_init(&msg);
    uint8_t i=0;

	// printk(KERN_ALERT"apollo send datal:\n");
	// for(; i<len; i++)
	// {
	// 	printk(KERN_ALERT"0x%x ", txbuf[i]);
	// }
	// printk("\r\n");

	spi_io_enable(1);
	if (len > APOLLO_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES)
	{
		transfer_1[0].tx_buf = spi_dev->buffer;
		transfer_1[0].rx_buf = spi_dev->bufferrx;
		transfer_1[0].len = APOLLO_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES * pkt_count;
		memcpy(spi_dev->buffer, txbuf, transfer_1[0].len);
		spi_message_add_tail(&transfer_1[0], &msg);

		if (0 != remainder)
		{
			transfer_1[1].tx_buf = spi_dev->buffer;
			transfer_1[1].rx_buf = spi_dev->bufferrx;
			transfer_1[1].len = remainder;
			memcpy(spi_dev->buffer, txbuf, transfer_1[1].len);
			spi_message_add_tail(&transfer_1[1], &msg);
		}
	}
	else
	{
		//transfer_1[0].tx_buf = (txbuf == NULL) ? NULL : txbuf;
		//transfer_1[0].rx_buf = (rxbuf == NULL) ? NULL : rxbuf;
		transfer_1[0].tx_buf = spi_dev->buffer;
		transfer_1[0].rx_buf = spi_dev->bufferrx;
		transfer_1[0].len = len;
		memcpy(spi_dev->buffer, txbuf, len);
		spi_message_add_tail(&transfer_1[0], &msg);
	}
	if (spi_sync(spidev, &msg))
		ret = -1;
	else
		ret = 0;
	spi_io_enable(0);
    // mutex_unlock(&spi_dev->buf_lock);  
	return ret;
}

void spi_init(void *dev)
{
	struct mt_chip_conf *spi_par;

	spidev = dev;

	spi_io_enable(1);

	spidev->controller_data = (void *)&spi_conf;
	spi_par = &spi_conf;
	if (!spi_par)
	{
		APOLLO_ERROR("spi config fail");
	}

	spi_par->setuptime = 30;
	spi_par->holdtime = 30;
	spi_par->high_time = 30; //10--6m   15--4m   20--3m  30--2m  [ 60--1m 120--0.5m  300--0.2m]
	spi_par->low_time = 30;
	spi_par->cs_idletime = 20;

	spi_par->rx_mlsb = 1;
	spi_par->tx_mlsb = 1;
	spi_par->tx_endian = 0;
	spi_par->rx_endian = 0;

	spi_par->cpol = 0;
	spi_par->cpha = 0;
	spi_par->com_mod = DMA_TRANSFER;

	spi_par->pause = 0;
	spi_par->finish_intr = 1;
	spi_par->deassert = 0;

	if (spi_setup(spidev))
	{
		APOLLO_ERROR("spi_setup fail");
	}

	struct spidev_data *spi_data;

	/* Allocate driver data */
	spi_data = kzalloc(sizeof(*spidev), GFP_KERNEL);
	if (!spi_data)
		return -ENOMEM;

	/* Initialize the driver data */
	spi_data->spi = spidev;
	spin_lock_init(&spi_data->spi_lock);
	mutex_init(&spi_data->buf_lock);
	if (!spi_data->buffer)
	{
		spi_data->buffer = kmalloc(SPI_BUFFER_SIZE, GFP_KERNEL);
		if (!spi_data->buffer)
		{
			return  -ENOMEM;
		}
	}

	if (!spi_data->bufferrx)
	{
		spi_data->bufferrx = kmalloc(SPI_BUFFER_SIZE, GFP_KERNEL);
		if (!spi_data->bufferrx)
		{
			kfree(spi_data->buffer);
			spi_data->buffer = NULL;
			return -ENOMEM;
		}
	}

	spi_dev = spi_data;

	spi_io_enable(0);
	//TODO
	//SPI DMA SETTING
}

int stm32_spi_probe(struct spi_device *spi)
{
    /**
    *给sensorhub 上电
    */
    hwPowerOn(MT6323_POWER_LDO_VCN33_WIFI, VOL_3600, "XJKSENSOR");
    mt_set_gpio_mode(GPIO_SENSORHUB_POWER_EN, GPIO_MODE_GPIO);
    mt_set_gpio_dir(GPIO_SENSORHUB_POWER_EN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_SENSORHUB_POWER_EN,1);

    msleep(200);

    hwPowerOn(MT6323_POWER_LDO_VGP3, VOL_1800, "TOUCHIC");
    mt_set_gpio_mode(GPIO139, GPIO_MODE_GPIO);
    mt_set_gpio_dir(GPIO139, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO139, 0);   
    // 初始化硬件spi
   	spi_init(spi);
	apollo_probe(spi);
	// msleep(1000);
	// uint8_t test_buff[15] = {0x80, 0xAA, 0x20, 0x08, 0x1B, 0xDF, 0x00, 0x01, 0x14, 0x0F, 0x9B, 0xDF, 0x15, 0x86};
	// printk(KERN_ALERT"spi_write_bytes_seria\r\n");
	// spi_write_bytes_serial(test_buff, 15);
	return 0;
} 

int stm32_spi_remove(struct spi_device *spi)
{
	return 0;
}

int stm32_spi_suspend(struct spi_device *spi, pm_message_t mesg)
{
	//CWMCU_suspend(&spi->dev);
	return 0;
} 

int stm32_spi_resume(struct spi_device *spi)
{
	//CWMCU_resume(&spi->dev);
}

void stm32_spi_shutdown(struct spi_device *spi)
{

	mt_set_gpio_out(GPIO_SENSORHUB_POWER_EN,0);
	mt_set_gpio_mode(GPIO_SENSORHUB_POWER_EN, GPIO_MODE_GPIO);
	mt_set_gpio_dir(GPIO_SENSORHUB_POWER_EN, GPIO_DIR_IN);
	mt_set_gpio_pull_enable(GPIO_SPI_CS_PIN, GPIO_PULL_DISABLE);
	hwPowerDown(MT6323_POWER_LDO_VCN33_WIFI, "XJKSENSOR");
	mt_set_gpio_out(GPIO139,0);
	mt_set_gpio_mode(GPIO139, GPIO_MODE_GPIO);
	mt_set_gpio_dir(GPIO139, GPIO_DIR_IN);
	mt_set_gpio_pull_enable(GPIO139, GPIO_PULL_DISABLE);
	hwPowerDown(MT6323_POWER_LDO_VGP3,  "TOUCHIC");
	apollo_shutdown(spi);
}

static struct spi_board_info spi_stm32_boardinfo __initdata = {
	.modalias = APOLLO_NAME,
	.bus_num = 0,
	.chip_select = 0,
	.mode = SPI_MODE_3,
};

static struct spi_driver stm32_spi_driver = {
	.driver = {
		.name = APOLLO_NAME,
		.owner = THIS_MODULE,
	},
	.probe = stm32_spi_probe,
	.remove = stm32_spi_remove,
	.suspend = stm32_spi_suspend,
	.resume = stm32_spi_resume,
	.shutdown = stm32_spi_shutdown
};

/*======================================================== */

int apollo_spi_register(void)
{
	spi_register_board_info(&spi_stm32_boardinfo, 1);
	return spi_register_driver(&stm32_spi_driver);
}


int apollo_spi_unregister(void)
{

}

int spi_write_bytes_serial(unsigned char *buffer, int len)
{
	int ret = 0;
	unsigned char *tx_buf = NULL, *rx_buf = NULL;

	if (len > APOLLO_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES)
	{
		return -1;
	}
	if (buffer == NULL)
		return -1;

	tx_buf = buffer;
	rx_buf = NULL;
	ret = spi_xfer(tx_buf, rx_buf, len);

	return ret;
} 

int spi_read_bytes_serial(unsigned char *buffer, int len)
{
	int ret = 0;
	unsigned char *tx_buf = NULL, *rx_buf = NULL;

	if (len > APOLLO_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES)
	{
		return -1;
	}
	if (buffer == NULL)
		return -1;
	tx_buf = NULL;
	rx_buf = buffer;
	ret = spi_xfer(tx_buf, rx_buf, len);
	return ret;
}

int apollo_write_then_read(uint8_t *txbuf,uint32_t n_tx,uint8_t *rxbuf,uint32_t n_rx)
{
	// 	int ret;
	// struct spi_transfer transfer_1[2];
	// int const pkt_count = len / APOLLO_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES;
	// int const remainder = len % APOLLO_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES;
	// struct spi_message msg;
	// spi_message_init(&msg);
    // uint8_t i=0;

	// printk(KERN_ALERT"apollo send datal:\n");
	// for(; i<len; i++)
	// {
	// 	printk(KERN_ALERT"0x%x ", txbuf[i]);
	// }
	// printk("\r\n");

	// spi_io_enable(1);
	// if (len > APOLLO_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES)
	// {
	// 	transfer_1[0].tx_buf = spi_dev->buffer;
	// 	transfer_1[0].rx_buf = spi_dev->bufferrx;
	// 	transfer_1[0].len = APOLLO_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES * pkt_count;
	// 	memcpy(spi_dev->buffer, txbuf, transfer_1[0].len);
	// 	spi_message_add_tail(&transfer_1[0], &msg);

	// 	if (0 != remainder)
	// 	{
	// 		transfer_1[1].tx_buf = spi_dev->buffer;
	// 		transfer_1[1].rx_buf = spi_dev->bufferrx;
	// 		transfer_1[1].len = remainder;
	// 		memcpy(spi_dev->buffer, txbuf, transfer_1[1].len);
	// 		spi_message_add_tail(&transfer_1[1], &msg);
	// 	}
	// }
	// else
	// {
	// 	//transfer_1[0].tx_buf = (txbuf == NULL) ? NULL : txbuf;
	// 	//transfer_1[0].rx_buf = (rxbuf == NULL) ? NULL : rxbuf;
	// 	transfer_1[0].tx_buf = spi_dev->buffer;
	// 	transfer_1[0].rx_buf = spi_dev->bufferrx;
	// 	transfer_1[0].len = n_tx + n_rx;
	// 	memcpy(spi_dev->buffer, txbuf, n_rx);
	// 	spi_message_add_tail(&transfer_1[0], &msg);
	// }
	// if (spi_sync(spidev, &msg))
	// 	ret = -1;
	// else
	// 	ret = 0;
	// spi_io_enable(0);
	// return ret;
	int ret = 0;
	struct spi_transfer transfer[2];
	struct spi_message msg;
	spi_message_init(&msg);
	spi_io_enable(1);
	// mutex_lock(&spi_dev->buf_lock);
	transfer[0].tx_buf = spi_dev->buffer;
	transfer[0].rx_buf = spi_dev->bufferrx;
	// transfer[0].tx_buf = (txbuf == NULL) ? NULL : txbuf;
	// transfer[0].rx_buf = (rxbuf == NULL) ? NULL : rxbuf;
	transfer[0].len = n_tx + n_rx;
	memset(spi_dev->buffer,0,transfer[0].len);
	memset( spi_dev->bufferrx,0,transfer[0].len);
	memcpy(spi_dev->buffer,txbuf,n_tx);
	spi_message_add_tail(&transfer[0], &msg);
	ret = spi_sync(spidev, &msg);
	if(!ret)
	{
		memcpy(rxbuf,&transfer[0].rx_buf[n_tx],n_rx);
	}
	// mutex_unlock(&spi_dev->buf_lock);
	spi_io_enable(0);
	return ret;
}

int user_read_ioc(struct user_ioc_transfer *user_ioc)
{
	int ret = 0;
	struct spi_transfer transfer[2];
	struct spi_message msg;
	spi_message_init(&msg);
	spi_io_enable(1);
	mutex_lock(&spi_dev->buf_lock);
	transfer[0].tx_buf = spi_dev->buffer;
	transfer[0].rx_buf = spi_dev->bufferrx;
	transfer[0].len =  user_ioc->len + 1;
	memset(spi_dev->buffer,0,transfer[0].len);
	memset( spi_dev->bufferrx,0,transfer[0].len);
	spi_dev->buffer[0] = user_ioc->command;
	spi_message_add_tail(&transfer[0], &msg);
	ret = spi_sync(spidev, &msg);
	if(!ret)
	{
		__copy_to_user((u8 __user *)(uintptr_t) user_ioc->rx_buf, &spi_dev->bufferrx[1],user_ioc->len);
	}
	mutex_unlock(&spi_dev->buf_lock);
	spi_io_enable(0);
	return ret;
}

int user_write_ioc(struct user_ioc_transfer *user_ioc)
{
	int ret = 0;
	struct spi_transfer transfer[2];
	struct spi_message msg;
	spi_message_init(&msg);
	spi_io_enable(1);
	mutex_lock(&spi_dev->buf_lock);
	transfer[0].tx_buf = spi_dev->buffer;
	transfer[0].rx_buf = NULL;
	transfer[0].len =  user_ioc->len + 1;
	memset(spi_dev->buffer,0,transfer[0].len);
	memset( spi_dev->bufferrx,0,transfer[0].len);
	spi_dev->buffer[0] = user_ioc->command;
	copy_from_user(&spi_dev->buffer[1], (const u8 __user *)(uintptr_t) user_ioc->tx_buf,user_ioc->len);
	spi_message_add_tail(&transfer[0], &msg);
	ret = spi_sync(spidev, &msg);
	mutex_unlock(&spi_dev->buf_lock);
	spi_io_enable(0);
	return ret;
}
