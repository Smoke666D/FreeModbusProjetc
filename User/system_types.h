/*
 * system_types.h
 *
 *  Created on: Oct 22, 2024
 *      Author: smoke
 */

#ifndef USER_SYSTEM_TYPES_H_
#define USER_SYSTEM_TYPES_H_

typedef struct
{
  u32 timer_counter;
  u8 tumer_on;
  u8 control_state;
  u8 old_control_state;
} CLEAN_TIMER_t;

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
    USER_PROCCES_WORK,
    USER_PROCESS_PAUSE,
    USER_PROCESS_ALARM,
    USER_PROCESS_WORK_TIME_OUT,
    USER_PROCESS_ZERO_CALIB,
    USER_PROCESS_POWER_OFF,
    USER_PROCESS_DOUBLE_CHANNEL_ERROR,
} USER_PROCESS_FSM_t;

typedef struct
{
  u8 Setting_old;
  u8 HEPA_CONTROL_FLAG;
  u32 Setting_Warning_Timeout;
  u32 Filter_Warning_Timeout;
  u32 PreFilter_Warning_Timeout;
  u32 pid_counter;
  u32 start_timeout;
  u32 hepa_counter;
  u8 FilterState;
} FMCH_Device_t;

typedef enum
{
   T_PRIOR = 0,
   CO2_PRIOR = 1,
   H_PRIOR   = 2,
} PRIOR_SENSOR_t;



typedef struct
{
 uint16_t Voltage;
 uint8_t high_voltage_timeout;
 uint8_t low_voltage_timeout;
 uint8_t power_on_flag;
 uint8_t power_off_flag;
 uint8_t power_off_timeout;
} AC_VOLTAGE_CONTROL_t;

#endif /* USER_SYSTEM_TYPES_H_ */
