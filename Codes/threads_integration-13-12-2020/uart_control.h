#ifndef UART_CONTROL_H
#define UART_CONTROL_H

typedef enum
{
    uart_msg_train_arrive,
    uart_msg_train_depart,
    uart_msg_train_panic,

}uart_msg;

int send_msg_uart(uart_list uart, uart_msg msg, int data);

#endif