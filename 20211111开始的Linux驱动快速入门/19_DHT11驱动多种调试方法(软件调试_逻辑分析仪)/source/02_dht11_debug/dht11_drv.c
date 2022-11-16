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
static int irq;
static int dht11_data = 0;  
static wait_queue_head_t dht11_wq;

int us_array[40];
int us_index;
int us_low_array[40];
int us_low_index;

int ready_high_pre_us = 0;
int ready_low_cycle = 0;
int ready_high_us = 0;
int read_gpio_ns = 0;

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
	u64 pre, last;

	/* 等待低电平 */
	while (gpiod_get_value(dht11_data_pin) && --timeout_us)
	{
		udelay(1);
		ready_high_pre_us++;
	}
	if (!timeout_us)
	{
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}

	/* 现在是低电平 */
	/* 等待高电平 */
	timeout_us = 200;
	
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0))
	pre = ktime_get_boottime_ns();
#else
	pre = ktime_get_boot_ns();
#endif
	while (!gpiod_get_value(dht11_data_pin) && --timeout_us)
	{
		//udelay(1);
		ready_low_cycle++;
	}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0))
	last = ktime_get_boottime_ns();
#else
	last = ktime_get_boot_ns();
#endif

	read_gpio_ns = (last - pre);
	
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
		ready_high_us++;
	}
	if (!timeout_us)
	{
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}
	
	return 0;	
}


static int dht11_read_byte(unsigned char *buf)
{
	int i;
	int us = 0;
	unsigned char data = 0;
	int timeout_us = 200;
	
	for (i = 0; i <8; i++)
	{
		/* 现在是低电平 */
		/* 等待高电平 */
		timeout_us = 400;
		us = 0;
		while (!gpiod_get_value(dht11_data_pin) && --timeout_us)
		{
			udelay(1);
			us++;
		}
		if (!timeout_us)
		{
			printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
			return -1;
		}
		us_low_array[us_low_index++] = us;

		/* 现在是高电平 */
		/* 等待低电平,累加高电平的时间 */
		timeout_us = 200;
		us = 0;
		while (gpiod_get_value(dht11_data_pin) && --timeout_us)
		{
			udelay(1);
			us++;
		}
		if (!timeout_us)
		{
			return -1;
		}
		us_array[us_index++] = us;
		if (us > 40)
		{
			/* get bit 1 */
			data = (data << 1) | 1;
		}
		else
		{
			/* get bit 0 */
			data = (data << 1) | 0;
		}

	}

	*buf = data;
	return 0;
}

/* 实现对应的open/read/write等函数，填入file_operations结构体                   */
static ssize_t dht11_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	unsigned long flags;
	int i;
	unsigned char data[5];

	if (size != 4)
		return -EINVAL;
	
	local_irq_save(flags);	  // 关中断
	us_index = 0;
	us_low_index = 0;

	ready_high_pre_us = 0;
	ready_low_cycle = 0;
	ready_high_us = 0;

	/* 1. 发送高脉冲启动DHT11 */
	dht11_reset();
	dht11_start();

	/* 2. 等待DHT11就绪 */
	if (dht11_wait_for_ready())
	{
		local_irq_restore(flags); // 恢复中断
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		return -EAGAIN;
	}

	/* 3. 读5字节数据 */
	for (i = 0; i < 5; i++)
	{
		if (dht11_read_byte(&data[i]))
		{
			local_irq_restore(flags); // 恢复中断
			printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
			return -EAGAIN;
		}
	}

	dht11_reset();
	
	local_irq_restore(flags); // 恢复中断

	/* 4. 根据校验码验证数据 */
	if (data[4] != (data[0] + data[1] + data[2] + data[3]))
	{
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}
	printk("wait for reay time: ready_high_pre_us = %d, ready_low ns = %d/%d, ready_high_us = %d\n", ready_high_pre_us, read_gpio_ns, ready_low_cycle, ready_high_us);
#if 0

	printk("get low us: \n");
	for (i  =0; i < 40; i++)
		printk("%d ", us_low_array[i]);
	printk("\n");

	printk("get high us: \n");
	for (i  =0; i < 40; i++)
		printk("%d ", us_array[i]);
	printk("\n");
#endif
	/* 5. copy_to_user */	
	/* data[0]/data[1] : 湿度 */
	/* data[2]/data[3] : 温度 */
	copy_to_user(buf, data, 4);
	return 4;
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


static irqreturn_t dht11_isr(int irq, void *dev_id)
{
	/* 2. 唤醒APP:去同一个链表把APP唤醒 */
	wake_up(&dht11_wq);
	
	return IRQ_HANDLED; // IRQ_WAKE_THREAD;
}



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


