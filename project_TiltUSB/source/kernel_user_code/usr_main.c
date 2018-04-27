#include <stdio.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>


int main(int argc , char ** argv)
{
int fd = -1;
int i,version;
size_t read_bytes;
struct input_event event_buf[1];
fd = open("/dev/input/event5", O_RDONLY);
if(fd < 0) {
printf("\nUnable to read from the device\n");
return(1);
}

printf("Reading Tilt USB Device\n");
while(1) {
	read_bytes = read(fd, event_buf, sizeof(struct input_event));
	switch(event_buf[0].type) {
	case EV_REL:
		if(event_buf[0].code==0)
		printf("REL X Value =: %d  ",event_buf[0].value);
		else if(event_buf[0].code==1)
		printf("REL Y Value =: %d\n",event_buf[0].value);
		break;
	}
}
close (fd);
}
