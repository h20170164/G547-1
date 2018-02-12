#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/types.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/cdev.h>
#include<asm/uaccess.h>

#define WRITE_COMMAND 	_IOW(0,0,unsigned long)
#define READ_STATUS 	_IOWR(0,1,unsigned long)
#define SLEEP		_IOW(0,2,unsigned long)
#define COMMAND_REG 	0x43
#define ADDRESS_MASK 	0xC0
#define CHANNEL_0 	0x40
#define CHANNEL_1 	0x41
#define CHANNEL_2 	0x42
#define READ_VALUE_CH_0 0xE2
#define READ_VALUE_CH_1 0xE4
#define READ_VALUE_CH_2 0xE8
#define READ_STATUS_CH_0 0xD2
#define READ_STATUS_CH_1 0xD4
#define READ_STATUS_CH_2 0xD8

static dev_t timer;
static struct cdev c_dev;
static struct class *cls;

static int ret_buffer;
static void *ptr_buffer;

unsigned char channel_no=0x80;
unsigned char status_value;
int reset_value=1193;

static int timer_open(struct inode *i,struct file *f)
{
	printk(KERN_INFO "Function: OPEN()\n");
	return 0;
}

static int timer_close(struct inode *i,struct file *f)
{
	printk(KERN_INFO "Function: CLOSE()\n");
	return 0;
}

static ssize_t timer_read(struct file *f,char __user *buf,size_t len,loff_t *off)
{
	char return_buffer[2]={0,0};
    	printk(KERN_INFO "Function: READ()\n");
	switch(channel_no)
	{
		case 0x00:
			outb_p(READ_VALUE_CH_0,COMMAND_REG);
			return_buffer[0]=inb(CHANNEL_0);
			return_buffer[1]=inb(CHANNEL_0);
			break;
		case 0x40:
			outb_p(READ_VALUE_CH_1,COMMAND_REG);
			return_buffer[0]=inb(CHANNEL_1);
			return_buffer[1]=inb(CHANNEL_1);
			break;
		case 0x80:
			outb_p(READ_VALUE_CH_2,COMMAND_REG);
			return_buffer[0]=inb(CHANNEL_2);
			return_buffer[1]=inb(CHANNEL_2);
			break;
	}
    	put_user(return_buffer[0],buf);
    	put_user(return_buffer[1],buf+1);
	return 0;
}

static ssize_t timer_write(struct file *f,const char __user *buf,size_t len,loff_t *off)
{
	printk(KERN_INFO "Function: WRITE()\n");
	return 0;
}

void delay_ms(void)
{
	outb_p(0xB0,COMMAND_REG);//channel 2 mode 0
	outb_p(((reset_value)&0xFF),CHANNEL_2);
	outb_p(((reset_value)>>8),CHANNEL_2);

	outb_p(READ_STATUS_CH_2,COMMAND_REG);//Send readback command
	status_value=inb(CHANNEL_2);
	while(!(status_value&0x80))//check output as high
	{
		status_value=inb(CHANNEL_2);//checks the output bit of the timer and waits until high	
	}
}

void sleep(unsigned long value)
{
	while(value!=0)
	{
		delay_ms();
		value--;
	}
}

static long timer_ioctl(struct file *file,unsigned int command,unsigned long arg)
{
	printk(KERN_INFO "Function: IOCTL()\n");
    	switch(command)
    	{
       		case WRITE_COMMAND:
			outb_p(arg,COMMAND_REG);
			channel_no=(arg&ADDRESS_MASK);
			break;
		case SLEEP:
			sleep(arg);
			break;
    	} 	
	return 0;
}

struct file_operations fops=
{
	.owner=THIS_MODULE,
    	.open=timer_open,
    	.release=timer_close,
    	.read=timer_read,
	.write=timer_write,
    	.unlocked_ioctl=timer_ioctl
};

static int __init timer_init(void)
{
    	ret_buffer=alloc_chrdev_region(&timer,0,1,"timer0driver");
    	if(ret_buffer<0)
    	{
    	    return -1;
    	}
    	cls=class_create(THIS_MODULE,"timerclass");
    	if(cls==NULL)
    	{
        	unregister_chrdev_region(timer,1);
        	return -1;
    	}
    	ptr_buffer=device_create(cls,NULL,timer,NULL,"timer0");
    	if(ptr_buffer==NULL)
    	{
        	class_destroy(cls);
        	unregister_chrdev_region(timer,1);
        	return -1;
    	}
    	cdev_init(&c_dev,&fops);
    	ret_buffer=cdev_add(&c_dev,timer,1);
    	if(ret_buffer<0)
    	{
        	device_destroy(cls,timer);
        	class_destroy(cls);
        	unregister_chrdev_region(timer,1);
        	return -1;
    	}
    	printk(KERN_INFO "TIMER driver registered succesfully with MAJOR NO: %d and MINOR NO: %d\n",MAJOR(timer),MINOR(timer));
    	return 0;
}

static void __exit timer_exit(void)
{
    	cdev_del(&c_dev);
    	device_destroy(cls,timer);
    	class_destroy(cls);
    	unregister_chrdev_region(timer,1);
    	printk(KERN_INFO "TIMER driver unregisterd succesfully\n");
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jainil Sanghvi");
MODULE_DESCRIPTION("Demo for IO mapped Hardware Access");
