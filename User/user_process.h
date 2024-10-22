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
  L_UNIT = 0,
  V_UNIT = 1,
  P_UNIT = 2,
} MES_UNIT_t;


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
 uint16_t Voltage;
 uint8_t high_voltage_timeout;
 uint8_t low_voltage_timeout;
 uint8_t power_on_flag;
 uint8_t power_off_flag;
 uint8_t power_off_timeout;
} AC_VOLTAGE_CONTROL_t;



typedef struct
{
  u8 Setting_old;
  u8 HEPA_CONTROL_FLAG;
  u32 Setting_Warning_Timeout;
  u32 Filter_Warning_Timeout;
  u32 PreFilter_Warning_Timeout;
  u32 pid_counter;
  u32 start_timeout;

} FMCH_Device_t;

typedef enum
{
  BP_CONFIG = 0,
  ONE_CH    = 1,
  TWO_CH    = 2,

} CHANNEL_COUNT_t;

typedef enum
{
  INP_DISCRETE_INPUT = 0,
  INP_PASSIVE_T_SENSOR = 1,
  INP_ROOM_CONTROLLER  = 2,
  INP_ANALOG_SENSOR  = 3,
} INPUT_SENSOR_TYPE_t;

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
void USER_FilterState( u8 on_state );
void USER_SetControlState(u8 state);
uint16_t USER_GetSetting();
uint16_t USER_GetFact(u8 * state);
USER_PROCESS_FSM_t USER_GetProccesState( void  );
TaskHandle_t * getUserProcessTaskHandle( void  );
void user_process_task(void *pvParameters);

#endif /* USER_USER_PROCESS_H_ */
