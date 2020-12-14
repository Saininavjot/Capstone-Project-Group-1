#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <pthread.h>
#include <mqueue.h>

#include "common.h"
#include "push_button.h"
#include "gpio_control.h"
#include "main.h"
#include "buzzer.h"


// queue descriptor
mqd_t buzzer_queue;

// array for queue elements
buzzer_thread_msg message_list[MAX_MSG_SIZE];  // array for queue messages
int buzzer_msg_index_pushed = 0;
int buzzer_msg_index_processed = 0;


// static functions
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

results buzzer_thread_init(void)
{
    results rc = ERROR;
    pthread_t buzzer_thread_t;
    struct mq_attr attr;

    // create message queue
    attr.mq_flags = 0;
    attr.mq_maxmsg = BUZZER_THREAD_QUEUE_SIZE; // queue depth
    attr.mq_msgsize = MAX_MSG_SIZE; // max size of each msg
    attr.mq_curmsgs = 0;
    
    // create a message queue
    // queue name should be initialized with /
    buzzer_queue = mq_open("/buzzer-queue", O_CREAT | O_RDWR, 0666, &attr);
    if (buzzer_queue < 0)
    {
        perror("error creating queue");
        return QUEUE_ERROR;
    }
    else
    {
        printf("[%s][%d] queue create success\r\n", __FILE__, __LINE__);
    }
    
    printf("[%s][%d] creating push button thread \r\n", __FILE__, __LINE__);
    
    // create thread
    rc = pthread_create(&buzzer_thread_t, NULL, buzzer_thread, NULL);
    if (rc != 0)
    {
        perror("error creating buzzer thread");
        return THREAD_ERROR;
    }
    else
    {
        printf("[%s][%d] success creating thread\r\n", __FILE__, __LINE__);
    }

    return SUCCESS;
}

results buzzer_thread_push_to_queue(buzzer_thread_msg buffer)
{
    // default prio of messages is 0
    message_list[buzzer_msg_index_pushed].data = buffer.data;
    message_list[buzzer_msg_index_pushed].id = buffer.id;
    message_list[buzzer_msg_index_pushed].timeout = buffer.timeout;

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
        perror("buzzer: push to queue failed");
        return ERROR;
    }
}
