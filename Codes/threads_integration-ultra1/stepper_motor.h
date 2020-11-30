#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

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
    gate_state state;
    int delay;
}motor_thread_msg;

results motor_thread_init(void);
results motor_thread_push_to_queue(motor_thread_msg buffer);

#endif // STEPPER_MOTOR_H
