/* apollo_sensorhub.h - header file for apollo sensorhub
 * Author: ncy
 */
#ifndef __APOLLO_SENSORHUB_H__
#define __APOLLO_SENSORHUB_H__
#include <linux/ioctl.h>
#include <linux/sensors_io.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/kfifo.h>

#define APOLLO_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES 0x400
#define APOLLO_NAME "apollo"
#define LOG_TAG_KERNEL "APOLLOKernel"
#define LOG_TAG_MCU "APOLLO"
#define COMMAD_HEAD_1 0x1b
#define COMMAD_HEAD_2 0xdf
#define JK_KERNEL_VERSION "0.0.0.30"
#define REAL_FIFO_CACHE_SIZE	32
#define FIFO_CACHE_SIZE 256
#define READ_PACKAGE_SIZE 1024
#define REAL_SENSOR_FIFO_DATA_SIZE 	(sizeof(struct sensor_kfifo)*REAL_FIFO_CACHE_SIZE)
#define SENSOR_FIFO_DATA_SIZE (sizeof(struct sensor_kfifo)*FIFO_CACHE_SIZE)

/* turn on gpio interrupt if defined */
struct SPI_PACKET
{
    uint8_t data[READ_PACKAGE_SIZE];
};

struct sensor_kfifo __STRUCT_KFIFO_PTR(struct SPI_PACKET, 0, struct SPI_PACKET);

struct proc_data
{
    /* data */
    struct sensor_kfifo *dataBuffer;
    wait_queue_head_t wait;
    struct list_head list;
};

struct spi_package_head {
    uint8_t magic;   //aa 
    uint8_t cmd_id;  //20
    uint16_t data_length; //len
    uint8_t data[1]; //
};

struct user_ioc_transfer {
	__u64 tx_buf;
	__u64 rx_buf;

	__u32		len;
	__u32		command;

};


struct sensorhub_ioc_transfer {
	void* tx_buf;
	void* rx_buf;

	__u32		len;
	__u32		speed_hz;

	__u16		delay_usecs;
	__u8		bits_per_word;
	__u8		cs_change;
	__u8		tx_nbits;
	__u8		rx_nbits;
	__u16		pad;

}; 

struct sensorhub_spi_transfer {
	__u64 tx_buf;
	__u64 rx_buf;
	__u32		len;
	__u8 isUserTransfer;
};


#define APOLLO_INFO(fmt,arg...)         printk("-APOLLO_INF- "fmt"\n",##arg)
#define APOLLO_ERROR(fmt,arg...)        printk("-APOLLO_ERR- "fmt"\n",##arg)
#define APOLLO_DEBUG(fmt,arg...)		printk("-APOLLO_DBG- "fmt"\n",##arg)

uint32_t get_enough_page_size(uint32_t size);
int apollo_spi_register(void);
int apollo_spi_unregister(void);
int spi_write_bytes_serial(unsigned char *buffer, int len);
int spi_read_bytes_serial(unsigned char *buffer, int len);
int apollo_write_then_read(uint8_t *txbuf,uint32_t n_tx,uint8_t *rxbuf,uint32_t n_rx);
int apollo_probe(void *bus_dev);
int apollo_shutdown(void *bus_dev);
int sync_file_init(void *bus_dev);
int xjktest_init(void *bus_dev);
int version_file_init(void *bus_dev);
bool is_packet_ok(uint8_t *data,uint16_t count);
uint16_t packet_size(uint8_t *data);
int user_read_ioc(struct user_ioc_transfer *user_ioc);
int user_write_ioc(struct user_ioc_transfer *user_ioc);
#endif /* __APOLLO_SENSORHUB_H__ */
