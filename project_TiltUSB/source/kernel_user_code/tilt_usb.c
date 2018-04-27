#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/input-polldev.h>
#include <linux/usb.h>
#include <linux/spinlock.h>

#define CTRL_EP_OUT 0x00
#define CTRL_EP_IN 0x80

#define X_AXIS 0
#define Y_AXIS 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JD EEE G547");
MODULE_DESCRIPTION("TILT  USB DEVICE");


struct input_polled_dev *p_dev;
struct usb_device *device;

int retval;
int x_val,y_val,z_val;
void read(void);

struct data {
unsigned int buffer[2];
};
struct data d;

spinlock_t lock;

void read(void)
{
spin_lock(&(lock));	
retval = usb_control_msg(device,usb_rcvctrlpipe(device,0),X_AXIS,USB_TYPE_VENDOR|USB_DIR_IN,0,0,(unsigned int *)d.buffer,2,5000);
if(retval)
{
printk(KERN_INFO "Control Scan Msg X returned %d\n",retval);
}

printk(KERN_INFO "Buffer X %d %d\n",*d.buffer,*(d.buffer+1));

x_val = d.buffer[0] | (d.buffer[1]<<8);
spin_unlock(&(lock));

spin_lock_irq(&(lock));	
retval = usb_control_msg(device,usb_rcvctrlpipe(device,0),Y_AXIS,USB_TYPE_VENDOR|USB_DIR_IN,0,0,(unsigned int *)d.buffer,2,5000);
if(retval)
{
printk(KERN_INFO "Control Scan Msg Y returned %d\n",retval);
}

printk(KERN_INFO "Buffer Y %d %d\n",*d.buffer,*(d.buffer+1));

y_val = d.buffer[0] | (d.buffer[1]<<8);
spin_unlock(&(lock));
}

void tilt_scan(struct input_polled_dev *p_dev)
{
struct input_dev *dev = p_dev->input;
read();
input_report_rel( dev , REL_X, x_val);
input_report_rel( dev , REL_Y, y_val);
input_sync(dev);
}


static int tilt_probe(struct usb_interface *interface,const struct usb_device_id *id)
{	
	struct input_dev *dev;
	device = interface_to_usbdev(interface);
	
	
	/* Polled Device Dynamic Memory Allocation */
	p_dev = input_allocate_polled_device();
	if(!p_dev){
	printk("not enough memory for Device\n");
	return -ENOMEM;
	}
	p_dev->poll = tilt_scan;
	p_dev->poll_interval = 5000;  //polling interval in msec

	dev = p_dev->input;	
	dev->name = "Tilt Device";
	input_set_capability(dev, EV_REL, REL_X);
	input_set_capability(dev, EV_REL, REL_Y);
	
	printk(KERN_INFO "Tilt Device (%04X:%04X) Connected\n",id->idVendor,id->idProduct);
	
	/*finally register with the input core */
	input_register_polled_device(p_dev);	
	return 0;
}
	
static void tilt_disconnect(struct usb_interface *interface)
{
	input_unregister_polled_device(p_dev);
	input_free_polled_device(p_dev);
	printk(KERN_ALERT "Tilt USB Disconnected");
}

static struct usb_device_id tilt_table[] = 
{
	{ USB_DEVICE(0x16c0,0x05dc) },
	{} //terminating entry
};


MODULE_DEVICE_TABLE (usb,tilt_table);

static struct usb_driver tilt_usb =
{
	.name = "tilt_usb",
	.id_table = tilt_table,
	.probe = tilt_probe,
	.disconnect = tilt_disconnect,
	
};
	
static int __init tilt_init(void)
{
return usb_register(&tilt_usb); // Register USB Driver  
}



static void __exit tilt_exit(void)
{
 
 usb_deregister(&tilt_usb);// Deregister USB Driver
}

module_init(tilt_init);
module_exit(tilt_exit);	
