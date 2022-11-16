
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <linux/device.h>

static int major;
static struct class *led_class;

/* registers */
// RCC_PLL4CR地址：0x50000000 + 0x894
static volatile unsigned int *RCC_PLL4CR;

// RCC_MP_AHB4ENSETR 地址：0x50000000 + 0xA28
static volatile unsigned int *RCC_MP_AHB4ENSETR;

// GPIOA_MODER 地址：0x50002000 + 0x00
static volatile unsigned int *GPIOA_MODER;

// GPIOA_BSRR 地址： 0x50002000 + 0x18
static volatile unsigned int *GPIOA_BSRR;

static ssize_t led_write(struct file *filp, const char __user *buf,
			 size_t count, loff_t *ppos)
{
	char val;
	/* copy_from_user : get data from app */
	copy_from_user(&val, buf, 1);

	/* to set gpio register: out 1/0 */
	if (val)
	{
		/* set gpa10 to let led on */
		*GPIOA_BSRR = (1<<26);
	}
	else
	{
		/* set gpa10 to let led off */
		*GPIOA_BSRR = (1<<10);
	}
	return 1;
}

static int led_open(struct inode *inode, struct file *filp)
{
	/* enalbe PLL4, it is clock source for all gpio */
	*RCC_PLL4CR |= (1<<0);
	while ((*RCC_PLL4CR & (1<<1)) == 0);
	
	/* enable gpioA */
	*RCC_MP_AHB4ENSETR |= (1<<0);
	
	/*
	 * configure gpa10 as gpio
	 * configure gpio as output 
	 */
	*GPIOA_MODER &= ~(3<<20);
	*GPIOA_MODER |= (1<<20);

	return 0;
}

static struct file_operations led_fops = {
	.owner		= THIS_MODULE,
	.write		= led_write,
	.open		= led_open,
};

/* 入口函数 */
static int __init led_init(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	
	major = register_chrdev(0, "100ask_led", &led_fops);

	/* ioremap(base_phy, size); */
	// RCC_PLL4CR地址：0x50000000 + 0x894
	RCC_PLL4CR = ioremap(0x50000000 + 0x894, 4);
	
	// RCC_MP_AHB4ENSETR 地址：0x50000000 + 0xA28
	RCC_MP_AHB4ENSETR = ioremap(0x50000000 + 0xA28, 4);
	
	// GPIOA_MODER 地址：0x50002000 + 0x00
	GPIOA_MODER = ioremap(0x50002000 + 0x00, 4);
	
	// GPIOA_BSRR 地址： 0x50002000 + 0x18
	GPIOA_BSRR = ioremap(0x50002000 + 0x18, 4);

	led_class = class_create(THIS_MODULE, "myled");
	device_create(led_class, NULL, MKDEV(major, 0), NULL, "myled"); /* /dev/myled */
	
	return 0;
}

static void __exit led_exit(void)
{
	iounmap(RCC_PLL4CR);
	iounmap(RCC_MP_AHB4ENSETR);
	iounmap(GPIOA_MODER);
	iounmap(GPIOA_BSRR);
	
	device_destroy(led_class, MKDEV(major, 0));
	class_destroy(led_class);
	
	unregister_chrdev(major, "100ask_led");
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");


