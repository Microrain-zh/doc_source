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
#include <linux/delay.h>
#include <linux/ktime.h>
#include <linux/version.h>

static int major;
static struct class *dht11_class;
static struct gpio_desc *dht11_data_pin;
static struct gpio_desc *dht11_test_pin;
static int irq;
static int dht11_data = 0;  
static wait_queue_head_t dht11_wq;
static u64 dht11_edge_time[82];
static int dht11_edge_cnt = 0;


static void dht11_reset(void)
{
	gpiod_direction_output(dht11_data_pin, 1);
	//gpiod_set_value(dht11_data_pin, 1);
}

static void dht11_start(void)
{
	//gpiod_direction_output(dht11_data_pin, GPIOD_OUT_HIGH);
	//schedule_timeout(3*HZ);	
	mdelay(30);
	gpiod_set_value(dht11_data_pin, 0);
	mdelay(20);
	gpiod_set_value(dht11_data_pin, 1);
	udelay(40);	
	gpiod_direction_input(dht11_data_pin);	
	udelay(2);	
}

static int dht11_wait_for_ready(void)
{
	int timeout_us = 20000;

	/* 等待低电平 */
	while (gpiod_get_value(dht11_data_pin) && --timeout_us)
	{
		udelay(1);
	}
	if (!timeout_us)
	{
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}

	/* 现在是低电平 */
	/* 等待高电平 */
	timeout_us = 200;
	
	while (!gpiod_get_value(dht11_data_pin) && --timeout_us)
	{
		udelay(1);
	}
	
	if (!timeout_us)
	{
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}

	/* 现在是高电平 */
	/* 等待低电平 */
	timeout_us = 200;
	while (gpiod_get_value(dht11_data_pin) && --timeout_us)
	{
		udelay(1);
	}
	if (!timeout_us)
	{
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}
	
	return 0;	
}

int dht11_parse_data(char *data)
{
	int i, j, m = 0;
		
	for (i = 0; i < 5; i++)
	{
		data[i] = 0;
		for (j = 0; j < 8; j++)
		{
			data[i] <<= 1;
			if (dht11_edge_time[m+1] - dht11_edge_time[m] >= 40000)
				data[i] |= 1;
			m += 2;	
		}
	}

	if (data[4] != data[0] + data[1] + data[2] + data[3])
	{
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}
	else
		return 0;
	
}

static irqreturn_t dht11_isr(int irq, void *dev_id)
{
	/* 1. 记录时间 */

	if (dht11_edge_cnt == 0)
		gpiod_set_value(dht11_test_pin, 1);


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0))
		dht11_edge_time[dht11_edge_cnt++] = ktime_get_boottime_ns();
#else
		dht11_edge_time[dht11_edge_cnt++] = ktime_get_boot_ns();
#endif

	
	if (dht11_edge_cnt >= 80)
	{	
		/* 2. 唤醒APP:去同一个链表把APP唤醒 */
		dht11_data = 1;
		wake_up(&dht11_wq);
	}
	
	return IRQ_HANDLED; // IRQ_WAKE_THREAD;
}



/* 实现对应的open/read/write等函数，填入file_operations结构体                   */
static ssize_t dht11_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	unsigned char data[5];
	int timeout;
	int err;

	if (size != 4)
		return -EINVAL;

	/* 1. 发送高脉冲启动DHT11 */
	dht11_reset();
	dht11_start();

	/* 2. 等待DHT11就绪 */
	if (dht11_wait_for_ready())
	{
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		dht11_reset();
		return -EAGAIN;
	}

	dht11_edge_cnt = 0;
	dht11_data = 0;

	gpiod_set_value(dht11_test_pin, 0);
	err = request_irq(irq, dht11_isr, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, "100ASK_DHT11", NULL);

	timeout = wait_event_timeout(dht11_wq, dht11_data, HZ);

	free_irq(irq, NULL);
	
	dht11_reset();

	if (!timeout)
	{
		printk("%s %s line %d, dht11_edge_cnt = %d\n", __FILE__, __FUNCTION__, __LINE__, dht11_edge_cnt);
		return -ETIMEDOUT;
	}


	if (!dht11_parse_data(data))
	{
		/* 5. copy_to_user */	
		/* data[0]/data[1] : 湿度 */
		/* data[2]/data[3] : 温度 */
		err = copy_to_user(buf, data, 4);

		return 4;
	}
	else
	{
		return -EAGAIN;
	}

}

static unsigned int dht11_drv_poll(struct file *fp, poll_table * wait)
{
//	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
//	poll_wait(fp, &dht11_wait, wait);
	return 0;
}



/* 定义自己的file_operations结构体                                              */
static struct file_operations dht11_fops = {
	.owner	 = THIS_MODULE,
	.read    = dht11_drv_read,
	.poll    = dht11_drv_poll,
};




/* 1. 从platform_device获得GPIO
 * 2. gpio=>irq
 * 3. request_irq
 */
static int dht11_probe(struct platform_device *pdev)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	/* 1. 获得硬件信息 */
	dht11_data_pin = gpiod_get(&pdev->dev, NULL, GPIOD_OUT_HIGH);
	if (IS_ERR(dht11_data_pin))
	{
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	}

	irq = gpiod_to_irq(dht11_data_pin);
	
	dht11_test_pin = gpiod_get(&pdev->dev, "test", GPIOD_OUT_HIGH);
	if (IS_ERR(dht11_test_pin))
	{
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	}

	//irq = gpiod_to_irq(dht11_echo);
	//request_irq(irq, dht11_isr, IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING, "dht11", NULL);

	/* 2. device_create */
	device_create(dht11_class, NULL, MKDEV(major, 0), NULL, "mydht11");

	return 0;
}

static int dht11_remove(struct platform_device *pdev)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	device_destroy(dht11_class, MKDEV(major, 0));
//	free_irq(irq, NULL);
	gpiod_put(dht11_data_pin);
	gpiod_put(dht11_test_pin);

	return 0;
}


static const struct of_device_id ask100_dht11[] = {
    { .compatible = "100ask,dht11" },
    { },
};

/* 1. 定义platform_driver */
static struct platform_driver dht11_driver = {
    .probe      = dht11_probe,
    .remove     = dht11_remove,
    .driver     = {
        .name   = "100ask_dht11",
        .of_match_table = ask100_dht11,
    },
};

/* 2. 在入口函数注册platform_driver */
static int __init dht11_init(void)
{
    int err;
    
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	/* 注册file_operations 	*/
	major = register_chrdev(0, "dht11", &dht11_fops);  

	dht11_class = class_create(THIS_MODULE, "dht11_class");
	if (IS_ERR(dht11_class)) {
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "dht11");
		return PTR_ERR(dht11_class);
	}

	init_waitqueue_head(&dht11_wq);

	
    err = platform_driver_register(&dht11_driver); 
	
	return err;
}

/* 3. 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数
 *     卸载platform_driver
 */
static void __exit dht11_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    platform_driver_unregister(&dht11_driver);
	class_destroy(dht11_class);
	unregister_chrdev(major, "dht11");
}


/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(dht11_init);
module_exit(dht11_exit);

MODULE_LICENSE("GPL");


