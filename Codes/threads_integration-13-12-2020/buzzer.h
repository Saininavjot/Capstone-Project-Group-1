#ifndef BUZZER_H
#define BUZZER_H

/*
 * Message structure for pushing it in push button thread queue 
 */
typedef struct buzzer_thread_msg
{
    int id;
    int data;
    int timeout;
}buzzer_thread_msg;

results buzzer_thread_init(void);

results buzzer_thread_push_to_queue(buzzer_thread_msg buffer);

#endif // BUZZER_H
