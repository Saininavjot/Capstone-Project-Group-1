/*
 * main.c
 *
 *  Created on: Nov 12, 2020
 *      Author: ramneet
 */
#include <stdio.h> // standard input output library
#include "gpio_control.h" // gpio to control gpio pins of BBB
#include <unistd.h> // library for usleep() function

#define MOTOR_PIN1 47 // Port 8 pin 15 (Input 1)
#define MOTOR_PIN2 46 // Port 8 pin 16 (Input 2)
#define MOTOR_PIN3 27 // Port 8 pin 17 (Input 3)
#define MOTOR_PIN4 65 // Port 8 pin 18 (Input 4)

int timeout = 2000; // delay to control speed of motor

void rotate_motor_colockwise(int no_of_steps) // function to rotate motor clock wise
{
// sequence AB-BC-CD-DA
    for (int i = 0; i < no_of_steps; i++) // increment steps until 128 steps are reached
    {
        // step 1 sequence AB
        gpio_control_write_value(MOTOR_PIN1, 1); //A=1
        gpio_control_write_value(MOTOR_PIN2, 1); //B=1
        gpio_control_write_value(MOTOR_PIN3, 0); //C=0
        gpio_control_write_value(MOTOR_PIN4, 0); //D=0
        usleep(timeout); // delay to control speed of motor
        // step 2 sequence BC
        gpio_control_write_value(MOTOR_PIN1, 0); // A=0
        gpio_control_write_value(MOTOR_PIN2, 1); // B=1
        gpio_control_write_value(MOTOR_PIN3, 1); // C=1
        gpio_control_write_value(MOTOR_PIN4, 0); // D=0
        usleep(timeout);
        // step 3 sequence CD
        gpio_control_write_value(MOTOR_PIN1, 0); //A=0
        gpio_control_write_value(MOTOR_PIN2, 0); //B=0
        gpio_control_write_value(MOTOR_PIN3, 1); //C=1
        gpio_control_write_value(MOTOR_PIN4, 1); //D=1
        usleep(timeout);
        // step 4 sequence DA
        gpio_control_write_value(MOTOR_PIN1, 1); // A=1
        gpio_control_write_value(MOTOR_PIN2, 0); // B=0
        gpio_control_write_value(MOTOR_PIN3, 0); // C=0
        gpio_control_write_value(MOTOR_PIN4, 1); // D=1
        usleep(timeout);
    }
}
    void rotate_motor_anti_colockwise(int no_of_steps) // function to rotate motor anticlockwise
    {
 // Anticlockwise DA-CD-BC-AB
        for (int i = 0; i < no_of_steps; i++)
        {
            // step 1 sequence DA
            gpio_control_write_value(MOTOR_PIN1, 1); // A=1
            gpio_control_write_value(MOTOR_PIN2, 0); // B=0
            gpio_control_write_value(MOTOR_PIN3, 0); // C=0
            gpio_control_write_value(MOTOR_PIN4, 1); // D=1
            usleep(timeout);
            // step 2 sequence CD
            gpio_control_write_value(MOTOR_PIN1, 0); // A=0
            gpio_control_write_value(MOTOR_PIN2, 0); // B=0
            gpio_control_write_value(MOTOR_PIN3, 1); // C=1
            gpio_control_write_value(MOTOR_PIN4, 1); // D=1
            usleep(timeout); // to control speed
            // step 3 sequence BC
            gpio_control_write_value(MOTOR_PIN1, 0); // A=0
            gpio_control_write_value(MOTOR_PIN2, 1); // B=1
            gpio_control_write_value(MOTOR_PIN3, 1); // C=1
            gpio_control_write_value(MOTOR_PIN4, 0); // D=0
            usleep(timeout);
            // step 4 sequence AB
            gpio_control_write_value(MOTOR_PIN1, 1); // A=1
            gpio_control_write_value(MOTOR_PIN2, 1); // B=1
            gpio_control_write_value(MOTOR_PIN3, 0); // C=0
            gpio_control_write_value(MOTOR_PIN4, 0); // D=0
            usleep(timeout);
        }
}

int main(void)
{
gpio_control_free_pin(MOTOR_PIN1); // unexport GPIO pin 47
gpio_control_free_pin(MOTOR_PIN2); // unexport GPIO pin 46
gpio_control_free_pin(MOTOR_PIN3); // unexport GPIO pin 27
gpio_control_free_pin(MOTOR_PIN4); // unexport GPIO pin 65
    gpio_control_init(MOTOR_PIN1, pin_direction_output); // set pin 1 as output pin
    gpio_control_init(MOTOR_PIN2, pin_direction_output); // set pin 2 as output pin
    gpio_control_init(MOTOR_PIN3, pin_direction_output); // set pin 3 as output pin
    gpio_control_init(MOTOR_PIN4, pin_direction_output); // set pin 4 as output pin
// initially ABCD=0000, wires are all at low level
    gpio_control_write_value(MOTOR_PIN1, 0); // A=0
    gpio_control_write_value(MOTOR_PIN2, 0); //B=0
    gpio_control_write_value(MOTOR_PIN3, 0); //C=0
    gpio_control_write_value(MOTOR_PIN4, 0); //D=0
while(1) // infinite loop
{
    rotate_motor_colockwise(128); // rotate motor clockwise for 128 steps
    usleep(1000000); // delay of 1 second
    rotate_motor_anti_colockwise(128); // rotate motor anticlockwise for 128 steps
    usleep(1000000); // delay of 1 second
}
}
