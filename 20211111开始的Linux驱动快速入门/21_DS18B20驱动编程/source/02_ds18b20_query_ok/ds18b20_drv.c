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
#include <linux/version.h>

static int major;
static struct class *ds18b20_class;
static struct gpio_desc *ds18b20_data_pin;

static int irq;
static int ds18b20_data = 0;  
static wait_queue_head_t ds18b20_wq;

void ds18b20_delay_us(int us)
{
	u64 pre, last;

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
	pre = ktime_get_boot_ns();
	while (1)
	{
		last = ktime_get_boot_ns();
		if (last - pre >= us * 1000)
			break;
	}
#else
	pre = ktime_get_boottime_ns();
	while (1)
	{
		last = ktime_get_boottime_ns();
		if (last - pre >= us * 1000)
			break;
	}
#endif
}

int ds18b20_wait_for_ack(void)
{
	int timeout_count = 500;

	/* 如果是高电平,等待 */
	while (gpiod_get_value(ds18b20_data_pin) && --timeout_count)
	{
		udelay(1);
	}

	if (!timeout_count)
		return -1;

	/* 现在是低电平 */
	timeout_count = 500;
	while (!gpiod_get_value(ds18b20_data_pin) && --timeout_count)
	{
		udelay(1);
	}

	if (!timeout_count)
		return -1;

	return 0;
}

static int ds18b20_reset(void)
{
	int ret;
	
	gpiod_direction_output(ds18b20_data_pin, 0);
	ds18b20_delay_us(480);  /* udelay不准确, 要用ktime */
	
	ret = gpiod_direction_input(ds18b20_data_pin);
	
	if (ds18b20_wait_for_ack())
		return -1;
	else
		return 0;
}

static void ds18b20_write_byte(unsigned char data)
{
	int i;
	for (i = 0; i <8; i++)
	{
		if (data & (1<<i))
		{
			/* 输出1 */
			gpiod_direction_output(ds18b20_data_pin, 0);
			ds18b20_delay_us(2);
			
			/* 设置为输入: 引脚默认为高 */
			gpiod_direction_input(ds18b20_data_pin);
			ds18b20_delay_us(60);
		}
		else
		{
			/* 输出0 */
			gpiod_direction_output(ds18b20_data_pin, 0);
			ds18b20_delay_us(60);

			/* 设置为输入: 引脚默认为高 */
			gpiod_direction_input(ds18b20_data_pin);
			ds18b20_delay_us(2);			
		}
	}
}

unsigned char ds18b20_read_byte(void)
{
	unsigned char data = 0;
	int i;

	for (i = 0; i < 8; i++)
	{
		gpiod_direction_output(ds18b20_data_pin, 0);
		ds18b20_delay_us(2);

		/* 设置为输入 */
		gpiod_direction_input(ds18b20_data_pin);

		/* 7us之后读引脚 */
		ds18b20_delay_us(7);
		if (gpiod_get_value(ds18b20_data_pin))
			data |= (1<<i);

		/* 读到数据后, 等待足够60us */
		ds18b20_delay_us(60);				
	}

	return data;
}

/* 实现对应的open/read/write等函数，填入file_operations结构体                   */
static ssize_t ds18b20_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	unsigned long flags;
	unsigned char tempL=0,tempH=0;
	unsigned int integer;
	unsigned char decimal1,decimal2,decimal;

	if (size != 5)
		return -EINVAL;

	local_irq_save(flags);	  // 关中断

	if (ds18b20_reset())
	{
		gpiod_direction_output(ds18b20_data_pin, 1);
		
		local_irq_restore(flags); // 恢复中断
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		return -ENODEV;
	}

	ds18b20_write_byte(0xcc); //忽略rom指令，直接使用功能指令
	ds18b20_write_byte(0x44); //温度转换指令

	/* 不能省略！ */
	gpiod_direction_output(ds18b20_data_pin, 1);

	local_irq_restore(flags); // 恢复中断

	//转换需要时间，延时1s
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(HZ); 

	local_irq_save(flags);	  // 关中断
	if (ds18b20_reset())
	{		
		gpiod_direction_output(ds18b20_data_pin, 1);
		local_irq_restore(flags); // 恢复中断
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		return -ENODEV;
	}
	
	ds18b20_write_byte(0xcc); //忽略rom指令，直接使用功能指令
	ds18b20_write_byte(0xbe); //读暂存器指令

	tempL = ds18b20_read_byte(); //读温度低8位
	tempH = ds18b20_read_byte(); //读温度高8位
	if (tempH > 0x7f)      							//最高位为1时温度是负
	{
		tempL    = ~tempL;         				    //补码转换，取反加一
		tempH    = ~tempH+1;      
		integer  = tempL/16+tempH*16;      			//整数部分
		decimal1 = (tempL&0x0f)*10/16; 			//小数第一位
		decimal2 = (tempL&0x0f)*100/16%10;			//小数第二位
		decimal  = decimal1*10+decimal2; 			//小数两位
	}
	else
	{
		integer  = tempL/16+tempH*16;      				//整数部分
		decimal1 = (tempL&0x0f)*10/16; 					//小数第一位
		decimal2 = (tempL&0x0f)*100/16%10;				//小数第二位
		decimal  = decimal1*10+decimal2; 				//小数两位
	}
		
	local_irq_restore(flags); // 恢复中断
	
	gpiod_direction_output(ds18b20_data_pin, 1);

	copy_to_user(buf, &integer, 4);
	copy_to_user(buf+4, &decimal, 1);

	return 5;
}

static unsigned int ds18b20_drv_poll(struct file *fp, poll_table * wait)
{
//	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
//	poll_wait(fp, &ds18b20_wait, wait);
	return 0;
}



/* 定义自己的file_operations结构体                                              */
static struct file_operations ds18b20_fops = {
	.owner	 = THIS_MODULE,
	.read    = ds18b20_drv_read,
	.poll    = ds18b20_drv_poll,
};



/* 1. 从platform_device获得GPIO
 * 2. gpio=>irq
 * 3. request_irq
 */
static int ds18b20_probe(struct platform_device *pdev)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	/* 1. 获得硬件信息 */
	ds18b20_data_pin = gpiod_get(&pdev->dev, NULL, GPIOD_OUT_HIGH);
	//irq = gpiod_to_irq(ds18b20_echo);
	//request_irq(irq, ds18b20_isr, IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING, "ds18b20", NULL);

	/* 2. device_create */
	device_create(ds18b20_class, NULL, MKDEV(major, 0), NULL, "myds18b20");

	return 0;
}

static int ds18b20_remove(struct platform_device *pdev)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	device_destroy(ds18b20_class, MKDEV(major, 0));
	gpiod_put(ds18b20_data_pin);
	return 0;
}


static const struct of_device_id ask100_ds18b20[] = {
    { .compatible = "100ask,ds18b20" },
    { },
};

/* 1. 定义platform_driver */
static struct platform_driver ds18b20_driver = {
    .probe      = ds18b20_probe,
    .remove     = ds18b20_remove,
    .driver     = {
        .name   = "100ask_ds18b20",
        .of_match_table = ask100_ds18b20,
    },
};

/* 2. 在入口函数注册platform_driver */
static int __init ds18b20_init(void)
{
    int err;
    
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	/* 注册file_operations 	*/
	major = register_chrdev(0, "ds18b20", &ds18b20_fops);  

	ds18b20_class = class_create(THIS_MODULE, "ds18b20_class");
	if (IS_ERR(ds18b20_class)) {
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "ds18b20");
		return PTR_ERR(ds18b20_class);
	}

	init_waitqueue_head(&ds18b20_wq);

	
    err = platform_driver_register(&ds18b20_driver); 
	
	return err;
}

/* 3. 有入口函数就应该有出口函数：卸载驱动程序时，就会去调用这个出口函数
 *     卸载platform_driver
 */
static void __exit ds18b20_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    platform_driver_unregister(&ds18b20_driver);
	class_destroy(ds18b20_class);
	unregister_chrdev(major, "ds18b20");
}


/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(ds18b20_init);
module_exit(ds18b20_exit);

MODULE_LICENSE("GPL");


