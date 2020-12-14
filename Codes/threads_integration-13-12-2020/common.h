#ifndef COMMON_H
#define COMMON_H


// enums

typedef enum{
    ERROR,
    SUCCESS,
    MALLOC_FAILED,
    NETWORK_FAILED,
    SYSTEM_BUZY,
    QUEUE_ERROR,
    THREAD_ERROR,
    UART_ERROR,
    
}results;

typedef enum{
    ESP_UART,
    ARDUINO_UART,
    BOTH_UART,
    
}uart_list;

// macros
#define MAX_MSG_SIZE    256

// thread queue size
#define MAIN_THREAD_QUEUE_SIZE          10
#define BUZZER_THREAD_QUEUE_SIZE        10
#define STEPPER_MOTOR_QUEUE_SIZE		10

#define BUZZER_GPIO_PIN     45  //port 8 pin 11 of BBB
#define SWITCH_GPIO_PIN     44  //port 8 pin 12 of BBB

#define MOTOR_PIN1 47           // Port 8 pin 15 (Input 1)
#define MOTOR_PIN2 46           // Port 8 pin 16 (Input 2)
#define MOTOR_PIN3 27           // Port 8 pin 17 (Input 3)
#define MOTOR_PIN4 65           // Port 8 pin 18 (Input 4)

#define SENSOR_1_TRIGGER 7      // port 8 pin 8 trig pin for sen 1 - 49
#define S1_T 49
#define SENSOR_1_ECHO 8         // port 8 pin 8 echo pin for sen 1 - 48
#define S1_E 48
#define SENSOR_2_TRIGGER 9      // port 8 pin 8 trig pin for sen 2 - 47
#define S2_T 47
#define SENSOR_2_ECHO 10        // port 8 pin 8 echo pin for sen 2 - 46
#define S2_E 46
#define SENSOR_1_PORT 8
#define SENSOR_2_PORT 8

#define UART4_PATH       "/dev/ttyO4"
#define UART1_PATH       "/dev/tty01"


#define MSEC_TO_USEC(x)     x*1000
#define MSLEEP(x)       usleep(1000*x)


#endif // COMMON_H
