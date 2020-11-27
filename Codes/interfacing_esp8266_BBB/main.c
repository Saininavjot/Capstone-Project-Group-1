#include <errno.h>   // header file that defines macros for reporting and retrieving error
#include <fcntl.h>   // header defines the requests and arguments for use by the functions to control and open files.
#include <stdlib.h>  //header defines several general purpose functions including dynamic memory management
#include <unistd.h>  //header file that provides access to the POSIX operating system API
#include <string.h>  //header defines functions for manipulating arrays of characters.

int main()
{
char val[20];
int fd= open("/dev/ttyO4", O_RDWR);   //opening the file ttyO4 to read and write value
if (fd== -1)
{
	printf("error open file \n\r");  //error if we are unable to open ttyO4 due to any reason
	return -1;
}
while(1)
{
	if (write(fd, "A", 1) != 1)   //transmitting "A"  to uart ttyO4 
	{
		printf("error writing \r\n");
	}
		
//	read(fd, val, 20);
	printf("writing A\r\n");   //print "writing A"  to terminal 
	sleep(2);
	write(fd, "B", 1);    //transmitting "B"  to uart ttyO4 
	printf("writing b\r\n");   //print "writing B"  to terminal
//	read(fd, val, 20);	
sleep(2);   //delay for 2 seconds
}
}
