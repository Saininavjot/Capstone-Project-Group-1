#ifndef MAIN_H
#define MAIN_H

#include "common.h"

/*
 * enum defining the types of messages pushed in the main thread
 */
typedef enum
{
    main_thread_painic_button_activity,
    main_thread_sensor_activity_train_arrived,
    main_thread_sensor_activity_train_depart,
    main_thread_mqtt_msg,

    // for generic messages
    main_thread_general,
}main_thread_msg_identifier;

/*
 * Message structure for pushing it in main thread queue 
 */
typedef struct main_thread_msg
{
    int id;
    int data;
    main_thread_msg_identifier msg_id;
}main_thread_msg;

/*
 * Push a message to main thread
 * in_param         : msg to be pushed in queue
 *    
 * @return          : Success/Failure  
 */
results main_thread_push_to_queue(main_thread_msg buffer);


#endif // MAIN_H