#ifndef PERIPHERALS_MANAGER_H
#define PERIPHERALS_MANAGER_H

#include "common.h"

typedef struct pm_message
{
    int id;
    int data;
}pm_message;



results peripherals_manager_init(void);

results peripherals_manager_push_to_queue(pm_message buffer);

#endif // PERIPHERALS_MANAGER_H