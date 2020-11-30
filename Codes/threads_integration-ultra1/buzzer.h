#ifndef BUZZER_H
#define BUZZER_H

typedef enum
{
    gate_state_open,
    gate_state_close,
    gate_state_panic,
}gate_state;

/*
 * Message structure for pushing it in push button thread queue 
 */
typedef struct motor_thread_msg
{
    int id;
    int data;
    int timeout;
}buzzer_thread_msg;

results buzzer_thread_init(void);

results buzzer_thread_push_to_queue(buzzer_thread_msg buffer);

#endif // BUZZER_H