#include <stdio.h>
#include <string.h>
#include <wiringSerial.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	char *port = "/dev/ttyAMA0";
	//serial open
	int fd;
	char s;

	if ((fd = serialOpen(port, 115200)) < 0)
	{
		printf("Device file open error.\n");
		return 1;
	}

	printf("start\n");

	while (1)
	{
		u_int8_t x;
		printf("%s\n", "ready");
		s = read(fd, &x, 1);
		printf("receivd : %d\n",(int)(s)); 
		printf("%s\n", "skip");
	}
	return (0);
}
