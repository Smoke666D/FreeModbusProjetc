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
    USER_PROCEES_RUN,
    USER_PEOCESS_WORK_TIME_OUT,
} USER_PROCESS_FSM_t;

u8 USER_GerErrorState();
u8 USER_FilterState();
void USER_SetControlState(u8 state);
uint16_t USER_GetSetting();
uint16_t USER_GetFact();
USER_PROCESS_FSM_t USER_GetProccesState( void  );
TaskHandle_t * getUserProcessTaskHandle( void  );
void user_process_task(void *pvParameters);

#endif /* USER_USER_PROCESS_H_ */
