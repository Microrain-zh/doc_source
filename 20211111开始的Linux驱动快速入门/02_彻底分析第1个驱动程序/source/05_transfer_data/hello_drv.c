#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
//#include <asm/uaccess.h>

static int major = 0;
static int ker_val = 123;

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
	return 4;
}



static struct file_operations hello_fops = {
	.owner		= THIS_MODULE,
	.read       = hello_read,
	.write      = hello_write,
};

int __init hello_init(void)
{
	printk("hello drv init\n");
	major = register_chrdev(0, "hello_drv", &hello_fops);
	return 0;
}

void __exit hello_exit(void)
{
	printk("hello drv exit\n");
	unregister_chrdev(major, "hello_drv");
}

MODULE_LICENSE("GPL");


module_init(hello_init);
module_exit(hello_exit);

//int init_module(void) __attribute__((alias(hello_init)));

