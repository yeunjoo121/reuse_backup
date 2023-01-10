#include <stdio.h>
#include <string.h>
#include <wiringSerial.h>
#include <unistd.h>

#define BAUD 115200

char *port0 = "/dev/ttyAMA0";
char *port3 = "/dev/ttyAMA1";


int main(void)
{
	int fd0;
	int fd3;
	int rec;
	if ((fd0 = serialOpen(port0, BAUD)) < 0)
	{
		printf("Device file open error.\n");
		return 1;
	}
	if ((fd3 = serialOpen(port3, BAUD)) < 0)
	{
		printf("Device file open error.\n");
		return 1;
	}
	printf("pi ready\n");
	serialPuts(fd0, "pi ready\n");
	while (1)
	{
		if (serialDataAvail(fd3))
		{
			printf("new data: \n");
			rec = serialGetchar(fd3);
			printf("received from win pc : %c \n", (char)rec);
			//re-transmit...
		}
	}
	return (0);
}
