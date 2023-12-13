/*
 * Author: ncy
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/syscalls.h>
#include "apollo_sensorhub.h"

extern char *saved_command_line;

static int xjk_version_open(struct inode *inode, struct file *file)
{
    int rc = 0;

    return rc;
}

static int xjk_version_release(struct inode *inode, struct file *file)
{
    int rc = 0;

    return rc;
}

static int xjk_version_read(struct file *file, unsigned char *buf, size_t count)
{
    char jk_version[128] = {0};
    char *ptr_pl, *ptr_lk, *ptr_sn;
    ptr_pl = strstr(saved_command_line, "pl_version=");
    ptr_lk = strstr(saved_command_line, "lk_version=");
    ptr_sn = strstr(saved_command_line, "sn_version=");
    if (ptr_pl == NULL || ptr_lk == NULL || ptr_sn == NULL)
    {
        printk(KERN_ERR "xjk version error\n");
        return 0;
    }
    strncpy(jk_version, ptr_pl, ptr_sn - ptr_pl);
    sprintf(jk_version + strlen(jk_version), "%s%s%s", "kernel_version=", JK_KERNEL_VERSION, "end");
    printk("jk_version=%s,len=%d\n", jk_version, strlen(jk_version));
    copy_to_user(buf, jk_version, strlen(jk_version));
    return strlen(jk_version);
}

static const struct file_operations xjk_version_fops = {
    .owner = THIS_MODULE,
    .open = xjk_version_open,
    .release = xjk_version_release,
    .write = NULL,
    .read = xjk_version_read,
    .unlocked_ioctl = NULL
    };

static struct miscdevice xjk_version_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "xjk_version",
    .fops = &xjk_version_fops};

/**
 *
 *
 */
int version_file_init(void *bus_dev)
{
    int error;
    //注册心吉康版本信息字符设备
    error = misc_register(&xjk_version_misc);
    return error;
}