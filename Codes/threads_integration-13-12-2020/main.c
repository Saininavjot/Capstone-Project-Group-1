#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <malloc.h>
#include <pthread.h>
#include <mqueue.h>

#include <iobb.h>

#include "main.h"
#include "peripherals_manager.h"
#include "common.h"
#include "gpio_control.h"
#include "push_button.h"
#include "buzzer.h"
#include "distance_sensor.h"
#include "uart_control.h"
#include "stepper_motor.h"

// macros and variables

// queue descriptor
mqd_t main_queue;

// array for queue elements
main_thread_msg message_list[MAX_MSG_SIZE];  // array for queue messages
int msg_index_pushed = 0;
int msg_index_processed = 0;

// Static Functions

/*
 * Freeup all the pins used in the project
 */
static void clean_up_system(void)
{
    // unexport and free up gpios
    gpio_control_free_pin(SWITCH_GPIO_PIN);
    MSLEEP(10);
    gpio_control_free_pin(BUZZER_GPIO_PIN);
    MSLEEP(10);
    gpio_control_free_pin(MOTOR_PIN1);
    MSLEEP(10);
    gpio_control_free_pin(MOTOR_PIN2);
    MSLEEP(10);
    gpio_control_free_pin(MOTOR_PIN3);
    MSLEEP(10);
    gpio_control_free_pin(MOTOR_PIN4);
    MSLEEP(10);
    iolib_free();
    MSLEEP(100);
}

static results initialise_gpios(void)
{
    if (-1 == iolib_init())  // Initialize the IOBB Library
    {
        printf("[%s][%d] error init iobb lib\r\n", __FILE__, __LINE__);
        return ERROR;
    }
    MSLEEP(10);
    if (-1== iolib_setdir(SENSOR_1_PORT, SENSOR_1_TRIGGER, DigitalOut))  // Setting Pin 10 or Port 8 as Output for Sensor trigger
    {
        printf("[%s][%d] error init iobb lib S1T \r\n", __FILE__, __LINE__);
        return ERROR;
    }
    if (-1 == iolib_setdir(SENSOR_1_PORT, SENSOR_1_ECHO, DigitalIn))    // Setting Pin 9 or Port 8 as Inputfor Sensor echo
    {
        printf("[%s][%d] error init iobb lib S1E \r\n", __FILE__, __LINE__);
        return ERROR;
    }
    if (-1 == iolib_setdir(SENSOR_2_PORT, SENSOR_2_TRIGGER, DigitalOut))  // Setting Pin 10 or Port 8 as Output for Sensor trigger
    {
        printf("[%s][%d] error init iobb lib S2T \r\n", __FILE__, __LINE__);
        return ERROR;
    }
    if (-1 == iolib_setdir(SENSOR_2_PORT, SENSOR_2_ECHO, DigitalIn))    // Setting Pin 9 or Port 8 as Inputfor Sensor echo
    {
        printf("[%s][%d] error init iobb lib S2E \r\n", __FILE__, __LINE__);
        return ERROR;
    }
    MSLEEP(100);
    
    for (int  i = 0; i < 3; i++)
    {
        // initialize gpio pin for push button
        if (-1 == gpio_control_init(SWITCH_GPIO_PIN, pin_direction_input))
        {
            printf("[%s][%d] error init switch pin %d try:%u\r\n",__FILE__, __LINE__, SWITCH_GPIO_PIN, i);
            MSLEEP(100);
            if (i == 2)
                return ERROR;
        }
        else
        {
            break;
        }
    }    
    for (int  i = 0; i < 3; i++)
    {
        // initialize gpio pin for push button
        if (-1 == gpio_control_init(BUZZER_GPIO_PIN, pin_direction_output))
        {
            printf("[%s][%d] error init buzzer pin dir %d try:%u\r\n", __FILE__, __LINE__, BUZZER_GPIO_PIN, i);
            MSLEEP(100);
            if (i == 2)
                return ERROR;
        }
        else
        {
            break;
        }
    }
    for (int  i = 0; i < 3; i++)
    {
        if (-1 == gpio_control_init(MOTOR_PIN1, pin_direction_output)) // set pin 1 as output pin
        {
            printf("[%s][%d] error init motor 1 %d, try:%u\r\n",__FILE__, __LINE__, MOTOR_PIN1, i);
            MSLEEP(100);
            if (i == 2)
                return ERROR;
        }
        else
        {
            break;
        }
    }
        
    for (int  i = 0; i < 3; i++)
    {
        if (-1 == gpio_control_init(MOTOR_PIN2, pin_direction_output)) // set pin 1 as output pin
        {
            printf("[%s][%d] error init motor 2 %d, try:%u\r\n",__FILE__, __LINE__, MOTOR_PIN2, i);
            MSLEEP(100);
            if (i == 2)
                return ERROR;
        }
        else
        {
            break;
        }
        
    }
    
    for (int  i = 0; i < 3; i++)
    {
        if (-1 == gpio_control_init(MOTOR_PIN3, pin_direction_output)) // set pin 1 as output pin
        {
            printf("[%s][%d] error init motor 3 %d try:%u\r\n",__FILE__, __LINE__, MOTOR_PIN3, i);
            MSLEEP(100);
            if (i == 2)
                return ERROR;
        }
        else
        {
            break;
        }
    }
    for (int  i = 0; i < 3; i++)
    {
        if (-1 == gpio_control_init(MOTOR_PIN4, pin_direction_output)) // set pin 1 as output pin
        {
            printf("[%s][%d] error init motor 4 %d tr:%u\r\n",__FILE__, __LINE__, MOTOR_PIN4, i);
            MSLEEP(100);
            if (i == 2)
                return ERROR;
        }
        else
        {
            break;
        }
    }

    return SUCCESS;
}

/*
 * panic button is pressed
 *  trigger the buzzer 2 sec
 *  send a message to mqtt
 *  display message on lcd
 */
static results panic_button_pressed(void)
{
    results res = ERROR;
    printf("[%s][%d] panic button pressed\r\n", __FILE__, __LINE__);
    
    // turn the buzzer on for 2 sec
    {
        buzzer_thread_msg buzzer_msg;
        buzzer_msg.timeout = 2000;
        buzzer_msg.data = 1;

        res = buzzer_thread_push_to_queue(buzzer_msg);
    }
    send_msg_uart(ESP_UART, uart_msg_train_panic, 1);

    return res;
}

static results panic_button_released(void)
{
    printf("[%s][%d] panic button pressed\r\n", __FILE__, __LINE__);
    return SUCCESS;
}

static results train_arrived(void)
{
    results res = ERROR;
    motor_thread_msg gate_opr;
    printf("[%s][%d] train arr updating other threads \r\n", __FILE__, __LINE__);
    
    // turn the buzzer on for 500m sec
    {
        buzzer_thread_msg buzzer_msg;
        buzzer_msg.timeout = 500;
        buzzer_msg.data = 1;

        res = buzzer_thread_push_to_queue(buzzer_msg);
    }

    // send a msg to mqtt
    {
        send_msg_uart(ESP_UART, uart_msg_train_arrive, 1);
    }
    
    gate_opr.state = gate_state_open;
    gate_opr.delay = 0;

    motor_thread_push_to_queue(gate_opr);
    return res;
}

static results train_departed(void)
{
    results res = ERROR;
    motor_thread_msg gate_opr;
    printf("[%s][%d] train dep updating other threads\r\n", __FILE__, __LINE__);
    
    // turn the buzzer on for 500m sec
    {
        buzzer_thread_msg buzzer_msg;
        buzzer_msg.timeout = 500;
        buzzer_msg.data = 1;

        res = buzzer_thread_push_to_queue(buzzer_msg);
    }

    // send a msg to mqtt
    {
        send_msg_uart(ESP_UART, uart_msg_train_depart, 1);
    }
    
    gate_opr.state = gate_state_close;
    gate_opr.delay = 0;

    motor_thread_push_to_queue(gate_opr);
    return res;
}

static void *main_thread(void* thread)
{  
    char rcvd_msg[MAX_MSG_SIZE];
    unsigned int msg_prio = 0;
    results res = ERROR;
    main_thread_msg* msg;

    printf("inside main thread\r\n");
    while (1)
    {
        res = mq_receive(main_queue, rcvd_msg, MAX_MSG_SIZE, &msg_prio);
        if (res == -1)
        {
            perror("mq rec error");
            usleep(MSEC_TO_USEC(250));
            continue;
        }

        msg_index_processed++;
        if (msg_index_processed >= MAIN_THREAD_QUEUE_SIZE)
        {
            msg_index_processed = 0;
        }

        if (rcvd_msg == NULL)
        {
            printf("[%s][%d] rcved msg is null\r\n", __FILE__, __LINE__);
            continue;
        }

        msg = (main_thread_msg*)rcvd_msg;

        switch (msg->msg_id)
        {
            case main_thread_painic_button_activity:
                printf("[%s][%d] panic button activity val:%u \r\n", __FILE__, __LINE__, msg->data);
                if (msg->data == 0)
                {
                    panic_button_released();
                }
                else if (msg->data == 1)
                {
                    panic_button_pressed();
                }
                break;
            case main_thread_sensor_activity_train_arrived:
                printf("[%s][%d] sensor has detected train arrive \r\n", __FILE__, __LINE__);
                train_arrived();
                
                break;
            case main_thread_sensor_activity_train_depart:
                printf("[%s][%d] sensor has detected train depart \r\n", __FILE__, __LINE__);
                train_departed();

                break;
            
            case main_thread_mqtt_msg:
                printf("[%s][%d] msg rcvd from mqtt \r\n", __FILE__, __LINE__);
                break;

            default:
                break;
        }
        printf("[%s][%d] msg id: %u, data: %u pushed:%u processed :%u\r\n",__FILE__, __LINE__, msg->data, msg->id, msg_index_pushed, msg_index_processed);
    }
    return 0;
}

static results main_thread_init(void)
{
    results rc = ERROR;
    pthread_t main_thread_t;
    struct mq_attr attr;

    // create message queue
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAIN_THREAD_QUEUE_SIZE; // queue depth
    attr.mq_msgsize = MAX_MSG_SIZE; // max size of each msg
    attr.mq_curmsgs = 0;
    
    // create a message queue
    // queue name should be initialized with /
    main_queue = mq_open("/main-queue", O_CREAT | O_RDWR, 0660, &attr);
    if (main_queue < 0)
    {
        perror("error creating queue");
        return QUEUE_ERROR;
    }
    else
    {
        printf("[%s][%d] queue create success\r\n", __FILE__, __LINE__);
    }

    printf("[%s][%d] creating main thread \r\n", __FILE__, __LINE__);
    
    // create thread
    rc = pthread_create(&main_thread_t, NULL, main_thread, NULL);
    if (rc != 0)
    {
        perror("error creating pm thread");
        return THREAD_ERROR;
    }
    else
    {
        printf("[%s][%d] success creating thread\r\n", __FILE__, __LINE__);
    }

    return SUCCESS;
}

// External functions

results main_thread_push_to_queue(main_thread_msg buffer)
{
    // default prio of messages is 0
    message_list[msg_index_pushed].data = buffer.data;
    message_list[msg_index_pushed].id = buffer.id;
    message_list[msg_index_pushed].msg_id = buffer.msg_id;

    if (0 == mq_send(main_queue, (const char*)&message_list[msg_index_pushed], MAX_MSG_SIZE, 0))
    {
        msg_index_pushed++;
        if (msg_index_pushed >= MAIN_THREAD_QUEUE_SIZE)
        {
            msg_index_pushed = 0;
        }
        return SUCCESS;
    }
    else
    {
        perror("push to main queue failed");
        return ERROR;
    }
}

int main()
{
    results res;
    // clean up pins before reinitializing them for our project
    clean_up_system();

    if (ERROR == initialise_gpios())
    {
        printf("******** Error init some gpio **********");
        return -1;
    }
    // initialize the pm_thread and queue
    res = main_thread_init();
    printf("[%s][%d] main thread init with %u\r\n", __FILE__, __LINE__, res);

    // initialize push button thread
    res = push_button_thread_init();
    printf("[%s][%d] push button thread init with res:%u\r\n",__FILE__, __LINE__, res);

    // initialize buzzer thread
    res = buzzer_thread_init();
    printf("[%s][%d] buzzer thread init with res:%u\r\n", __FILE__, __LINE__, res);

    // initialize ultrasonic sensor thread
    res = distance_sensor_thread_init();
    printf("[%s][%d] distance sensor thread init with res:%u\r\n", __FILE__, __LINE__, res);

    res = motor_thread_init();
    printf("[%s][%d] motor thread init with res:%u\r\n", __FILE__, __LINE__, res);

    // push an event in panic thread every 500ms to read button value
    while (1)
    {
       // msg_test.data = data++;
       // msg_test.id = id++;
       // msg_test.msg_id = main_thread_painic_button_activity;
       // main_thread_push_to_queue(msg_test);
        sleep(1);
    }
}
