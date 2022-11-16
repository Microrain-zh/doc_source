
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


static void hello_dev_release(struct device *dev)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
}

#if 0
struct resource {
	resource_size_t start;
	resource_size_t end;
	const char *name;
	unsigned long flags;
	unsigned long desc;
	struct resource *parent, *sibling, *child;
};

#endif

static struct resource hello_dev_res[] = {
	{
		.start = 2, /* pin2 */
		.end   = 2, /* pin2 */
		.flags = IORESOURCE_TYPE_BITS,
	},
};


static struct platform_device hello_dev = {
        .name = "100ask_hello",
        .dev = {
                .release = hello_dev_release,
         },
        .num_resources = 1,
        .resource      = hello_dev_res,
};

static int __init hello_dev_init(void)
{
    int err;
    
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    err = platform_device_register(&hello_dev);   
    
    return err;
}

static void __exit hello_dev_exit(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    platform_device_unregister(&hello_dev);
}

module_init(hello_dev_init);
module_exit(hello_dev_exit);

MODULE_LICENSE("GPL");

