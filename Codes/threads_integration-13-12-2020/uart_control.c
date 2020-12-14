#include <errno.h>   // header file that defines macros for reporting and retrieving error
#include <fcntl.h>   // header defines the requests and arguments for use by the functions to control and open files.
#include <stdlib.h>  //header defines several general purpose functions including dynamic memory management
#include <unistd.h>  //header file that provides access to the POSIX operating system API
#include <string.h>  //header defines functions for manipulating arrays of characters.
#include <stdio.h>
#include "main.h"
#include "common.h"
#include "uart_control.h"

int send_msg_uart(uart_list uart, uart_msg msg, int data)
{
    int fd = 0;
    if (uart == ESP_UART || uart == BOTH_UART)
    {
        fd= open(UART4_PATH, O_RDWR);   //opening the file ttyO4 to read and write value
        if (fd== -1)
        {
            perror("error open file");  //error if we are unable to open ttyO4 due to any reason
            return UART_ERROR;
        }

        if (msg == uart_msg_train_arrive)
        {
            printf("[%s][%d] wrting train arrive to esp\r\n", __FILE__, __LINE__);
            write(fd, "A", 1);    //transmitting "B"  to uart ttyO4 
        }
        else if (msg == uart_msg_train_depart)
        {
            printf("[%s][%d] wrting train arrive to esp\r\n", __FILE__, __LINE__);
            write(fd, "D", 1);    //transmitting "D"  to uart ttyO4
        }
        else if(msg == uart_msg_train_panic)
        {
            printf("[%s][%d] wrting train arrive to esp\r\n", __FILE__, __LINE__);
            write(fd, "p", 1);    //transmitting "D"  to uart ttyO4
        }
        close(fd);
    }
    if (uart == ARDUINO_UART || uart == BOTH_UART)
    {
        // add code for lcd
        fd= open(UART1_PATH, O_RDWR);   //opening the file ttyO4 to read and write value
        if (fd== -1)
        {
            perror("error open file");  //error if we are unable to open ttyO4 due to any reason
            return UART_ERROR;
        }

        if (msg == uart_msg_train_arrive)
        {
            printf("[%s][%d] wrting train arrive to esp\r\n", __FILE__, __LINE__);
            write(fd, "A", 1);    //transmitting "B"  to uart ttyO4 
        }
        else if (msg == uart_msg_train_depart)
        {
            printf("[%s][%d] wrting train arrive to esp\r\n", __FILE__, __LINE__);
            write(fd, "D", 1);    //transmitting "D"  to uart ttyO4
        }
        else if(msg == uart_msg_train_panic)
        {
            printf("[%s][%d] wrting train arrive to esp\r\n", __FILE__, __LINE__);
            write(fd, "p", 1);    //transmitting "D"  to uart ttyO4
        }
        close(fd);
    }

    return SUCCESS;
}
