/*
 * gpio_control.h
 *
 *  Created on: Oct 30, 2020
 *      Author: ramneet
 */

#ifndef GPIO_CONTROL_H
#define GPIO_CONTROL_H

typedef enum pin_state
{
    pin_direction_input,
    pin_direction_output,

}pin_direction;

typedef enum edge_trigger
{
    edge_trigger_rising,
    edge_trigger_falling,

}edge_trigger;

/*
 * this function exports the gpio file and set the direction of pin
 * use this before using any gpio operatios
 * in_param pin:    pin number
 * in param state:  pin direction i.e. input or output
 *
 * @return:        1 :success and -1: failure
 */
int gpio_control_init(int pin, pin_direction state);

/*
 * write value to a gpio, need to initialize the pin before calling this function
 * in_param pin     : pin number
 * in_param value   : value to be written
 *
 * @return          : 1 success and -1 failure
 */
int gpio_control_write_value(int pin, int value);

/*
 * read value to a gpio, need to initialize the pin before calling this function
 * in_param pin     : pin number
 * in_param value   : read value fro the GPIO
 *
 * @return          : 1 success and -1 failure
 */
int gpio_control_read_value(int pin, int* value);

/*
 * unexport the pin and restore it to be used in differet programs
 * in_param pin     : pin number
 *
 * @return          : 1 success and -1 failure
 */
int gpio_control_free_pin(int pin);

/*
 * set the edge of input pin, on which interrupt will be triggered
 *
 * in_param         : pin number
 * in_param         : edge on which it needs to be trigered
 *
 * @return          : 1 success and -1 failure
 */
int gpio_set_edge(unsigned int gpio, edge_trigger edge);

/*
 * open and return file descriptor for a particular gpio pin
 * in_param         : pin number
 *
 * @return          : file descriptor for this file, -1 if the operation fails
 */
int gpio_fd_open(unsigned int gpio);

#endif // GPIO_CONTROL_H
