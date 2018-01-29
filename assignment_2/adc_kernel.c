#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/types.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/cdev.h>
#include<linux/random.h>
#include<asm/uaccess.h>

#define SET_CHANNEL _IOW(0,0,unsigned long)

static dev_t adc;
static struct cdev c_dev;
static struct class *cls;

static int channel_no;
static int ret_buffer;
static void *ptr_buffer;

static int adc_open(struct inode *i,struct file *f)
{
	printk(KERN_INFO "Function: OPEN()\n");
	return 0;
}

static int adc_close(struct inode *i,struct file *f)
{
	printk(KERN_INFO "Function: CLOSE()\n");
	return 0;
}

static ssize_t adc_read(struct file *f,char __user *buf,size_t len,loff_t *off)
{
	char return_buffer[2];
	get_random_bytes(return_buffer,2);
    	printk(KERN_INFO "Function: READ()\n");
    	put_user(return_buffer[0],buf);
    	put_user(return_buffer[1],buf+1);
	return 0;
}

static ssize_t adc_write(struct file *f,const char __user *buf,size_t len,loff_t *off)
{
	printk(KERN_INFO "Function: WRITE()\n");
	return 0;
}

static long adc_ioctl(struct file *file,unsigned int command,unsigned long arg)
{
	printk(KERN_INFO "Function: IOCTL()\n");
    	switch(command)
    	{
       		case SET_CHANNEL:
        	channel_no=arg;
    	}
    	printk(KERN_INFO "ADC Channel changed to %d\n",channel_no);
	return 0;
}

struct file_operations fops=
{
	.owner=THIS_MODULE,
    	.open=adc_open,
    	.release=adc_close,
    	.read=adc_read,
	.write=adc_write,
    	.unlocked_ioctl=adc_ioctl
};

static int __init adc_init(void)
{
    	ret_buffer=alloc_chrdev_region(&adc,0,1,"ADCDEVICE");
    	if(ret_buffer<0)
    	{
    	    return -1;
    	}
    	cls=class_create(THIS_MODULE,"adcclass");
    	if(cls==NULL)
    	{
        	unregister_chrdev_region(adc,1);
        	return -1;
    	}
    	ptr_buffer=device_create(cls,NULL,adc,NULL,"adcdevice");
    	if(ptr_buffer==NULL)
    	{
        	class_destroy(cls);
        	unregister_chrdev_region(adc,1);
        	return -1;
    	}
    	cdev_init(&c_dev,&fops);
    	ret_buffer=cdev_add(&c_dev,adc,1);
    	if(ret_buffer<0)
    	{
        	device_destroy(cls,adc);
        	class_destroy(cls);
        	unregister_chrdev_region(adc,1);
        	return -1;
    	}
    	printk(KERN_INFO "ADC driver registered succesfully with MAJOR NO: %d and MINOR NO: %d\n",MAJOR(adc),MINOR(adc));
    	channel_no=0;/*Channel set to 0 by default*/
    	return 0;
}
static void __exit adc_exit(void)
{
    	cdev_del(&c_dev);
    	device_destroy(cls,adc);
    	class_destroy(cls);
    	unregister_chrdev_region(adc,1);
    	printk(KERN_INFO "ADC driver unregisterd succesfully\n");
}
module_init(adc_init);
module_exit(adc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jainil Sanghvi");
MODULE_DESCRIPTION("Demo for char device driver emulating ADC");
