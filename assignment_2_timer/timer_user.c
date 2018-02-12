#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>

#define WRITE_COMMAND 	_IOW(0,0,unsigned long)
#define READ_STATUS 	_IOWR(0,1,unsigned long)
#define SLEEP		_IOW(0,2,unsigned long)

int main(int argc,char *argv[])
{
	int fd,choice;
	int ret_val;
	unsigned long delay;
    	char *buffer;
    	buffer=(char*)malloc(sizeof(char)*2);
	fd=open("/dev/timer0",0);
	if(fd<0)
    	{
        	printf("Device Open Failed\n");
        	exit(-1);
    	}
	printf("Enter 1 for read,2 for sleep");
	scanf("%d",&choice);
	switch(choice)
	{
		case 1:
			ret_val=read(fd,buffer,2);
       			if(ret_val>=0)
        		printf("COUNTER Value: %d\n",(((unsigned char)buffer[1])*256)+((unsigned char)buffer[0]));
        		else
        		printf("Device Read Failed\n");
			break;
		case 2:
			printf("Enter value of delay in milli second");
			scanf("%lu",&delay);
			system("date");
			ret_val=ioctl(fd,SLEEP,delay);
			system("date");
        		if(ret_val>=0)
			printf("Device IOCTL Successful\n");
			else
			printf("Device IOCTL Failed\n");	
			break;
	}
}
