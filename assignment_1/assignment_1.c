#include<linux/init.h>
#include<linux/module.h>

int module_1_init(void)
{
	printk(KERN_ALERT "Hello Kernel World\n");
	return 0;
}

int module_1_exit(void)
{
	printk(KERN_ALERT "BBye\n");
}

module_init(module_1_init);
module_exit(module_1_exit);
