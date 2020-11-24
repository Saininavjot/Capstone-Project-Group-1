#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main()
{
char val[20];
int fd= open("/dev/ttyO4", O_RDWR);
if (fd== -1)
{
	printf("error open file \n\r");
	return -1;
}
while(1)
{
	if (write(fd, "A", 1) != 1)
	{
		printf("error writing \r\n");
	}
		
//	read(fd, val, 20);
	printf("writing A\r\n");
	sleep(2);
	write(fd, "B", 1);
	printf("writing b\r\n");
//	read(fd, val, 20);	
sleep(2);
}
}
