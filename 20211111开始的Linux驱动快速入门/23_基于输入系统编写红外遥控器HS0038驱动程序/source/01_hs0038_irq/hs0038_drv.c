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
static struct class *hs0038_class;
static struct gpio_desc *hs0038_data_pin;
static struct gpio_desc *hs0038_test_pin;
static int irq;
static int hs0038_data = 0;  
static wait_queue_head_t hs0038_wq;
static u64 hs0038_edge_time[100];
static int hs0038_edge_cnt = 0;


static void hs0038_reset(void)
{
	gpiod_direction_output(hs0038_data_pin, 1);
	//gpiod_set_value(hs0038_data_pin, 1);
}

static void hs0038_start(void)
{
	//gpiod_direction_output(hs0038_data_pin, GPIOD_OUT_HIGH);
	//schedule_timeout(3*HZ);	
	mdelay(30);
	gpiod_set_value(hs0038_data_pin, 0);
	mdelay(20);
	gpiod_set_value(hs0038_data_pin, 1);
	udelay(40);	
	gpiod_direction_input(hs0038_data_pin);	
	//udelay(2);	
}

static int hs0038_wait_for_ready(void)
{
	int timeout_us = 20000;

	/* 等待低电平 */
	while (gpiod_get_value(hs0038_data_pin) && --timeout_us)
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
	
	while (!gpiod_get_value(hs0038_data_pin) && --timeout_us)
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
	while (gpiod_get_value(hs0038_data_pin) && --timeout_us)
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

int hs0038_parse_data(char *data)
{
	int i, j, m = 0;
	int offset = 0;

	for (offset = 0; offset <= 2; offset ++)
	{
		m = offset;
		
		for (i = 0; i < 5; i++)
		{
			data[i] = 0;
			for (j = 0; j < 8; j++)
			{
				data[i] <<= 1;
				if (hs0038_edge_time[m+1] - hs0038_edge_time[m] >= 40000)
					data[i] |= 1;
				m += 2;	
			}
		}

		if (data[4] == data[0] + data[1] + data[2] + data[3])
		{
			return 0;
		}
	}

	printk("%s %s line %d, hs0038_edge_cnt = %d\n", __FILE__, __FUNCTION__, __LINE__, hs0038_edge_cnt);
	return -1;
}
	

static irqreturn_t hs0038_isr(int irq, void *dev_id)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
	return IRQ_HANDLED; // IRQ_WAKE_THREAD;
}



/* 实现对应的open/read/write等函数，填入file_operations结构体                   */
static ssize_t hs0038_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	unsigned char data[5];
	int timeout;
	int err;

	if (size != 4)
		return -EINVAL;

	//gpiod_set_value(hs0038_test_pin, 0);

	/* 1. 发送高脉冲启动DHT11 */
	hs0038_reset();
	hs0038_start();

	hs0038_edge_cnt = 0;
	hs0038_data = 0;

	err = request_irq(irq, hs0038_isr, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, "100ASK_DHT11", NULL);

#if 0
	/* 2. 等待DHT11就绪 */
	if (hs0038_wait_for_ready())
	{
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		hs0038_reset();
		return -EAGAIN;
	}
#endif

	timeout = wait_event_timeout(hs0038_wq, hs0038_data, HZ);

	free_irq(irq, NULL);
	
	hs0038_reset();

#if 0
	if (!timeout)
	{
		printk("%s %s line %d, hs0038_edge_cnt = %d\n", __FILE__, __FUNCTION__, __LINE__, hs0038_edge_cnt);
		return -ETIMEDOUT;
	}
#endif

	if (!hs0038_parse_data(data))
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

static unsigned int hs0038_drv_poll(struct file *fp, poll_table * wait)
{
//	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
//	poll_wait(fp, &hs0038_wait, wait);
	return 0;
}



/* 定义自己的file_operations结构体                                              */
static struct file_operations hs0038_fops = {
	.owner	 = THIS_MODULE,
	.read    = hs0038_drv_read,
	.poll    = hs0038_drv_poll,
};




/* 1. 从platform_device获得GPIO
 * 2. gpio=>irq
 * 3. request_irq
 */
static int hs0038_probe(struct platform_device *pdev)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	/* 1. 获得硬件信息 */
	hs0038_data_pin = gpiod_get(&pdev->dev, NULL, 0);
	if (IS_ERR(hs0038_data_pin))
	{
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	}

	irq = gpiod_to_irq(hs0038_data_pin);

	request_irq(irq, hs0038_isr, IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING, "hs0038", NULL);

	/* 2. device_create */
	device_create(hs0038_class, NULL, MKDEV(major, 0), NULL, "myhs0038");

	return 0;
}

static int hs0038_remove(struct platform_device *pdev)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	device_destroy(hs0038_class, MKDEV(major, 0));
	free_irq(irq, NULL);
	gpiod_put(hs0038_data_pin);
//	gpiod_put(hs0038_test_pin);

	return 0;
}


static const struct of_device_id ask100_hs0038[] = {
    { .compatible = "100ask,hs0038" },
    { },
};

/* 1. 定义platform_driver */
static struct platform_driver hs0038_driver = {
    .probe      = hs0038_probe,
    .remove     = hs0038_remove,
    .driver     = {
        .name   = "100ask_hs0038",
        .of_match_table = ask100_hs0038,
    },
};

/* 2. 在入口函数注册platform_driver */
static int __init hs0038_init(void)
{
    int err;
    
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	/* 注册file_operations 	*/
	major = register_chrdev(0, "hs0038", &hs0038_fops);  

	hs0038_class = class_create(THIS_MODULE, "hs0038_class");
	if (IS_ERR(hs0038_class)) {
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "hs0038");
		return PTR_ERR(hs0038_class);
	}

	init_waitqueue_head(&hs0038_wq);

	
    err = platform_driver_register(&hs0038_driver); 
	
	return err;
}

/* 3. 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数
 *     卸载platform_driver
 */
static void __exit hs0038_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    platform_driver_unregister(&hs0038_driver);
	class_destroy(hs0038_class);
	unregister_chrdev(major, "hs0038");
}


/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(hs0038_init);
module_exit(hs0038_exit);

MODULE_LICENSE("GPL");


