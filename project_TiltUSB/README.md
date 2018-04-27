# TILT USB Device
*Accelerometer Device implemented over USB*


## Motivation

Accelerometer device can be implemented over low cost usb hardware and data can be reported back to kernel through Input Subsystem. 
These events reported to Input Core can be furthure used to provide user space application.

## Building

    make all

To Build kernel module as well as user space code.

    make user

To Build user space code 

    make kernel

To Build Kernel module

   make clean
To remove all object files
 
## Loading

    modprobe input-polldev
To load module with dependencies.
 
    insmod tilt_usb.ko
To load tilt_usb in kernel

    rmmod tilt_usb
To unload tilt_usb module from kernel
   
## Running

    ./usr_main.o

To start the getting data from accelerometer over usb onto Input Subsystem

## Building and Flashing firmware
 
    make hex
Command to build firmware and generate hex file

    make program
Command to flash fuses and hex file

    make fuses
Command to flash the fuse bits

    make flash
Command to flash the hex file

    make clean
Command to delete object and hex files

## Using Commandline Tool 
    make all
Command to build commandline TiltDevice tool
 
    make clean
Command to delete all object files

    sudo ./TiltDevice x
Command to run commandline TiltDevice tool to get X-axis values 
 
    sudo ./TiltDevice y
Command to run commandline TiltDevice tool to get Y-axis values 
