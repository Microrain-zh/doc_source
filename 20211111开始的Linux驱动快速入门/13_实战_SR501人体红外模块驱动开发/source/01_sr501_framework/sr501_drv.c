#include <linux/module.h>
#include <linux/poll.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <asm/current.h>


/* 实现对应的open/read/write等函数，填入file_operations结构体                   */
static ssize_t sr501_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	
	return 0;
}

static unsigned int sr501_drv_poll(struct file *fp, poll_table * wait)
{
//	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
//	poll_wait(fp, &sr501_wait, wait);
	return 0;
}



/* 定义自己的file_operations结构体                                              */
static struct file_operations sr501_drv = {
	.owner	 = THIS_MODULE,
	.read    = sr501_drv_read,
	.poll    = sr501_drv_poll,
};


static irqreturn_t sr501_isr(int irq, void *dev_id)
{
	return IRQ_WAKE_THREAD;
}



/* 1. 从platform_device获得GPIO
 * 2. gpio=>irq
 * 3. request_irq
 */
static int sr501_probe(struct platform_device *pdev)
{
}

static int sr501_remove(struct platform_device *pdev)
{
}


static const struct of_device_id ask100_sr501[] = {
    { .compatible = "100ask,sr501" },
    { },
};

/* 1. 定义platform_driver */
static struct platform_driver sr501s_driver = {
    .probe      = sr501_probe,
    .remove     = sr501_remove,
    .driver     = {
        .name   = "100ask_sr501",
        .of_match_table = ask100_sr501,
    },
};

/* 2. 在入口函数注册platform_driver */
static int __init sr501_init(void)
{
    int err;
    
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
    err = platform_driver_register(&sr501s_driver); 
	
	return err;
}

/* 3. 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数
 *     卸载platform_driver
 */
static void __exit sr501_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    platform_driver_unregister(&sr501s_driver);
}


/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(sr501_init);
module_exit(sr501_exit);

MODULE_LICENSE("GPL");


