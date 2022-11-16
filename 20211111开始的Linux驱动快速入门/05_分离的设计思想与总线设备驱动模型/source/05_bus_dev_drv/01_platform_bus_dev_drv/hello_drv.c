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



static int hello_probe(struct platform_device *pdev)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
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

static void __exit lhello_drv_exit(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    platform_driver_unregister(&hello_driver);
}

module_init(hello_drv_init);
module_exit(lhello_drv_exit);

MODULE_LICENSE("GPL");

