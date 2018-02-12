This Device Driver code will execute delay in milliseconds through 8254 PIT

 
Follow these steps to build the program after cloning:

#1 To build and insert kernel module 
$ make all
$ sudo insmod timer_kernel.ko

This will load the device driver into kernel space.


#2 To build and run user space program 
$ gcc timer_user.c -o timer_user
$ sudo ./timer_user

The program will ask for 2 choices:
a. Read current timer counter value.
b. Insert delay (in milliseconds).
	- Selection of this choice will ask for delay in milliseconds.
	- Upon entering value, the delay will be executed.
