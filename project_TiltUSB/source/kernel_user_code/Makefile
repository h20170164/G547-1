obj-m := tilt_usb.o
clean:
	make -C /lib/modules/4.4.0-119-generic/build M=$(PWD) clean
user:
	gcc usr_main.c -o usr_main.o
kernel:
	make -C /lib/modules/4.4.0-119-generic/build M=$(PWD) modules

all:
	make -C /lib/modules/4.4.0-119-generic/build M=$(PWD) modules
	gcc usr_main.c -o usr_main.o

