
#include <linux/module.h>
#include <linux/fs.h>

static int major = 0;

static struct file_operations hello_fops = {
	.owner		= THIS_MODULE,
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

