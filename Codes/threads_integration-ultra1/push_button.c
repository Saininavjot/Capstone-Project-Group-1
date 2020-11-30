#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <pthread.h>

#include "common.h"
#include "push_button.h"
#include "gpio_control.h"
#include "main.h"

// macros and variables

// static functions
static void *push_button_thread(void* thread)
{  
    int val = 0;
    int last_known_state = 0;
    main_thread_msg msg;
    msg.msg_id = main_thread_painic_button_activity;  // initialize message id with button activity
    
    printf("[%s][%d] inside push button thread\r\n",__FILE__, __LINE__);

    // read value of the switch after every 500ms and check if it is pressed
    while (1)
    {
        gpio_control_read_value(SWITCH_GPIO_PIN, &val);

        // if the switch is pressed turn on the buzzer else turn off
        // if the last updated state is same as new state, then we dont eed to update the value
        if ((val == 1) && (last_known_state != 1))
        {
            msg.data = val;
            main_thread_push_to_queue(msg);
            last_known_state = val;
        }
        else if ((val == 0) && (last_known_state != 0))
        {
            msg.data = val;
            main_thread_push_to_queue(msg);
            last_known_state = val;
        }
        
        usleep(MSEC_TO_USEC(250));
    }

    return 0;
}


// External functions

results push_button_thread_init(void)
{
    results rc = ERROR;
    pthread_t push_button_thread_t;
    

    // initialize gpio pin for push button
    if (-1 == gpio_control_init(SWITCH_GPIO_PIN, pin_direction_input))
    {
        printf("[%s][%d] error setting pin dir %d\r\n",__FILE__, __LINE__, SWITCH_GPIO_PIN);
        return ERROR;
    }

    printf("[%s][%d] creating push button thread \r\n", __FILE__, __LINE__);
    
    // create thread
    rc = pthread_create(&push_button_thread_t, NULL, push_button_thread, NULL);
    if (rc != 0)
    {
        perror("error creating push button thread");
        return THREAD_ERROR;
    }
    else
    {
        printf("[%s][%d] success creating thread\r\n",__FILE__, __LINE__);
    }

    return SUCCESS;
}