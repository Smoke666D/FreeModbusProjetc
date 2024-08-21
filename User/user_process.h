/*
 * user_process.h
 *
 *  Created on: Aug 21, 2024
 *      Author: i.dymov
 */

#ifndef USER_USER_PROCESS_H_
#define USER_USER_PROCESS_H_

#include "system_init.h"


typedef enum
{
    USER_PROCCES_IDLE,
    USER_PROCCES_STOP,
    USER_RROCCES_WORK,
    USER_PROCESS_PAUSE,
    USER_PROCESS_ALARM,
} USER_PROCESS_FSM_t;


void USER_GetProccesState(  char * str );
TaskHandle_t * getUserProcessTaskHandle();
void user_process_task(void *pvParameters);

#endif /* USER_USER_PROCESS_H_ */
