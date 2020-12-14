#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#include <iobb.h> // Library to access GPIO of Beaglebone

// macros and variables
long int start, stop, count; // Variables to store values

//clock_t before, end, difference; // To store Time of UltraSonic Sensor

clock_t before_pulse, end_pulse,difference, distance; // To store Time to calculate distance

int cflag = 1; // Flag to stabilize count

#define SENSOR_1_TRIGGER 7 // port 8 pin 8 trig pin for sen 1 - 49
#define S1_T 49
#define SENSOR_1_ECHO 8 // port 8 pin 8 echo pin for sen 1 - 48
#define S1_E 48
#define SENSOR_2_TRIGGER 9 // port 8 pin 8 trig pin for sen 2 - 47
#define S2_T 47
#define SENSOR_2_ECHO 10 // port 8 pin 8 echo pin for sen 2 - 46
#define S2_E 46
#define SENSOR_1_PORT 8
#define SENSOR_2_PORT 8

// static functions
static void *distance_sensor_thread(void* thread)
{
    int val = 0;
    int last_known_state = 0;
    //main_thread_msg msg;
    //msg.msg_id = main_thread_sensor_activity;

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
        if ( distance >= 4 && distance <= 6)
        {
            msg.data = val;
            main_thread_push_to_queue(msg);

            printf("train arriving......\r\n");
            printf("Distance= %ld \r\n", distance);
            printf("\r\n");
        }

        pin_low(8, 7); // setting trigger low for next senses
        iolib_delay_ms(500);  // delay to settle trigger and Echo

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
        if (distance >= 4 && distance <= 6)
        {
            printf("train Departing......\r\n");
            printf("Distance= %ld\r\n", distance);
            printf("\r\n");
        }

        pin_low(8,9); // setting trigger low for next senses
        iolib_delay_ms(500);  // delay to settle trigger and Echo

    }
    return 0;
}


// External functions

int main(void)
{
    int rc = -1;
    pthread_t dist_sensor_thread_t;

    iolib_init();  // Initialize the IOBB Library
    iolib_setdir(SENSOR_1_PORT, SENSOR_1_TRIGGER, DigitalOut);  // Setting Pin 10 or Port 8 as Output for Sensor trigger
    iolib_setdir(SENSOR_1_PORT, SENSOR_1_ECHO, DigitalIn);    // Setting Pin 9 or Port 8 as Inputfor Sensor echo

    iolib_setdir(SENSOR_2_PORT, SENSOR_2_TRIGGER, DigitalOut);  // Setting Pin 10 or Port 8 as Output for Sensor trigger
    iolib_setdir(SENSOR_2_PORT, SENSOR_2_ECHO, DigitalIn);    // Setting Pin 9 or Port 8 as Inputfor Sensor echo

    printf("[%s][%d] creating dist sensor thread \r\n", __FILE__, __LINE__);

    // create thread
    rc = pthread_create(&dist_sensor_thread_t, NULL, distance_sensor_thread, NULL);
    if (rc != 0)
    {
        perror("error creating distance sensor thread");
        return -1;
    }
    else
    {
        printf("[%s][%d] success creating thread\r\n",__FILE__, __LINE__);
    }

    while(1)
    {
        sleep(2);
    }

    return 1;
}
