/*
 * main.c
 *
 *  Created on: Nov 12, 2020
 *      Author: ramneet
 */
#include <stdio.h>
#include "gpio_control.h"
#include <unistd.h>

#define MOTOR_PIN1 47
#define MOTOR_PIN2 46
#define MOTOR_PIN3 27
#define MOTOR_PIN4 65

int timeout = 2000000;

void rotate_motor_colockwise(int no_of_steps)
{

    for (int i = 0; i < no_of_steps; i++)
    {
        // step 1
        gpio_control_write_value(MOTOR_PIN1, 0);
        gpio_control_write_value(MOTOR_PIN2, 0);
        gpio_control_write_value(MOTOR_PIN3, 0);
        gpio_control_write_value(MOTOR_PIN4, 1);
        usleep(timeout);
        // step 2
        gpio_control_write_value(MOTOR_PIN1, 0);
        gpio_control_write_value(MOTOR_PIN2, 0);
        gpio_control_write_value(MOTOR_PIN3, 1);
        gpio_control_write_value(MOTOR_PIN4, 0);
        usleep(timeout);
        // step 3
        gpio_control_write_value(MOTOR_PIN1, 0);
        gpio_control_write_value(MOTOR_PIN2, 1);
        gpio_control_write_value(MOTOR_PIN3, 0);
        gpio_control_write_value(MOTOR_PIN4, 0);
        usleep(timeout);
        // step 4
        gpio_control_write_value(MOTOR_PIN1, 1);
        gpio_control_write_value(MOTOR_PIN2, 0);
        gpio_control_write_value(MOTOR_PIN3, 0);
        gpio_control_write_value(MOTOR_PIN4, 0);
        usleep(timeout);
    }

//    gpio_control_write_value(MOTOR_PIN1, 1);
  //  gpio_control_write_value(MOTOR_PIN2, 1);
}

int main(void)
{
	gpio_control_free_pin(MOTOR_PIN1);
	gpio_control_free_pin(MOTOR_PIN2);
	gpio_control_free_pin(MOTOR_PIN3);
	gpio_control_free_pin(MOTOR_PIN4);
    gpio_control_init(MOTOR_PIN1, pin_direction_output);
    gpio_control_init(MOTOR_PIN2, pin_direction_output);
    gpio_control_init(MOTOR_PIN3, pin_direction_output);
    gpio_control_init(MOTOR_PIN4, pin_direction_output);

    gpio_control_write_value(MOTOR_PIN1, 0);
    gpio_control_write_value(MOTOR_PIN2, 0);
    gpio_control_write_value(MOTOR_PIN3, 0);
    gpio_control_write_value(MOTOR_PIN4, 0);

    rotate_motor_colockwise(200);
}
