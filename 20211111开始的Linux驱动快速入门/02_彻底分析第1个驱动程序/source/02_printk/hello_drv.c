
#include <linux/module.h>

int __init hello_init(void)
{
	printk("hello drv init\n");
	return 0;
}

void __exit hello_exit(void)
{
	printk("hello drv exit\n");
}

MODULE_LICENSE("GPL");


module_init(hello_init);
module_exit(hello_exit);

//int init_module(void) __attribute__((alias(hello_init)));

