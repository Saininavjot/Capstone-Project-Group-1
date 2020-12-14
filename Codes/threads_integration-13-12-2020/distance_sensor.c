#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#include "common.h"
#include "main.h"

#include <iobb.h> // Library to access GPIO of Beaglebone

// macros and variables
long int start, stop, count; // Variables to store values

//clock_t before, end, difference; // To store Time of UltraSonic Sensor

clock_t before_pulse, end_pulse,difference, distance; // To store Time to calculate distance

int cflag = 1; // Flag to stabilize count

int sensor1_last_state = 0, sensor2_last_state = 0;

// static functions
static void *distance_sensor_thread(void* thread)
{
    //int last_known_state = 0;
    main_thread_msg msg;

    printf("[%s][%d] inside distance sensor thread\r\n",__FILE__, __LINE__);

    while(1)
    {

    /******SENSOR TO SENSE ARIVAL OF TRAIN*******/

        pin_high(8, 7);  // Trigger Ultrasonic Sensor
        usleep(10);
        pin_low(8, 7);

        while(is_low(8, 8))  // Waiting for Echo Pulse to starts
        {
            before_pulse = clock(); //Current Clock Time
        }

        while(is_high(8, 8)) // Waiting and reading time of Echo pulse
        {
            end_pulse = clock();
        }

        difference = end_pulse - before_pulse; // reading absolute time of Echo Pulse by distance
        distance = (difference/2)/30; // calculating approximate value in centimeters

        // If distance is between 80-90cms....approximate distance of train from sensor
        if ( distance >= 3 && distance <= 7)
        {
            if (sensor1_last_state != 1)
            {
                msg.data = distance;
                msg.msg_id = main_thread_sensor_activity_train_arrived;
                main_thread_push_to_queue(msg);
                sensor1_last_state = 1;

                printf("[%s][%d]train arriving......\r\n",  __FILE__, __LINE__);
                printf("[%s][%d]Distance= %ld \r\n",  __FILE__, __LINE__, distance);
                printf("\r\n");
            }
        }
        else
        {
        	if (sensor1_last_state != 0)
        	{
        		printf("[%s][%d]Change last known state for sensor 1\r\n",  __FILE__, __LINE__);
        		sensor1_last_state = 0;
        	}
        }
        

        pin_low(8, 7); // setting trigger low for next senses

    /******SENSOR TO SENSE DEPARTURE OF TRAIN*******/

        pin_high(8,9);  // Trigger Ultrasonic Sensor
        usleep(10);
        pin_low(8, 9);

        while(is_low(8, 10))  // Waiting for Echo Pulse to starts
        {
            before_pulse = clock(); //load current Clock Time
        }
        while(is_high(8, 10)) // waiting and reading time of Echo pulse
        {
            end_pulse = clock();
        }

        difference = end_pulse - before_pulse;// reading absolute time of Echo Pulse by distance
        distance = (difference/2)/30; // calculating approximate value in centimeters

        // If distance is between 80-90cms....approximate distance of train from sensor
        if (distance >= 3 && distance <= 7)
        {
            if (sensor2_last_state != 1)
            {
                msg.data = distance;
                msg.msg_id = main_thread_sensor_activity_train_depart;
                main_thread_push_to_queue(msg);
                sensor2_last_state = 1;
                printf("[%s][%d]train Departing......\r\n",  __FILE__, __LINE__);
                printf("[%s][%d]Distance= %ld\r\n",  __FILE__, __LINE__, distance);
                printf("\r\n");
            }
        }
        else
        {
        	if (sensor2_last_state != 0)
        	{
        		printf("[%s][%d]Change last known state for sensor 2\r\n",  __FILE__, __LINE__);
        		sensor2_last_state = 0;
        	}
        }
        
        pin_low(8,9); // setting trigger low for next senses
        iolib_delay_ms(500);  // delay to settle trigger and Echo

    }
    return 0;
}


// External functions

results distance_sensor_thread_init(void)
{
    results rc = ERROR;
    pthread_t dist_sensor_thread_t;

    printf("[%s][%d] creating dist sensor thread \r\n", __FILE__, __LINE__);

    // create thread
    rc = pthread_create(&dist_sensor_thread_t, NULL, distance_sensor_thread, NULL);
    if (rc != 0)
    {
        perror("error creating distance sensor thread");
        return THREAD_ERROR;
    }
    else
    {
        printf("[%s][%d] success creating thread\r\n",__FILE__, __LINE__);
    }

    return 1;
}
