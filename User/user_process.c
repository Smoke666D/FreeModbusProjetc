/*
 * user_process.c
 *
 *  Created on: Aug 21, 2024
 *      Author: i.dymov
 */
#include "user_process.h"
#include "string.h"


static TaskHandle_t processTaskHandle;
static USER_PROCESS_FSM_t task_fsm;

TaskHandle_t * getUserProcessTaskHandle()
{
    return &processTaskHandle;
}



USER_PROCESS_FSM_t USER_GetProccesState()
{
    return (task_fsm);
}

void user_process_task(void *pvParameters)
{
   task_fsm = USER_PROCCES_IDLE;
   while(1)
   {
       switch (task_fsm)
       {
           case USER_PROCCES_IDLE:
               break;
           case USER_PROCCES_STOP:
               break;
           case USER_RROCCES_WORK:



               break;
           case USER_PROCESS_PAUSE:
               break;
           case USER_PROCESS_ALARM:
               break;
       }
       vTaskDelay(100);
   }
}
