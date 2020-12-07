nclude <errno.h> // Header file which is set of system calls in event of error to indicate what went wrong 
#include <fcntl.h>// file control options (read, write, open, close)
#include <stdio.h>// standard input output library
#include <stdlib.h>// define functions performing general functions
#include <sys/stat.h>// define data return from system calls function stat() 
#include <unistd.h> // define sleep () function
#include <string.h> // define functions for manipulating arrays of characters

int main()
{
char val[20]; // buffer to store value
int fd= open("/dev/ttyO4", O_RDWR);// open UART 4 of Beaglebone black ( pin 11(RX), pin 13(TX))
if (fd== -1)// unable to open file
{
        printf("error open file \n\r");// print error while opening file
        return -1; // returning error
}
while(1) // infinite loop
{
        if (write(fd, "A", 1) != 1)//if unable to  write A on serial pin 
        {
                printf("error writing \r\n"); // then print error while writing to tramsmitter pin
        }

        printf("writing A\r\n");// display "writing A" on terminal
        sleep(3);// delay of 3 second
        write(fd, "D", 1);// write D 
        printf("writing D\r\n"); // display D on terminal
        sleep(3);// delay of 3 second
        write(fd, "P", 1);// write P
        printf("writing P \r\n");// display writing P on terminal
        sleep(3);// delay of 3 seconds
}
}
