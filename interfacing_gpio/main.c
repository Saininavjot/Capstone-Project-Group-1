/*
 * main.c
 *
 * Created on: Oct 30, 2020
 *      Author: ramneet
 */

#include <stdio.h>
#include <stdlib.h>
#include "gpio_control.h"
#include <unistd.h>


#define BUZZER_GPIO_PIN 45 //port 8 pin 11 of BBB
#define SWITCH_GPIO_PIN 44 //port 8 pin 12 of BBB

/*
 * Freeup all the pins used in the project
 */
static void clean_up_system(void)
{
    // unexport and free up gpios
    gpio_control_free_pin(BUZZER_GPIO_PIN);
    gpio_control_free_pin(SWITCH_GPIO_PIN);
}


int main()
{
    int val = 0;
    int last_known_state = 0;

    // clean up pins before reinitializing them for our project
    clean_up_system();

    if (-1 == gpio_control_init(BUZZER_GPIO_PIN, pin_direction_output))
    {
        printf("error setting pin dir %d \r\n", BUZZER_GPIO_PIN);
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

        // if the switch is pressed turn on the buzzer else turn off
        // if the last updated state is same as new state, then we don't need to update the value
        if ((val == 1) && (last_known_state != 1))
        {
            gpio_control_write_value(BUZZER_GPIO_PIN, 1);
            last_known_state = 1;
        }
        else if ((val == 0) && (last_known_state != 0))
        {
            gpio_control_write_value(BUZZER_GPIO_PIN, 0);
            last_known_state = 0;
        }

        usleep(500*1000);
    }

    //clean up before exiting
    clean_up_system();

    return 0;
}
