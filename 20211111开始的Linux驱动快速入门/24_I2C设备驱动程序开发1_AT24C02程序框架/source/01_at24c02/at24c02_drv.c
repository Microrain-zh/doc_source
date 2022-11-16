#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/mod_devicetable.h>
#include <linux/log2.h>
#include <linux/bitops.h>
#include <linux/jiffies.h>
#include <linux/of.h>
#include <linux/acpi.h>
#include <linux/i2c.h>

static int at24c02_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

static int at24c02_remove(struct i2c_client *client)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

static const struct of_device_id at24c02_of_match[] = {
	{.compatible = "100ask,at24c02"},
	{}
};

static const struct i2c_device_id at24c02_ids[] = {
	{ "xxxxyyy",	(kernel_ulong_t)NULL },
	{ /* END OF LIST */ }
};

static struct i2c_driver at24c02_drv = {
	.driver = {
		.name = "my24c02",
		.of_match_table	 = at24c02_of_match,
	},
	.probe = at24c02_probe,
	.remove = at24c02_remove,
	.id_table = at24c02_ids,
};

static int at24c02_init(void)
{
	int err;
	err = i2c_add_driver(&at24c02_drv);
	printk("%s %s %d, err = %d\n", __FILE__, __FUNCTION__, __LINE__, err);
	return err;
}

static void at24c02_exit(void)
{
	i2c_del_driver(&at24c02_drv);
}

module_init(at24c02_init);
module_exit(at24c02_exit);

MODULE_LICENSE("GPL");


