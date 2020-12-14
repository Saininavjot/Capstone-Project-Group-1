#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <pthread.h>
#include <mqueue.h>
#include "peripherals_manager.h"
#include "common.h"

#define MAX_MSG_SIZE    256

// queue descriptor
mqd_t pm_queue;

// array for queue elements
pm_message rcvd_msgs[PANIC_TREAD_QUEUE_SIZE];
int last_msg_index = 0;

void *pm_thread(void* threadid)
{
    char rcvd_msg[MAX_MSG_SIZE];
    int msg_prio = 0;
    results res = ERROR;
    pm_message* msg;

    printf("inside pm thread\r\n");
    while (1)
    {
        res = mq_receive(pm_queue, rcvd_msg, MAX_MSG_SIZE, &msg_prio);
        if (res == -1)
        {
            perror("mq rec error");
            usleep(MSEC_TO_USEC(250));
            continue;
        }

        if (rcvd_msg == NULL)
        {
            printf("rcved msg is null");
        }
        msg = (pm_message*)rcvd_msg;
        printf("msg id: %u, data: %u\r\n", msg->data, msg->id);
    }
}


results peripherals_manager_init(void)
{
    results rc = ERROR;
    pthread_t pm_thread_t;
    struct mq_attr attr;
    struct mq_attr mq_attr_ret;

    printf("Creating pm thread\r\n");

    rc = pthread_create(&pm_thread_t, NULL, pm_thread, NULL);
    if (rc != 0)
    {
        printf("error creating pm thread rc:%u", rc);
        return rc;
    }
    else
    {
        printf("success creating thread\r\n");
    }

    printf("create pm queue\r\n");

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    
    // queue name should be initialized with /
    pm_queue = mq_open("/pm_queue", O_CREAT | O_RDWR, 0666, &attr);
    if (pm_queue < 0)
    {
        printf("error creating queue %u\r\n", pm_queue);
        return QUEUE_ERROR;
    }
    else
    {
        printf("queue create success\r\n");
    }

    mq_getattr(pm_queue, &mq_attr_ret);
    printf("pm_queue attr %lu, %lu\r\n", mq_attr_ret.mq_msgsize, mq_attr_ret.mq_maxmsg);

    return SUCCESS;
}

results peripherals_manager_push_to_queue(pm_message buffer)
{
    // default prio of messages is 0
    rcvd_msgs[last_msg_index].data = buffer.data;
    rcvd_msgs[last_msg_index].id = buffer.id;

    if (0 == mq_send(pm_queue, (const char*)&rcvd_msgs[last_msg_index], MAX_MSG_SIZE, 0))
    {
        last_msg_index++;
        if (last_msg_index >= PANIC_TREAD_QUEUE_SIZE)
        {
            last_msg_index = 0;
        }
        //return SUCCESS;
    }
    else
    {
        perror("push to pm queue failed");
        return ERROR;
    }
}