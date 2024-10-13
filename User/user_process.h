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
    USER_PEOCESS_WORK_TIME_OUT,
    USER_PEOCESS_ZERO_CALIB,
    USER_PROCESS_POWER_OFF,
    USER_PROCESS_DOUBLE_CHANNEL_ERROR,
} USER_PROCESS_FSM_t;



typedef struct
{
  u32 timer_counter;
  u8 tumer_on;
  u8 control_state;
  u8 old_control_state;
} CLEAN_TIMER_t;

#define WORK_CODE      2
#define CALIBRATE_CODE 1
#define STOP_CODE  0

float ComputeSetPoint();
void UPDATE_COOF();
u8 getStateVAV();
u8 getStateDCV();
void SystemCalibraionStop();
void SystemCalibraionStart();
u8 getProcessStateCode();
void USER_AOUT_SET(u8 channel, float data);
float USER_AOUT_GET(u8 channel);
float getDAC1_Out();
void USER_SetSettingChange();
u8 USER_GerErrorState();
u8 USER_FilterState( u8 * state);
void USER_SetControlState(u8 state);
uint16_t USER_GetSetting();
uint16_t USER_GetFact(u8 * state);
USER_PROCESS_FSM_t USER_GetProccesState( void  );
TaskHandle_t * getUserProcessTaskHandle( void  );
void user_process_task(void *pvParameters);

#endif /* USER_USER_PROCESS_H_ */
