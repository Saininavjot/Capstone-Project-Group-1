#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <pthread.h>
#include <mqueue.h>

#include "common.h"
#include "push_button.h"
#include "gpio_control.h"
#include "main.h"
#include "stepper_motor.h"


// queue descriptor
mqd_t buzzer_queue;

// array for queue elements
buzzer_thread_msg message_list[MAX_MSG_SIZE];  // array for queue messages
int motor_msg_index_pushed = 0;
int motor_msg_index_processed = 0;


// static functions

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





static void *buzzer_thread(void* thread)
{
    char rcvd_msg[MAX_MSG_SIZE];
    unsigned int msg_prio = 0;
    results res = ERROR;
    buzzer_thread_msg* msg;
 
    printf("[%s][%d] inside buzzer button thread\r\n", __FILE__, __LINE__);
    
    while(1)
    {
        res = mq_receive(buzzer_queue, rcvd_msg, MAX_MSG_SIZE, &msg_prio);
        if (res == -1)
        {
            perror("mq rec error");
            usleep(MSEC_TO_USEC(250));
            continue;
        }

        buzzer_msg_index_processed++;
        if (buzzer_msg_index_processed >= BUZZER_THREAD_QUEUE_SIZE)
        {
            buzzer_msg_index_processed = 0;
        }

        if (rcvd_msg == NULL)
        {
            printf("[%s][%d] rcved msg is null",__FILE__, __LINE__);
            continue;
        }

        msg = (buzzer_thread_msg*)rcvd_msg;

        if (msg->data == 0)
        {
            printf("[%s][%d] turn the buzzer off \r\n",__FILE__, __LINE__);
            gpio_control_write_value(BUZZER_GPIO_PIN, 0);
        }
        else if (msg->data == 1)
        {
            printf("[%s][%d] turn the buzzer on \r\n", __FILE__, __LINE__);
            gpio_control_write_value(BUZZER_GPIO_PIN, 1);
            usleep(MSEC_TO_USEC(msg->timeout));

            // turn the buzzer off after after timeout
            gpio_control_write_value(BUZZER_GPIO_PIN, 0);
        }
    }

    return 0;
}

// External functions

results motor_thread_init(void)
{
    results rc = ERROR;
    pthread_t motor_thread_t;
    struct mq_attr attr;

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

    // create message queue
    attr.mq_flags = 0;
    attr.mq_maxmsg = BUZZER_THREAD_QUEUE_SIZE; // queue depth
    attr.mq_msgsize = MAX_MSG_SIZE; // max size of each msg
    attr.mq_curmsgs = 0;
    
    // create a message queue
    // queue name should be initialized with /
    buzzer_queue = mq_open("/motor-queue", O_CREAT | O_RDWR, 0666, &attr);
    if (buzzer_queue < 0)
    {
        perror("error creating queue");
        return QUEUE_ERROR;
    }
    else
    {
        printf("[%s][%d] queue create success\r\n", __FILE__, __LINE__);
    }
    
    printf("[%s][%d] creating motor thread \r\n", __FILE__, __LINE__);
    
    // create thread
    rc = pthread_create(&motor_thread_t, NULL, buzzer_thread, NULL);
    if (rc != 0)
    {
        perror("error creating motor thread");
        return THREAD_ERROR;
    }
    else
    {
        printf("[%s][%d] success creating thread\r\n", __FILE__, __LINE__);
    }

    return SUCCESS;
}

results buzzer_thread_push_to_queue(motor_thread_msg buffer)
{
    // default prio of messages is 0
    message_list[motor_msg_index_pushed].data = buffer.data;
    message_list[motor_msg_index_pushed].id = buffer.id;
    message_list[motor_msg_index_pushed].timeout = buffer.timeout;

    if (0 == mq_send(buzzer_queue, (const char*)&message_list[buzzer_msg_index_pushed], MAX_MSG_SIZE, 0))
    {
        buzzer_msg_index_pushed++;
        if (buzzer_msg_index_pushed >= BUZZER_THREAD_QUEUE_SIZE)
        {
            buzzer_msg_index_pushed = 0;
        }
        return SUCCESS;
    }
    else
    {
        perror("push to main queue failed");
        return ERROR;
    }
}