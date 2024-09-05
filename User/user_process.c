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
static long temp_counter;
static long mb_time_out = 0;
static u8 control_state =0;

void USER_SetControlState(u8 state)
{
    mb_time_out = 0;
    control_state =  state;
}

u8 getControlState()
{
    return (control_state);
}

uint16_t USER_GetSetting()
{
    return (u16)(setpoint);
}

uint16_t USER_GetFact()
{
    return (u16)(getAIN(SENS2)*getRegFloat(KOOFKPS)) ;
}



static void USER_OnOffControll()
{
  u8 start;
  u8 control_type = getReg8(CONTROL_TYPE );
  if (control_type !=   MKV_MB_DIN)
  {
      mb_time_out++;
      if ( mb_time_out>= getReg8(MOD_BUS_TIMEOUT)*100)
          control_state = 0;
  }
  else
  {
      mb_time_out =0;
  }
   start = (control_type ==   MKV_MB_DIN) ? ucDinGet(INPUT_1) : control_state ;
  if ( start && (task_fsm == USER_PROCCES_IDLE))
  {
      task_fsm = USER_PEOCESS_WORK_TIME_OUT;
  }
  if ( (start==0) && (task_fsm != USER_PROCCES_IDLE) &&  (task_fsm != USER_PROCESS_ALARM))
  {
       task_fsm = USER_PROCCES_IDLE;
  }
}

static void USER_SETTING_CHECK( u8 * point_old,u8 * flag)
{

    if (getReg8(CONTROL_TYPE )== MKV_MB_DIN) setReg8(MODE,ucDinGet(INPUT_2));

    if (getReg8(MODE ))
        {
            if ( *point_old == 0)*flag = 1;
            *point_old = 1;
             setpoint = getReg16(SETTING1);
        }
        else
        {
            if ( *point_old == 1) *flag = 1;
            *point_old = 0;
             setpoint = getReg16(SETTING2);
        }

}

void user_process_task(void *pvParameters)
{
   static PID_TypeDef TPID;
   double Temp, PIDOut;
   u16 ref;
   uint8_t ac_voltage;
   u8 set_point_old = 0;
   u8 set_point_flag =0;
   u32 pid_counter = 0;
   u32 start_timeout = 0;
   task_fsm = USER_PROCCES_IDLE;
   PID(&TPID, &Temp, &PIDOut, & setpoint, getRegFloat(COOF_P), getRegFloat(COOF_I), 0, _PID_P_ON_E, _PID_CD_DIRECT);
   PID_SetMode(&TPID, _PID_MODE_AUTOMATIC);
   PID_SetOutputLimits(&TPID,0,9.9);
   PID_SetSampleTime(&TPID, 20);
   while(1)
   {
       vTaskDelay(10);
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

       USER_OnOffControll();
       USER_SETTING_CHECK( &set_point_old,&set_point_flag );
       if ( FSM_STATE >= 3 )
       {
           HAL_ResetBit(CRACH_Port,  CRACH_Pin);
           eSetDUT(OUT_2,TRUE);
       }
       else
       {
           HAL_SetBit(CRACH_Port,  CRACH_Pin);
           eSetDUT(OUT_2,FALSE);
       }
       eSetDUT(OUT_3,ucDinGet(INPUT_3) || getReg8(LIGTH)); //Свет



       switch (task_fsm)
       {

           case USER_PROCCES_IDLE:

               eSetDUT(OUT_1,FALSE);
               FSM_STATE = 0;
               start_timeout = 0;
               temp_counter = 0;
               break;
           case USER_PEOCESS_WORK_TIME_OUT:
               if ( (++start_timeout)> ( getReg8(FAN_START_TIMEOUT)*100))
                   task_fsm = USER_RROCCES_WORK;
               break;
           case USER_RROCCES_WORK:
               if (++pid_counter >=10)
               {
                   pid_counter = 0;
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
               }

               FSM_STATE = 1;
               if ((++temp_counter) >= 30000 )
               {
                   vADDRecord(SETTING_ERROR);
                   temp_counter = 30000;
                   FSM_STATE = 4;
               }
               eSetDUT(OUT_1,TRUE);
               break;
           case USER_PROCEES_RUN:
               FSM_STATE = 2;
               if ((abs(setpoint-Temp) > ( setpoint*0.05) ) || ( set_point_flag == 1))
               {
                   task_fsm = USER_RROCCES_WORK;
                   temp_counter = 0;
               }
               break;
           case USER_PROCESS_ALARM:
               FSM_STATE = 3;
               if  ((ac_voltage <= getReg8(HIGH_VOLTAGE_OFF)) && ( ac_voltage >=  getReg8(LOW_VOLTAGE_OFF)))
               {
                    task_fsm = USER_PROCCES_IDLE;
               }
               eSetDUT(OUT_1,FALSE);
               vTaskDelay(20);
               break;
       }
   }
}
