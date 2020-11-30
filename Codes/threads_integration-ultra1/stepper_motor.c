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
mqd_t motor_queue;

// array for queue elements
motor_thread_msg message_list[STEPPER_MOTOR_QUEUE_SIZE];  // array for queue messages
int motor_msg_index_pushed = 0;
int motor_msg_index_processed = 0;

gate_state last_kown_state = gate_state_close;

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

static void *motor_thread(void* thread)
{
    char rcvd_msg[MAX_MSG_SIZE];
    unsigned int msg_prio = 0;
    results res = ERROR;
    motor_thread_msg* msg;
 
    printf("[%s][%d] inside motor thread\r\n", __FILE__, __LINE__);
    
    while(1)
    {
        res = mq_receive(motor_queue, rcvd_msg, MAX_MSG_SIZE, &msg_prio);
        if (res == -1)
        {
            perror("mq rec error");
            usleep(MSEC_TO_USEC(250));
            continue;
        }
        printf("[%s][%d] msg rcvd motor\r\n", __FILE__, __LINE__);

        motor_msg_index_processed++;
        if (motor_msg_index_processed >= STEPPER_MOTOR_QUEUE_SIZE)
        {
            motor_msg_index_processed = 0;
        }

        if (rcvd_msg == NULL)
        {
            printf("[%s][%d] rcved msg is null\r\n",__FILE__, __LINE__);
            continue;
        }

        msg = (motor_thread_msg*)rcvd_msg;

        if (msg->state == gate_state_open)
        {
        	if (last_kown_state == gate_state_close)
        	{
        		printf("[%s][%d] open gate \r\n",__FILE__, __LINE__);
        		last_kown_state = gate_state_open;
        		rotate_motor_anti_colockwise(128);
        	}
        }
        else if (msg->state == gate_state_close)
        {
        	if (last_kown_state == gate_state_open)
        	{
        		printf("[%s][%d] close gate \r\n", __FILE__, __LINE__);
        		last_kown_state = gate_state_close;
        		rotate_motor_colockwise(128);
        	}
        }
        else if (msg->state == gate_state_panic)
        {
            printf("[%s][%d] close gate \r\n", __FILE__, __LINE__);
            rotate_motor_colockwise(128);
            sleep(120);
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
    attr.mq_maxmsg = STEPPER_MOTOR_QUEUE_SIZE; // queue depth
    attr.mq_msgsize = MAX_MSG_SIZE; // max size of each msg
    attr.mq_curmsgs = 0;
    
    // create a message queue
    // queue name should be initialized with /
    motor_queue = mq_open("/motor-queue", O_CREAT | O_RDWR, 0666, &attr);
    if (motor_queue < 0)
    {
        perror("error creating motor queue");
        return QUEUE_ERROR;
    }
    else
    {
        printf("[%s][%d] queue create success\r\n", __FILE__, __LINE__);
    }
    
    printf("[%s][%d] creating motor thread \r\n", __FILE__, __LINE__);
    
    // create thread
    rc = pthread_create(&motor_thread_t, NULL, motor_thread, NULL);
    if (rc != 0)
    {
        perror("error creating motor thread");
        return THREAD_ERROR;
    }
    else
    {
        printf("[%s][%d] success creating motor thread\r\n", __FILE__, __LINE__);
    }

    return SUCCESS;
}

results  motor_thread_push_to_queue(motor_thread_msg buffer)
{
    // default prio of messages is 0
	printf("[%s][%d] motor push to Q\r\n", __FILE__, __LINE__);
    message_list[motor_msg_index_pushed].delay = buffer.delay;
    message_list[motor_msg_index_pushed].state = buffer.state;

    if (0 == mq_send(motor_queue, (const char*)&message_list[motor_msg_index_pushed], MAX_MSG_SIZE, 0))
    {
        motor_msg_index_pushed++;
        if (motor_msg_index_pushed >= STEPPER_MOTOR_QUEUE_SIZE)
        {
            motor_msg_index_pushed = 0;
        }
        return SUCCESS;
    }
    else
    {
        perror("motor : push to main queue failed");
        return ERROR;
    }
}
