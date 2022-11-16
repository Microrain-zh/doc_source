#include<linux/init.h>    
#include<linux/kernel.h>  
#include<linux/module.h>  
  
static int __init hello_init(void)
{
    printk("hello driver init\n");
    return 0;
}
 
static void __exit hello_exit(void)
{
    printk("hello driver exit\n");
}
module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");

