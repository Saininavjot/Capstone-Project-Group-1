/*
 * main.c
 *
 * Created on: Oct 30, 2020
 *      Author: ramneet
 */

#include <stdio.h> // standard library for input and output
#include "gpio_control.h" // library to control GPIO pins of BBB
#include <unistd.h> //library that include usleep function


#define BUZZER_GPIO_PIN 45 //port 8 pin 11 of BBB
#define SWITCH_GPIO_PIN 44 //port 8 pin 12 of BBB

/*
 * Freeup all the pins used in the project
 */
static void clean_up_system(void) // function to free up pins to be used in code
{
    // unexport and free up gpios
    gpio_control_free_pin(BUZZER_GPIO_PIN); // calling free pin function from gpio_control.c file
    gpio_control_free_pin(SWITCH_GPIO_PIN);
}


int main()
{
    int val = 0; // val variable to store 1 or 0 value of the pin
    int last_known_state = 0; // it store last state of the pin


    // clean up pins before reinitializing them for our project
    clean_up_system(); // clear pin 45 and 44 of BBB

    if (-1 == gpio_control_init(BUZZER_GPIO_PIN, pin_direction_output)) // Initialize Buzzer GPIO pin and set direction
    {
        printf("error setting pin dir %d \r\n", BUZZER_GPIO_PIN); // print in case error occurs
        return 0;
    }

    if (-1 == gpio_control_init(SWITCH_GPIO_PIN, pin_direction_input))//GPIO_44
    {
        printf("error setting pin dir %d\r\n", SWITCH_GPIO_PIN);
        return 0;
    }

    // make sure buzzer is off before starting in loop
    gpio_control_write_value(BUZZER_GPIO_PIN, 0);
    last_known_state = 0;

    // read value of the switch after every 500ms and check if it is pressed
    // if pressed then turn on the buzzer
    while(1)
    {
        gpio_control_read_value(SWITCH_GPIO_PIN, &val);

        // if the switch is pressed turn on, the buzzer else turn off
        // if the last updated state is same as new state, then we don't need to update the value
        if ((val == 1) && (last_known_state != 1)) // -if push button is 1, and last state is 0
        {
            gpio_control_write_value(BUZZER_GPIO_PIN, 1); // then, Buzzer is turned on
            last_known_state = 1; // set variable last state to 1
        }
        else if ((val == 0) && (last_known_state != 0)) // else if push button is off
        {
            gpio_control_write_value(BUZZER_GPIO_PIN, 0); // buzzer is off
            last_known_state = 0;
        }

        usleep(500*1000); // wait for 500 ms sec, then again check the status of push button
    }

    //clean up before exiting
    clean_up_system();

    return 0;
}
