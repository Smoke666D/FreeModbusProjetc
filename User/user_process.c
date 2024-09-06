/*
 * user_process.c
 *
 *  Created on: Aug 21, 2024
 *      Author: i.dymov
 */
#include "user_process.h"
#include "string.h"
#include "pid.h"
#include "hw_lib_din.h"
#include "hw_lib_adc.h"
#include "hal_timers.h"
#include "data_model.h"
#include "stdlib.h"
#include "math.h"

#define FILTER_WARNINR_VALUE 98
#define FILTER_ERROR  0x01
#define SETTING_ERROR 0x02
#define LOW_VOLTAGE_ERROR 0x04
#define HIGH_VOLTAGE_ERROR 0x08





static u8 setting_change_flag =0;    //Флаг измения значения устаки, нужен для изменения отображения на индикаторе текущей уставки
static TaskHandle_t processTaskHandle;
static USER_PROCESS_FSM_t task_fsm;
static u8 FilterState;
static double setpoint;
static long temp_counter;
static long mb_time_out = 0;
static double SET_POINT;
static u8 error_state;

TaskHandle_t * getUserProcessTaskHandle()
{
    return (&processTaskHandle);
}

void USER_SetSettingChange()
{
    setting_change_flag = 1;
}

u8 USER_GerErrorState()
{
    return (error_state);
}

void USER_SetControlState(u8 state)
{
    mb_time_out = 0;
    setReg8(SYSTEM_START,state);

}



uint16_t USER_GetSetting()
{
    return (u16)(setpoint);
}
uint16_t USER_GetFilterState()
{
    return ( FilterState);
}

USER_PROCESS_FSM_t USER_GetProccesState()
{
    return (task_fsm);
}

uint16_t USER_GetFact()
{
    return (u16)(sqrt( (float)getAIN(SENS2))*getRegFloat(KOOFKPS)) ;
}



static void USER_SETTING_CHECK(u8 control_type, u8 * point_old)
{
      u8 start;
      if (control_type !=   MKV_MB_DIN)
      {
          mb_time_out++;
          if ( mb_time_out>= getReg8(MOD_BUS_TIMEOUT)*100)
              setReg8(SYSTEM_START, 0);
      }
      else
      {
          mb_time_out =0;
      }
       start = (control_type ==   MKV_MB_DIN) ? ucDinGet(INPUT_1) : getReg8(SYSTEM_START) ;
      if ( start && (task_fsm == USER_PROCCES_IDLE))
      {
          task_fsm = USER_PEOCESS_WORK_TIME_OUT;
      }
      if ( (start==0) && (task_fsm != USER_PROCCES_IDLE) &&  (task_fsm != USER_PROCESS_ALARM))
      {
           task_fsm = USER_PROCCES_IDLE;
      }
    if ( control_type == MKV_MB_DIN) setReg8(MODE,ucDinGet(INPUT_3));

    if ((getReg8(MODE ) != *point_old) || (setting_change_flag))
    {
        setting_change_flag  = 0;
        *point_old = getReg8(MODE );
        setpoint = getReg16((*point_old ==0)? SETTING1 : SETTING2);
        SET_POINT =pow(setpoint/getRegFloat(KOOFKPS),2);

    }

}


u8 USER_FilterState()
{
    u16 sensor_data = getAIN(SENS1);
    if (sensor_data  <= getReg16(FILTER_LOW))
            return (0);
    if (sensor_data  >= getReg16(FILTER_HIGH))
            return (100);
    u16 temp = sensor_data - getReg16(FILTER_LOW);
    temp = temp/(getReg16(FILTER_HIGH) - getReg16(FILTER_LOW));
    return (temp);
}

static float DAC1_OUT = 0;

float getDAC1_Out()
{
    return (DAC1_OUT);
}

void user_process_task(void *pvParameters)
{
   PID_TypeDef TPID;
   double Temp, PIDOut;
   u16 ref;
   uint8_t ac_voltage;
   u8 set_point_old = 2;
   u32 pid_counter = 0;
   u32 start_timeout = 0;
   error_state = 0;
   task_fsm = USER_PROCCES_IDLE;
   FilterState = 0;
   PID(&TPID, &Temp, &PIDOut, &SET_POINT, getRegFloat(COOF_P), getRegFloat(COOF_I), 0, _PID_P_ON_E, _PID_CD_DIRECT);
   PID_SetMode(&TPID, _PID_MODE_AUTOMATIC);
   PID_SetOutputLimits(&TPID,0,9.9);
   PID_SetSampleTime(&TPID, 20);
   while(1)
   {
       vTaskDelay(10);
       ac_voltage = getACVoltage();
       if  (ac_voltage >= getReg8(HIGH_VOLTAGE_ON))
       {
           if (task_fsm != USER_PROCESS_ALARM)
           {
               error_state |= HIGH_VOLTAGE_ERROR;
               vADDRecord(HIGH_VOLTAGE_ERROR);
               task_fsm = USER_PROCESS_ALARM;
           }
       }
       else if ( ac_voltage <=  getReg8(LOW_VOLTAGE_ON))
       {
           if (task_fsm != USER_PROCESS_ALARM)
           {
               error_state |= LOW_VOLTAGE_ERROR;
               vADDRecord(LOW_VOLTAGE_ERROR);
               task_fsm = USER_PROCESS_ALARM;
           }
       }
       u8 c_type =getReg8(CONTROL_TYPE );
       USER_SETTING_CHECK(c_type, &set_point_old);
// Если засоренность фильта больше значения устваки, то выставляем предупрежние и делаем запись в журнал
       FilterState = USER_FilterState();
       if  ((FilterState >=FILTER_WARNINR_VALUE) && ((error_state & FILTER_ERROR) == 0))
       {
            vADDRecord(FILTER_ERROR);
            error_state |=FILTER_ERROR;
       }
//Ecли есть ошибка включаем реле и зажигаем светодиод
       if ( error_state )
       {
           HAL_ResetBit(CRACH_Port,  CRACH_Pin);
           eSetDUT(OUT_3,TRUE);
       }
       else
       {
           HAL_SetBit(CRACH_Port,  CRACH_Pin);
           eSetDUT(OUT_3,FALSE);
       }
       eSetDUT(OUT_2, (c_type == MKV_MB_DIN )? ucDinGet(INPUT_2) : getReg8(LIGTH) ); //Свет
       switch (task_fsm)
       {
           case USER_PROCCES_IDLE:
               eSetDUT(OUT_1,FALSE);
               PIDOut = 0;
               USER_AOUT_SET(DAC1,0);
               start_timeout = 0;
               temp_counter = 0;
               error_state = 0;
               break;
           case USER_PEOCESS_WORK_TIME_OUT:
               if ( (++start_timeout)> ( getReg8(FAN_START_TIMEOUT)*100))
               {
                   task_fsm = USER_RROCCES_WORK;
                   CalibrateZeroStart();
               }
               PID_Init(&TPID);
               break;
           case USER_RROCCES_WORK:
               if (++pid_counter >=10)
               {

                   pid_counter = 0;
                   Temp = getAIN(SENS1);
                   if (abs(SET_POINT-Temp)  <= ( SET_POINT *0.05) )
                   {
                       task_fsm = USER_PROCEES_RUN;
                   }
                   PID_Compute(&TPID);
                   if ((PIDOut >=9.5) && ((error_state & SETTING_ERROR )==0))
                   {
                        vADDRecord(SETTING_ERROR);
                        error_state |= SETTING_ERROR;
                    }
                    USER_AOUT_SET(DAC1,PIDOut);

               }
               if ((++temp_counter) == 30000 )
               {
                   vADDRecord(SETTING_ERROR);
                   error_state |= SETTING_ERROR;
               }
               if ( temp_counter > 30000) temp_counter = 30001;
               eSetDUT(OUT_1,TRUE);
               break;
           case USER_PROCEES_RUN:

               if (abs(SET_POINT-Temp) > ( SET_POINT*0.05) )
               {
                   task_fsm = USER_RROCCES_WORK;
                   temp_counter = 0;
               }
               break;
           case USER_PROCESS_ALARM:
               if  ((ac_voltage <= getReg8(HIGH_VOLTAGE_OFF)) && ( ac_voltage >=  getReg8(LOW_VOLTAGE_OFF)))
               {
                    task_fsm = USER_PROCCES_IDLE;
               }
               PIDOut = 0;
               USER_AOUT_SET(DAC1,0);
               eSetDUT(OUT_1,FALSE);
               break;
       }
   }
}

static float AOUTDATA[3]={0,0,0};
void USER_AOUT_SET(u8 channel, float data)
{
    AOUTDATA[channel]= data;
    u16 ref = (uint16_t)fGetDacCalData(channel,data);
    switch (channel)
    {
        case DAC1:
            HAL_TIMER_SetPWMPulse(TIMER9,TIM_CHANNEL_1 ,ref *10 );
            break;
        case DAC2:
            HAL_TIMER_SetPWMPulse(TIMER9,TIM_CHANNEL_2 ,ref *10 );
            break;
        case DAC3:
            HAL_TIMER_SetPWMPulse(TIMER9,TIM_CHANNEL_3 ,ref *10 );
            break;
    }


}

float USER_AOUT_GET(u8 channel)
{
    return (AOUTDATA[channel]);
}

