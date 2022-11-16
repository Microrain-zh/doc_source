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
static unsigned int hs0038_data = 0;  
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

/* 0 : 成功, *val中记录数据
 * -1: 没接收完毕
 * -2: 解析错误
 */
int hs0038_parse_data(unsigned int *val)
{
	u64 tmp;
	unsigned char data[4];
	int i, j, m;
	
	/* 判断是否重复码 */
	if (hs0038_edge_cnt == 3)
	{
		tmp = hs0038_edge_time[1] - hs0038_edge_time[0];
		if (tmp > 8000000 && tmp < 10000000)
		{
			tmp = hs0038_edge_time[2] - hs0038_edge_time[1];
			if (tmp < 3000000)
			{
				/* 获得了重复码 */
				*val = hs0038_data;
				return 0;
			}
		}
	}

	/* 接收到了66次中断 */
	m = 3;
	if (hs0038_edge_cnt >= 66)
	{
		/* 解析到了数据 */
		for (i = 0; i < 4; i++)
		{
			data[i] = 0;
			/* 先接收到bit0 */
			for (j = 0; j < 8; j++)
			{
				/* 数值: 1 */	
				if (hs0038_edge_time[m+1] - hs0038_edge_time[m] > 1000000)
					data[i] |= (1<<j);
			}
		}

		/* 检验数据 */
		if (data[0] != ~data[1])
			return -2;

		if ((data[2] & 0x7f) != (~data[3] & 0x7f))
			return -2;

		hs0038_data = (data[0] << 8) | (data[2]);
		*val = hs0038_data;
		return 0;
	}
	else
	{
		/* 数据没接收完毕 */
		return -1;
	}	
}
	

static irqreturn_t hs0038_isr(int irq, void *dev_id)
{
	unsigned int val;
	int ret;
	//printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0))
		hs0038_edge_time[hs0038_edge_cnt++] = ktime_get_boottime_ns();
#else
		hs0038_edge_time[hs0038_edge_cnt++] = ktime_get_boot_ns();
#endif

	/* 判断超时 */
	
	if (hs0038_edge_cnt >= 2)
	{
		if (hs0038_edge_time[hs0038_edge_cnt-1] - hs0038_edge_time[hs0038_edge_cnt-2] > 100000000)
		{
			/* 超时 */
			hs0038_edge_time[0] = hs0038_edge_time[hs0038_edge_cnt-1];
			hs0038_edge_cnt = 1;			
			return IRQ_HANDLED; // IRQ_WAKE_THREAD;
		}
	}

	ret = hs0038_parse_data(&val);
	if (!ret)
	{
		/* 解析成功 */
		hs0038_edge_cnt = 0;
		printk("get ir code = 0x%x\n", val);		
	}
	else if (ret == -2)
	{
		/* 解析失败 */
		hs0038_edge_cnt = 0;
	}
	
	return IRQ_HANDLED; // IRQ_WAKE_THREAD;
}



/* 实现对应的open/read/write等函数，填入file_operations结构体                   */
static ssize_t hs0038_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	return 0;
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


