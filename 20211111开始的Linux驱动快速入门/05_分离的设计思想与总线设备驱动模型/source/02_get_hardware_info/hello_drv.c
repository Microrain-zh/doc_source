#include <linux/module.h>

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
#include <linux/platform_device.h>

static int major = 0;
static int ker_val = 123;

static int pin;

static ssize_t hello_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	int err;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	copy_to_user(buf, &ker_val, 4);
	return 4;
}

static ssize_t hello_write (struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
	int err;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	copy_from_user(&ker_val, buf, 4);

	if (ker_val)
	{
		/* 控制LED */
		gpio_xxx(pin, 1);
	}
	else
	{
		/* 控制LED */
		gpio_xxx(pin, 0);
	}

	
	return 4;
}



static struct file_operations hello_fops = {
	.owner		= THIS_MODULE,
	.read       = hello_read,
	.write      = hello_write,
};


static int hello_probe(struct platform_device *pdev)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

	/* 1. 获得资源 */
	pin = pdev->resource[0].start;

	/* 2. 注册file_operatoins */
	major = register_chrdev(0, "hello_drv", &hello_fops);
	
    return 0;
}

static int hello_remove(struct platform_device *pdev)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    return 0;
}


static struct platform_driver hello_driver = {
    .probe      = hello_probe,
    .remove     = hello_remove,
    .driver     = {
        .name   = "100ask_hello",
    },
};

static int __init hello_drv_init(void)
{
    int err;
    
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    err = platform_driver_register(&hello_driver); 
    
    return err;
}

static void __exit hello_drv_exit(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    platform_driver_unregister(&hello_driver);
}

module_init(hello_drv_init);
module_exit(lhello_drv_exit);

MODULE_LICENSE("GPL");

