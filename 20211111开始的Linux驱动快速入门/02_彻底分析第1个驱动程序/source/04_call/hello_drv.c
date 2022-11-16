
#include <linux/module.h>
#include <linux/fs.h>

static int major = 0;

static ssize_t hello_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	int err;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 1;
}

static ssize_t hello_write (struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
	int err;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 1;
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

