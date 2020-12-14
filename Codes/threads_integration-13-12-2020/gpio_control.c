/*
 * gpio_control.c
 *
 *  Created on: Oct 30, 2020
 *      Author: ramneet
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "gpio_control.h"

int gpio_control_init(int pin, pin_direction state)
{
    char buffer[50];//
    int buf_len;
    int fd;

    if (pin < 10)
    {
        buf_len = 1;
    }
    else if (pin < 100)
    {
        buf_len = 2;
    }
    else
    {
        printf("[%s][%d] pin no:%d put of scope", __FILE__, __LINE__,pin);
        return -1;
    }

    snprintf(buffer, 50, "%d", pin);

    // Export the desired pin by writing to /sys/class/gpio/export

    fd = open("/sys/class/gpio/export", O_WRONLY);//file descriptor
    if (fd == -1) {
        perror("Unable to open /sys/class/gpio/export");
        return -1;
    }
	else
	{
		printf("[%s][%d] export success\r\n",__FILE__, __LINE__);
	}

    if (write(fd, buffer, buf_len) != buf_len)
    {
        perror("Error writing to /sys/class/gpio/export");
        close(fd);
        return -1;
    }
	else
	{
		printf("[%s][%d] success exporting pin:%d\r\n",__FILE__, __LINE__, pin);
	}

    close(fd);

    // Set the pin to be an output by writing "out" to /sys/class/gpio/gpio24/direction

    snprintf(buffer, 50, "/sys/class/gpio/gpio%d/direction", pin);

    fd = open(buffer, O_WRONLY);
    if (fd == -1) {
        printf("[%s][%d] Unable to open %s\r\n",__FILE__, __LINE__, buffer);
        goto error;
    }

    if (state == pin_direction_input)
    {
        if (write(fd, "in", 2) != 2)  //2 is the length of string passed "in" here
        {
            printf("[%s][%d] Error writing to /sys/class/gpio/gpio%d/directionr\r\n",__FILE__, __LINE__, pin);
            close(fd);
            goto error;
        }
        {
            printf("[%s][%d] success open dir file for pin:%d\r\n",__FILE__, __LINE__, pin);
        }
    }
    else if (state == pin_direction_output)
    {
        if (write(fd, "out", 3) != 3) {
            printf("[%s][%d] Error writing to /sys/class/gpio/gpio%d/direction\r\n",__FILE__, __LINE__, pin);
            close(fd);
            goto error;
        }
        {
            printf("[%s][%d] success open dir file for pin:%d\r\n",__FILE__, __LINE__, pin);
        }
    }

    close(fd);
    return 1;

error:

    // Unexport the pin by writing to /sys/class/gpio/unexport

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1)
    {
        perror("Unable to open /sys/class/gpio/unexport");
        return -1;
    }

    snprintf(buffer, 50, "%d", pin);
    printf("[%s][%d] %s\r\n", __FILE__, __LINE__, buffer);

    if (write(fd, buffer, buf_len) != buf_len)
    {
        perror("Error writing to /sys/class/gpio/unexport");
    }
    close(fd);

    return -1;
}

int gpio_control_write_value(int pin, int value)
{
    int fd;
    char buffer[50];

    snprintf(buffer, 50, "/sys/class/gpio/gpio%d/value", pin);

    fd = open(buffer, O_WRONLY);
    if (fd == -1) {
        printf("[%s][%d] Unable to open %s\r\n",__FILE__, __LINE__, buffer);
        return -1;
    }

    if (value == 1)
    {
        if (write(fd, "1", 1) != 1) {
            perror("Error writing 1");
            close(fd);
            return -1;
        }
    }
    else if (value == 0)
    {
        if (write(fd, "0", 1) != 1) {
            perror("Error writing 0");
            close(fd);
            return -1;
        }
    }

    close(fd);
    return 1;
}

int gpio_control_read_value(int pin, int* value)
{
    int fd;
    char buffer[50];
    char val[2];

    snprintf(buffer, 50, "/sys/class/gpio/gpio%d/value", pin);

    fd = open(buffer, O_RDWR);
    if (fd == -1) {
        printf("[%s][%d] Unable to open %s\r\n",__FILE__, __LINE__, buffer);
    }

    //read fun read value in char but the function return is in int
    if (read(fd, val, 2) == -1)  
    {
        perror("Error reading val");
        close(fd);
        return -1;
    }

    if (strstr(val, "1") == NULL)
    {
		*value = 1;
	} else {
		*value = 0;
	}

    //printf("[%s][%d] val is %d\r\n",__FILE__, __LINE__, *value);
    
    close(fd);
    return 1;
}

int gpio_control_free_pin(int pin)
{
    int fd;
    char buffer[50];
    int buff_len;

    if (pin < 10)
    {
        buff_len = 1;
    }
    else if (pin < 100)
    {
        buff_len = 2;
    }
    else
    {
        printf("[%s][%d] pin no:%d put of scope",__FILE__, __LINE__, pin);
        return -1;
    }

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1)
    {
        perror("Unable to open /sys/class/gpio/unexport");
        return -1;
    }

    snprintf(buffer, 50, "%d", pin);
    printf("[%s][%d] free pin %s\r\n", __FILE__, __LINE__, buffer);

    if (write(fd, buffer, buff_len) != buff_len)
    {
        perror("Error writing to /sys/class/gpio/unexport");
        close(fd);
        return -1;
    }

    close(fd);
    return 1;
}

int gpio_set_edge(unsigned int gpio, edge_trigger edge)
{
	int fd;
	char buf[50];

	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/edge", gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-edge");
		return fd;
	}
    else
    {
        printf("[%s][%d] success setting edge:%u \r\n",__FILE__, __LINE__, gpio);
    }

    if (edge == edge_trigger_rising)
    {
        write(fd, "rising", strlen("rising") + 1);
    }
    else if (edge == edge_trigger_falling)
    {
        write(fd, "falling", strlen("falling") + 1);
    }
	
	close(fd);
	return 1;
}

int gpio_fd_open(unsigned int gpio)
{
	int fd;
	char buf[50];

	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", gpio);

	fd = open(buf, O_RDONLY | O_NONBLOCK );
	if (fd < 0) {
		perror("gpio/fd_open");
	}

	return fd;
}

