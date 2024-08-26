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

static double setpoint;
static u16 Fact;
static u8 FSM_STATE;
uint16_t USER_GetSetting()
{
    return (u16)(setpoint);
}

uint16_t USER_GetFact()
{
    return (u16)( getRegFloat(KOOFKPS)*(float)getAIN(SENS1)) ;
}

void user_process_task(void *pvParameters)
{
   static PID_TypeDef TPID;
   double Temp, PIDOut;
   long temp_counter=0;
   u16 ref;
   uint8_t ac_voltage;
   u8 set_point_old = 0;
   u8 set_point_flag =0;
   task_fsm = USER_PROCCES_IDLE;
   PID(&TPID, &Temp, &PIDOut, & setpoint, getRegFloat(COOF_P), getRegFloat(COOF_I), 0, _PID_P_ON_E, _PID_CD_DIRECT);
   PID_SetMode(&TPID, _PID_MODE_AUTOMATIC);
   PID_SetOutputLimits(&TPID,0,9.9);
   PID_SetSampleTime(&TPID, 20);
   while(1)
   {
       Fact = 0;
       ac_voltage = getACVoltage();

       if  (ac_voltage >= getReg8(HIGH_VOLTAGE_ON))
       {
           if (task_fsm != USER_PROCESS_ALARM)
           {
               vADDRecord(HIGH_VOLTAGE_ERROR);
               task_fsm = USER_PROCESS_ALARM;
           }
       }
       else if ( ac_voltage <=  getReg8(LOW_VOLTAGE_ON))
       {
           if (task_fsm != USER_PROCESS_ALARM)
           {
               vADDRecord(LOW_VOLTAGE_ERROR);
               task_fsm = USER_PROCESS_ALARM;
           }
       }
    /*   if (ucDinGet(OUT_2))
       {
           if ( set_point_old == 0) set_point_flag = 1;
           set_point_old = 1;
           setpoint = getReg16(SETTING1);
       }
       else
       {
           if ( set_point_old == 1) set_point_flag = 1;
           set_point_old = 0;
           setpoint = getReg16(SETTING2);
       }
       if ( FSM_STATE >= 3 )
       {
          // HAL_SetBit(CRACH_Port,  CRACH_Pin);
           eSetDUT(OUT_2,TRUE);
       }*/
       switch (task_fsm)
       {
           case USER_PROCCES_IDLE:
               vTaskDelay(20);
              // HAL_SetBit(CRACH_Port,  CRACH_Pin);
               eSetDUT(OUT_2,FALSE);
               FSM_STATE = 0;
               temp_counter = 0;
               break;
           case USER_RROCCES_WORK:
               FSM_STATE = 1;
               if ((++temp_counter) >= 30000 )
               {
                   vADDRecord(SETTING_ERROR);
                   temp_counter = 30000;
                   FSM_STATE = 4;
               }
               vTaskDelay(20);
               eSetDUT(OUT_1,TRUE);
               Temp = getAIN(SENS1);
               if (abs(setpoint-Temp)  <= ( setpoint*0.05) )
               {
                   task_fsm = USER_PROCEES_RUN;
               }
               PID_Compute(&TPID);
               if (PIDOut >=9.5)
               {
                   vADDRecord(SETTING_ERROR);
                   FSM_STATE = 4;
               }
               ref = (uint16_t)fGetDacCalData(DAC2,PIDOut);
               HAL_TIMER_SetPWMPulse(TIMER9,TIM_CHANNEL_2 ,ref *10 );

               break;
           case USER_PROCEES_RUN:
               FSM_STATE = 2;
               if ( set_point_flag == 1)  task_fsm = USER_RROCCES_WORK;
               break;
           case USER_PROCESS_ALARM:
               FSM_STATE = 3;
               if  ((ac_voltage <= getReg8(HIGH_VOLTAGE_OFF)) && ( ac_voltage >=  getReg8(LOW_VOLTAGE_OFF)))
               {
                    task_fsm = USER_PROCCES_IDLE;
               }
               HAL_ResetBit(CRACH_Port,  CRACH_Pin);
               eSetDUT(OUT_1,FALSE);
               vTaskDelay(20);
               break;
       }
   }
}
