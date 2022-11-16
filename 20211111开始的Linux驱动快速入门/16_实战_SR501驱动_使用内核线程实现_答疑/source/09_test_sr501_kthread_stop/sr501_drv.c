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
#include <linux/kthread.h>

static int major;
static struct class *sr501_class;
static struct gpio_desc *sr501_gpio;
static int irq;
static int sr501_data = 0;  
static wait_queue_head_t sr501_wq;
static struct task_struct *sr501_kthread;


/* 实现对应的open/read/write等函数，填入file_operations结构体                   */
static ssize_t sr501_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
#if 0
	int val;
	int len = (size < 4)? size : 4;
	
	val = gpiod_get_value(sr501_gpio); 
	copy_to_user(buf, &val, len);
	return len;
#else
	int len = (size < 4)? size : 4;
	/* 1. 有数据就copy_to_uesr */
	/* 2. 无数据就休眠:  放入某个链表 */

	wait_event_interruptible(sr501_wq, sr501_data);	
	copy_to_user(buf, &sr501_data, len);
	sr501_data = 0;
	return len;
#endif
}

static unsigned int sr501_drv_poll(struct file *fp, poll_table * wait)
{
//	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
//	poll_wait(fp, &sr501_wait, wait);
	return 0;
}



/* 定义自己的file_operations结构体                                              */
static struct file_operations sr501_fops = {
	.owner	 = THIS_MODULE,
	.read    = sr501_drv_read,
	.poll    = sr501_drv_poll,
};


static irqreturn_t sr501_isr(int irq, void *dev_id)
{
	int val = gpiod_get_value(sr501_gpio);
	/* 1. 记录数据 */
	sr501_data = 0x80 | val;
	printk("%s %s %d, val = 0x%x\n", __FILE__, __FUNCTION__, __LINE__, sr501_data);

	/* 2. 唤醒APP:去同一个链表把APP唤醒 */
	wake_up(&sr501_wq);
	
	return IRQ_HANDLED; // IRQ_WAKE_THREAD;
}



/* 1. 从platform_device获得GPIO
 * 2. gpio=>irq
 * 3. request_irq
 */
static int sr501_probe(struct platform_device *pdev)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	/* 1. 获得硬件信息 */
	sr501_gpio = gpiod_get(&pdev->dev, NULL, 0);
	gpiod_direction_input(sr501_gpio);

	irq = gpiod_to_irq(sr501_gpio);
	request_irq(irq, sr501_isr, IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING, "sr501", NULL);

	/* 2. device_create */
	device_create(sr501_class, NULL, MKDEV(major, 0), NULL, "sr501");

	return 0;
}

static int sr501_remove(struct platform_device *pdev)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	device_destroy(sr501_class, MKDEV(major, 0));
	free_irq(irq, NULL);
	gpiod_put(sr501_gpio);
	return 0;
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

static int sr501_thread_func(void *data)
{
	int cnt = 0;
	while (!kthread_should_stop()) 
	{
		printk("%s %s line %d, cnt = %d\n", __FILE__, __FUNCTION__, __LINE__, cnt++);
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(5*HZ);
	}
	return 0;
}

/* 2. 在入口函数注册platform_driver */
static int __init sr501_init(void)
{
    int err;
    
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	sr501_kthread = kthread_run(sr501_thread_func, NULL, "sr501d");
	return 0;
	
	/* 注册file_operations 	*/
	major = register_chrdev(0, "sr501", &sr501_fops);  

	sr501_class = class_create(THIS_MODULE, "sr501_class");
	if (IS_ERR(sr501_class)) {
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "sr501");
		return PTR_ERR(sr501_class);
	}

	init_waitqueue_head(&sr501_wq);

	
    err = platform_driver_register(&sr501s_driver); 
	
	return err;
}

/* 3. 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数
 *     卸载platform_driver
 */
static void __exit sr501_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	kthread_stop(sr501_kthread);
	return ;

    platform_driver_unregister(&sr501s_driver);
	class_destroy(sr501_class);
	unregister_chrdev(major, "sr501");
}


/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(sr501_init);
module_exit(sr501_exit);

MODULE_LICENSE("GPL");


