#include <linux/init.h>
#include <linux/module.h>
#include <asm/gpio.h>
#include <linux/kernel.h>

static int   led_dev_init(void)
{
	printk("%s\n",__func__);

	return 0;
}

static void  led_dev_exit(void)
{
	printk("%s\n",__func__);

}

module_init(led_dev_init);
module_exit(led_dev_exit);
MODULE_AUTHOR("Test");
MODULE_DESCRIPTION("led driver");
MODULE_LICENSE("GPL");
