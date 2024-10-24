/*
 * user_process.h
 *
 *  Created on: Aug 21, 2024
 *      Author: i.dymov
 */

#ifndef USER_USER_PROCESS_H_
#define USER_USER_PROCESS_H_

#include "main.h"
#include "system_types.h"







typedef enum
{
  BP_CONFIG = 0,
  ONE_CH    = 1,
  TWO_CH    = 2,

} CHANNEL_COUNT_t;





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
void USER_SetControlState(u8 state);
uint16_t USER_GetSetting();
uint16_t USER_GetFact(u8 * state);
USER_PROCESS_FSM_t USER_GetProccesState( void  );
TaskHandle_t * getUserProcessTaskHandle( void  );
void user_process_task(void *pvParameters);

#endif /* USER_USER_PROCESS_H_ */
