/*
 * ultrasonic.c
 *
 *  Created on: Oct 28, 2020
 *      Author: Ramneet
 */
#include <iobb.h>  // Library to access GPIO of Beaglebone
#include <time.h>  //Time Library
#include <stdio.h> // Standard IO library
#include <unistd.h>  //header file that provides access to the POSIX operating system API
#include <stdlib.h>  //provide functions involving memory allocation, process control and others.

clock_t before_pulse, end_pulse,difference, distance; // To store Time to calculate distance
int main(void)
{
  iolib_init();  // Initialize the IOBB Library

  //SENSOR 1 IO Directions
  iolib_setdir(8, 7, DigitalOut);  // Setting direction of Pin 7 of Port 8 as Output for Trigger
  iolib_setdir(8, 8, DigitalIn);   // Setting direction of Pin 8 of Port 8 as Input for Echo

  //SENSOR 2 IO Directions
  iolib_setdir(8, 9, DigitalOut);  // Setting direction of Pin 9 of Port 8 as Output for Trigger
  iolib_setdir(8, 10, DigitalIn);  // Setting direction of Pin 10 of Port 8 as Input for Echo


  printf("sensor started \n");
  iolib_delay_ms(500);

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


   // If distance is in the range 4-6 cms....approximate distance of train from sensor
    if ( distance>= 4 && distance =< 6)
    	{
    		printf("train arriving......\r\n");
    		printf("Distance= %d \r\n", distance);
    		printf("\r\n", distance);
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

    // If distance is in the range 4-6 cms....approximate distance of train from sensor
    if (distance >= 4 && distance =< 6)
    	{
    		printf("train Departing......\r\n");
    		printf("Distance= %d\r\n", distance);
    		printf("\r\n", distance);
        }

    pin_low(8,9); // setting trigger low for next senses
    iolib_delay_ms(500);  // delay to settle trigger and Echo

  }

  iolib_free();
  return(0);
}

