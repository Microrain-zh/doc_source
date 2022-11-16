
#include <linux/module.h>

int init_module(void)
{
}

void cleanup_module(void)
{
}

MODULE_LICENSE("GPL");


//module_init(hello_init);

//int init_module(void) __attribute__((alias(hello_init)));

