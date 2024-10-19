/*
 * user_process.c
 *
 *  Created on: Aug 21, 2024
 *      Author: i.dymov
 */
#include "user_process.h"
#include "user_process_service.h"
#include "string.h"
#include "pid.h"
#include "hw_lib_din.h"
#include "hw_lib_adc.h"
#include "hal_timers.h"
#include "data_model.h"
#include "stdlib.h"
#include "math.h"

#define FILTER_WARNINR_VALUE 90





static  PID_TypeDef TPID;
static  PID_TypeDef TPID2;
static u8 setting_change_flag =0;    //Флаг измения значения устаки, нужен для изменения отображения на индикаторе текущей уставки
static TaskHandle_t processTaskHandle;
static USER_PROCESS_FSM_t task_fsm;
static u8 FilterState;
static float setpoint;
static long temp_counter;
static long mb_time_out = 0;
static float SET_POINT;
static float SET_POINT1;
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



u8 getProcessStateCode()
{
    switch( task_fsm )
    {
        default:
            return (STOP_CODE);
        case USER_PEOCESS_WORK_TIME_OUT:
        case USER_PEOCESS_ZERO_CALIB:
           return (CALIBRATE_CODE);
       case USER_RROCCES_WORK:
           return (WORK_CODE);
     }
}

uint16_t USER_GetFact(u8 * state)
{
 if (task_fsm == USER_RROCCES_WORK)
 {
     *state = 1;
    return (u16)( sqrt((float)getAIN(SENS1))*getRegFloat(KOOFKPS)) ;
 }
 else
 {
     *state = 0;
    return 0;
 }
 }




static void USER_SETTING_CHECK(u8 control_type, u8 * point_old)
{
      u8 start;
      if (control_type !=   MKV_MB_DIN)
      {
          mb_time_out++;
          if ( mb_time_out>= getReg8(MOD_BUS_TIMEOUT)*100)
          {
              setReg8(SYSTEM_START, 0);
          }
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

static u32 hepa_counter = 0;


void USER_FilterState( u8 on_state)
{
    if (on_state)
    {
         if (hepa_counter == 6000)
         {
                u16 sensor_data = getAIN(SENS2);
                if (sensor_data  <= getReg16(FILTER_LOW))
                    FilterState = 0;
                else
                    if (sensor_data  >= getReg16(FILTER_HIGH))
                        FilterState = 100;
                    else
                    {
                        u16 temp = sensor_data - getReg16(FILTER_LOW);
                        temp = ((float)temp/(getReg16(FILTER_HIGH) - getReg16(FILTER_LOW)))*100;
                        FilterState = (u8)temp;
                    }
                if (getReg8(MODE) == 0)
                {
                    setReg8(RESURSE, FilterState );
                }
         }
         hepa_counter++;
         if (hepa_counter > 6000) hepa_counter = 0;
     }
     else
        hepa_counter =0;
}

void UPDATE_COOF()
{
    PID_SetTunings2(&TPID,getRegFloat(COOF_P), getRegFloat(COOF_I), 0);
}

void UPDATE_COOFCAV()
{
    PID_SetTunings2(&TPID,getRegFloat(COOF_P), getRegFloat(COOF_I), 0);
    PID_SetTunings2(&TPID2,getRegFloat(COOF_P1), getRegFloat(COOF_I1), 0);
}

static float DAC1_OUT = 0;

float getDAC1_Out()
{
    return (DAC1_OUT);
}

u16 testdata[10]={700,680,670,660,650,670,685,500,550,625};
static float Temp;
static float PIDOut;
static float PIDOut2;

static u16 counterpid = 0;

float setTestDta(float input)
{
  counterpid ++;
  counterpid  =  counterpid %10;
  float out = 625;
  if (input ==0) return 10;
  if (input == 5.5) out = 625.0;
  else
  if (input < 5.5)  out = 600;
  else
  if (input > 5.5 ) out =  670;
//  printf("data %f %f\r\n",input,out);


  return out;
}

static u32 filter_warning_timer =0;
static u32 setting_wrning_timer = 0;

void vResetFilterError()
{
    error_state &=~FILTER_ERROR;
    filter_warning_timer = 0;
}

void vFMCH_FSM( u32 * start_timeout, u32 * pid_counter, u8 * HEPA_CONTROL_ON,  u8 * set_point_old)
{
    u8 c_type  =getReg8( CONTROL_TYPE );
    if (MB_TASK_GetMode()!=2)
    {
       if (c_type == MKV_MB_DIN ) setReg8(LIGTH, ucDinGet(INPUT_2));
       eSetDUT(OUT_2, getReg8(LIGTH));
    }
    USER_SETTING_CHECK(c_type, set_point_old);
    if ( error_state & (LOW_VOLTAGE_ERROR | HIGH_VOLTAGE_ERROR))
    {
        task_fsm = USER_PROCESS_ALARM;
    }
    // Если засоренность фильта больше значения устваки, то выставляем предупрежние и делаем запись в журнал
     USER_FilterState(*HEPA_CONTROL_ON);
     if  ((FilterState >=FILTER_WARNINR_VALUE) && ( *HEPA_CONTROL_ON))
     {
          if ((error_state & FILTER_ERROR) == 0)
          {
               if (++ filter_warning_timer >= 18000)
               {
                   vADDRecord(FILTER_ERROR);
                   error_state |=FILTER_ERROR;
               }
         }
      }
      else
          vResetFilterError();
        //Свет
      if (MB_TASK_GetMode() && (task_fsm != USER_PROCCES_IDLE))
      {
           task_fsm = USER_PROCCES_IDLE;
           HAL_SetBit(CRACH_Port,  CRACH_Pin);
           eSetDUT(OUT_3,FALSE);
      }
    switch (task_fsm)
               {
                   case USER_PROCCES_IDLE: // @suppress("Symbol is not resolved")
                       *HEPA_CONTROL_ON = 0;
                       eSetDUT(OUT_1,FALSE);
                       PIDOut = 0;
                       USER_AOUT_SET(DAC1,0);
                       USER_AOUT_SET(DAC2,0);
                       USER_AOUT_SET(DAC3,0);
                       *start_timeout = 0;
                       temp_counter = 0;
                       error_state = 0;
                       break;
                   case USER_PEOCESS_WORK_TIME_OUT:
                       *HEPA_CONTROL_ON = 0;
                       if ( (++*start_timeout)> ( getReg8(FAN_START_TIMEOUT)*100))
                       {
                           task_fsm = USER_PEOCESS_ZERO_CALIB;
                           CalibrateZeroStart();
                       }
                       break;
                   case USER_PEOCESS_ZERO_CALIB:

                       if (CalibrationZeroWhait())
                       {
                           PIDOut = 0;
                           UPDATE_COOF();
                           PID_Init(&TPID,0,setTestDta( PIDOut));
                           task_fsm = USER_RROCCES_WORK;
                       }
                       break;
                   case USER_RROCCES_WORK:
                       if (++*pid_counter >=10)
                       {
                           *pid_counter = 0;
                           Temp = getAIN(SENS1);
                           PID_Compute(&TPID,getAIN(SENS1));
                           float PID_Out = PIDOut/1000.0;
                           USER_AOUT_SET(DAC2,PID_Out);
                           if ( ((PID_Out) >=9.5) && (Temp < SET_POINT ) )
                           {
                              if  ((error_state & SETTING_ERROR )==0 )
                              {
                                  if ( ++setting_wrning_timer >= 1800 )
                                  {
                                      vADDRecord(SETTING_ERROR);
                                      error_state |= SETTING_ERROR;
                                  }
                              }
                           }
                           else
                           {
                               setting_wrning_timer = 0;
                               error_state &= ~SETTING_ERROR;
                           }
                           if (fabs(SET_POINT-Temp) <= ( SET_POINT*0.02) )
                           {
                               *HEPA_CONTROL_ON = 1;
                           }
                           else
                           {
                               *HEPA_CONTROL_ON = 0;
                           }
                       }
                       eSetDUT(OUT_1,TRUE);
                       break;
                   case USER_PROCESS_ALARM:
                       if ( ( error_state & (LOW_VOLTAGE_ERROR | HIGH_VOLTAGE_ERROR)) == 0 )  task_fsm = USER_PROCCES_IDLE;
                       PIDOut = 0;
                       USER_AOUT_SET(DAC2,0);
                       eSetDUT(OUT_1,FALSE);
                       error_state &= ~SETTING_ERROR;
                       *HEPA_CONTROL_ON = 0;
                       break;
               }

}

void SystemCalibraionStart()
{
   task_fsm = USER_PEOCESS_ZERO_CALIB;
}
void SystemCalibraionStop()
{
   task_fsm = USER_RROCCES_WORK;
}


static u8 din_mask = 0;





void vCDV_SetpointCheck(  DISCRET_STATE_t * state, u32 * timeout  )
{
      //Сбрасываем ошибку дискретных входов
    error_state &=~DIN_ERROR;
    u8 new_state = ( getReg8( CONTROL_TYPE )==  MKV_MB_DIN)? ( (u8)uiGetDinMask() & 0x0F ) : getReg8(CDV_CONTOROL);
    if ( din_mask != new_state )
    {
         din_mask = new_state ;
          *timeout = 0;
    }
    u8 temp_state;
    if ( getReg8( CONTROL_TYPE )==  MKV_MB_DIN )
    {
        switch (din_mask)
            {
                    case 0:

                        temp_state = 0;
                        break;
                    case 1:

                        temp_state = 1;
                        break;
                    case 3:

                        temp_state = 2;
                        break;
                    case 7:

                        temp_state = 3;
                        break;
                    case 0xF:

                        temp_state = 4;
                        break;
                    default:
                        error_state |=DIN_ERROR;
                        return;
                    break;
                }
    }
    else
    {
        temp_state = din_mask;

    }

    if (*state!=temp_state)
    {
            if ( ++(*timeout) >= getReg8(SETTING_TIMER)*100 )
            {
                   *state = temp_state;
                   setReg8(CDV_CONTOROL,temp_state);
            }
        }

    CleanTimerFuncton(state);
    return ;
}

float ComputeSetPoint()
{
    float temp_float = 0;
    switch (getReg8( INPUT_SENSOR_MODE))
    {
        case 0:
            temp_float =( getAIN(AIN1)/10.0*(getReg16(SETTING_MAX) - getReg16(SETTING_MIN))) + getReg16(SETTING_MIN);
            break;
        case 1:
            temp_float =( (getAIN(AIN1)-2.0)/8.0*(getReg16(SETTING_MAX) - getReg16(SETTING_MIN))) + getReg16(SETTING_MIN);
            break;
        default:
            temp_float =( (getAIN(AIN1)-4.0)/18.0*(getReg16(SETTING_MAX) - getReg16(SETTING_MIN))) + getReg16(SETTING_MIN);
            break;
    }
    return (temp_float);
}

static DISCRET_STATE_t state = SETTING_CLOSE;

u8 getStateDCV()
{
    return state;
}

u8 getStateVAV()
{
    return state;
}


void vBP()
{


}


void Channel2Reg( u8 state, float setpoint )
{
    float PID_Out;
    u8 ch_count = getReg8(CDV_BP_CH_COUNT);
    if (ch_count  == 2)
    {
         if (state == SETTING_CLOSE) PID_Out = 0.0;
         else if (state == SETTING_OPEN ) PID_Out = 10.0;
         else
         {

            SET_POINT1  = setpoint + (float)getReg16(OFFSET_CH2);
            PID_Compute(&TPID2,getAIN(SENS2));
            PID_Out = PIDOut2/1000.0;
         }

        USER_AOUT_SET(DAC2,PID_Out);
    }
    else if (ch_count  == 0) vBP();
}


void vRoomContollerFSM( u8 state)
{
    float PID_Out;
    if ( state == SETTING_OPEN ) PID_Out = 10.0;
    else if (state == SETTING_CLOSE) PID_Out = 0.0;
    else
    {
        SET_POINT = ComputeSetPoint();
        PID_Compute(&TPID,getAIN(SENS1));
        PID_Out = PIDOut/1000.0;
    }
    USER_AOUT_SET(DAC1,PID_Out);
    Channel2Reg(state,getAIN(SENS1) );
}


AIN_NAME_t const SENSOR_NAME[]={DCAIN1,DCAIN2,DCAIN3};




void ErrorSensorCheck()
{
    AIN_NAME_t sensor_name;
    switch ((INPUT_SENSOR_t)getReg8(SENSOR_TYPE_ID))
    {
        case ANALOG_SENSOR:
        case ROOM_CONTROLLER:

            sensor_name = SENSOR_NAME[getReg8(PRIOR_SENSOR)];
            switch (getReg8( INPUT_SENSOR_MODE))
            {
                case T2_10:
                    if (getAIN(sensor_name) < 2.0 ) error_state |= ANALOG_SENSOR_ERROR;
                    else error_state &= ~ANALOG_SENSOR_ERROR;
                    break;
                case T4_20:
                    if (getAIN(sensor_name) < 4.0 ) error_state |= ANALOG_SENSOR_ERROR;
                    else error_state &= ~ANALOG_SENSOR_ERROR;
                    break;
                default:
                    error_state &= ~ANALOG_SENSOR_ERROR;
                    break;
            }
            break;
        case STATIC_TERMSENSOR:
            break;
        default:
            break;

    }



}


float GetSensor(u8 * after_zone)
{
   float temp_float = 0;

   if (getReg8(SENSOR_TYPE_ID) == STATIC_TERMSENSOR)
   {
       temp_float = getAIN(DCAIN4);
       *after_zone = 1;
   }
   else
   {
       AIN_NAME_t sensor_name = SENSOR_NAME[getReg8(PRIOR_SENSOR)];
       if ( sensor_name == DCAIN1 ) *after_zone = 1;
       float min_data = getRegFloat(MIN_SET);
       float max_data = getRegFloat(MAX_SET);
       float offset   = getRegFloat(SENS_OFS);
       switch (getReg8( INPUT_SENSOR_MODE))
       {
          case T0_10:
              temp_float =( getAIN(sensor_name)/10.0*(max_data - min_data)) + min_data;
              break;
          case T2_10:
              temp_float =( (getAIN(sensor_name)-2.0)/8.0*(max_data - min_data)) + min_data;
              break;
          default:
              temp_float =( (getAIN(sensor_name)-4.0)/18.0*(max_data - min_data)) + min_data;
              break;
      }
      temp_float +=offset;
   }
  return (temp_float);

}



void vAnalogSensorFSM( DISCRET_STATE_t state)
{
    float PID_Out;
    if ( state == 4 ) PID_Out =10.0;
    else if (state == 0) PID_Out =0.0;
    else
    {
            float temp_float = getAIN(SENS1);
            if (temp_float <= getReg16(SETTING_MIN))
            {
                PID_SetOutputLimits(&TPID, USER_AOUT_GET(DAC1),TPID.OutMax);
            }
            if (temp_float >= getReg16(SETTING_MAX))
            {
                  PID_SetOutputLimits(&TPID, TPID.OutMin, USER_AOUT_GET(DAC1));
            }
            SET_POINT = getRegFloat(SENS_SETTING);
            u8 after_zone = 0;
            float input_data = GetSensor(&after_zone);
            if ( after_zone )
            {

                switch ( getReg8(AFTER_ZONE_SETTING ))
                {
                    case 0:
                        PID_SetControllerDirection(&TPID,_PID_CD_DIRECT );
                        break;
                    case 1:
                        PID_SetControllerDirection(&TPID,_PID_CD_REVERSE );
                        break;
                    default:
                        if (input_data < getAIN(DCAIN4))
                            PID_SetControllerDirection(&TPID,_PID_CD_DIRECT );
                        else
                            PID_SetControllerDirection(&TPID,_PID_CD_REVERSE);
                        break;
                }
            }
            else PID_SetControllerDirection(&TPID,_PID_CD_DIRECT );
            PID_Compute(&TPID,input_data);
            PID_Out = PIDOut/1000.0;
    }
    USER_AOUT_SET(DAC1,PID_Out);
    Channel2Reg(state,getAIN(SENS1));

}





void vDiscreteInputFSM( DISCRET_STATE_t state)
{
    switch (state)
    {
        case SETTING_OPEN:
             USER_AOUT_SET(DAC1,10.0);
             break;
        case SETTING_MINIMUM:
             SET_POINT = (float)getReg16(SETTING_MIN);
             break;
        case SETTING_MIDIUM:
             SET_POINT = (float)getReg16(SETTING_MID);
             break;
        case SETTING_MAXIMUN:
             SET_POINT = (float)getReg16(SETTING_MAX);
             break;
        default:
             USER_AOUT_SET(DAC1,0.0);
             break;

     }
    if ((state>=1) && (state<=3))
    {
          PID_Compute(&TPID,getAIN(SENS1));
          float PID_Out = PIDOut/1000.0;
          USER_AOUT_SET(DAC1,PID_Out);
     }
    Channel2Reg(state,SET_POINT);

}

void vCDV_FSM( u32 * start_timeout, u32 * pid_counter, u8 * cal_flag)
{
    vCDV_SetpointCheck(&state, start_timeout);
    ErrorSensorCheck();
    vCheckDoubleChannelAlarm(&error_state);
    switch (task_fsm)
    {
            case USER_PROCCES_IDLE:
                error_state = 0;
                *start_timeout = 0;
                InitCleanTimer();
                task_fsm =USER_RROCCES_WORK;
                break;
            case USER_RROCCES_WORK:
                *cal_flag = 0;
                if ((error_state & DIN_ERROR) || (error_state & ANALOG_SENSOR_ERROR))
                {
                   task_fsm  = USER_PROCESS_ALARM;
                }
                else if (error_state & ( FIRST_CHANNEL_ERROR | SECOND_CHANNEL_ERROR ))
                {
                    task_fsm  = USER_PROCESS_DOUBLE_CHANNEL_ERROR;
                }
                else
                {

                    if (++*pid_counter >=10)
                    {
                        UPDATE_COOFCAV();
                        *pid_counter = 0;
                        switch ((INPUT_SENSOR_t)getReg8(SENSOR_TYPE_ID))
                        {
                            case DISCRETE_INPUT:
                                vDiscreteInputFSM(state);
                                break;
                            case ROOM_CONTROLLER:
                                vRoomContollerFSM(state);
                                break;
                            case ANALOG_SENSOR:
                            case STATIC_TERMSENSOR:
                                vAnalogSensorFSM(state);
                                break;
                        }
                    }
                }
                break;
            case USER_PEOCESS_ZERO_CALIB:
                if ((*start_timeout) >= ( getReg16(ZERO_POINT_TIMEOUT)*6000) )
                {
                    printf("start \r\n");
                    if (*cal_flag == 0)
                    {
                        CalibrateZeroStart();
                        *cal_flag = 1;
                    }
                    else
                    {
                        if (CalibrationZeroWhait())
                        {
                            *start_timeout = 0;
                            task_fsm = USER_RROCCES_WORK;
                        }
                    }
                }
                else
                {
                    (*start_timeout)++;
                    if (getReg8(CDV_BP_CH_COUNT) >1 )  USER_AOUT_SET(DAC1,0);
                    USER_AOUT_SET(DAC2,0);
                }
                break;
            case USER_PROCESS_ALARM:

                if (error_state & (HIGH_VOLTAGE_ERROR | LOW_VOLTAGE_ERROR | DIN_ERROR | ANALOG_SENSOR_ERROR) == 0) task_fsm = USER_PROCCES_IDLE;

                if (error_state & ( DIN_ERROR  | ANALOG_SENSOR_ERROR))
                {
                    USER_AOUT_SET(DAC1,0.0);
                    USER_AOUT_SET(DAC2,0.0);
                }
                break;
            case USER_PROCESS_DOUBLE_CHANNEL_ERROR:
                if (error_state & FIRST_CHANNEL_ERROR)
                {
                    USER_AOUT_SET(DAC2,0.0);
                    USER_AOUT_SET(DAC1,10.0);
                }
                else if (error_state & SECOND_CHANNEL_ERROR)
                {
                    USER_AOUT_SET(DAC1,0.0);
                    USER_AOUT_SET(DAC2,10.0);

                }
                else
                {
                    task_fsm = USER_PROCCES_IDLE;
                }
                break;

    }
}




typedef struct
{
 uint16_t Voltage;
 uint8_t high_voltage_timeout;
 uint8_t low_voltage_timeout;
 uint8_t power_on_flag;
 uint8_t power_off_flag;
 uint8_t power_off_timeout;
} AC_VOLTAGE_CONTROL_t;


void InitVoltageControl( AC_VOLTAGE_CONTROL_t * ac_control )
{
    ac_control->high_voltage_timeout = 0;
    ac_control->low_voltage_timeout = 0;
    ac_control->power_on_flag = 0;
    ac_control->power_off_flag = 0;
    ac_control->power_off_timeout = 0;

}

void VoltageControlCheck( AC_VOLTAGE_CONTROL_t * ac_control)
{
    if ( ac_control->power_on_flag)
    {
         if ( ac_control->Voltage >= (uint16_t)getReg8(HIGH_VOLTAGE_ON))
         {
             if ((error_state & HIGH_VOLTAGE_ERROR) == 0)
             {
                 if  (++ac_control->high_voltage_timeout>100)
                 {
                     error_state |= HIGH_VOLTAGE_ERROR;
                     vADDRecord(HIGH_VOLTAGE_ERROR);
                 }
             }
         }
         else
             ac_control->high_voltage_timeout = 0;

         if ( ac_control->Voltage <= (uint16_t)getReg8(LOW_VOLTAGE_ON))
         {
             if ((error_state & LOW_VOLTAGE_ERROR) == 0)
             {
                 if  (++ac_control->low_voltage_timeout>100)
                 {
                     error_state |= LOW_VOLTAGE_ERROR;
                     vADDRecord(LOW_VOLTAGE_ERROR);
                 }
             }
         }
         else
             ac_control->low_voltage_timeout = 0;
         if  ((ac_control->Voltage <= (uint16_t)getReg8(HIGH_VOLTAGE_OFF)) && ( ac_control->Voltage >=  (uint16_t)getReg8(LOW_VOLTAGE_OFF)))
         {
             error_state &= (~(LOW_VOLTAGE_ERROR | HIGH_VOLTAGE_ERROR ));
         }
         if ( ac_control->Voltage < 20 )
         {
             if    (++ac_control->power_off_timeout> 2)
              {
                     HAL_ResetBit(LDCDATA_2_3_E_REW_CD_LED_Port,  LCDLED_Pin);
                     SaveBeforePowerOff();
                     ac_control->power_on_flag = 0;
                     ac_control->power_off_flag = 1;
                     task_fsm = USER_PROCCES_IDLE;
              }
         }
         else {
             ac_control->power_off_timeout = 0;
        }
    }
    else
    {
        if  ( ac_control->Voltage >= (uint16_t)getReg8(LOW_VOLTAGE_OFF) ) ac_control->power_on_flag = 1;
        if ((ac_control->Voltage >40) && (ac_control->power_off_flag))
        {
              vTaskDelay(10);
              NVIC_SystemReset();
        }
    }

}


void user_process_task(void *pvParameters)
{
   static AC_VOLTAGE_CONTROL_t ac_contorl;
   static u8 HEPA_CONTROL_ON = 0;
   static u8 flag = 0;
   u8 set_point_old = 2;
   u32 pid_counter = 0;
   u32 start_timeout = 0;
   u8 process_mode = getReg8(DEVICE_TYPE);
   error_state = 0;
   task_fsm = USER_PROCCES_IDLE;
   FilterState = 0;
   PID(&TPID,  &PIDOut, &SET_POINT, getRegFloat(COOF_P), getRegFloat(COOF_I), 0, _PID_CD_DIRECT);
   PID_SetOutputLimits(&TPID,(float)1000.0,(float)10000.0);
   PID(&TPID2, &PIDOut2, &SET_POINT1, getRegFloat(COOF_P), getRegFloat(COOF_I), 0, _PID_CD_DIRECT);
   PID_SetOutputLimits(&TPID2,(float)0000.0,(float)10000.0);
   InitVoltageControl(&ac_contorl);
   while(1)
   {
       vTaskDelay(10);
       if (MB_TASK_GetMode()!=2)
       {
           ac_contorl.Voltage = (uint16_t)getAIN(AC220);
           VoltageControlCheck(&ac_contorl);
           if (process_mode == DEV_FMCH )
                   vFMCH_FSM(&start_timeout, &pid_counter, &HEPA_CONTROL_ON , &set_point_old);
           else
                   vCDV_FSM(&start_timeout,&pid_counter,&flag );

           //Ecли есть ошибка включаем реле и зажигаем светодиод
           if ( error_state  & (~SETTING_ERROR))
           {
                 HAL_ResetBit(CRACH_Port,  CRACH_Pin);
                 eSetDUT(OUT_3,TRUE);
           }
           else
           {
                 HAL_SetBit(CRACH_Port,  CRACH_Pin);
                 eSetDUT(OUT_3,FALSE);
           }
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
            HAL_TIMER_SetPWMPulse(TIMER9,TIM_CHANNEL_1 ,ref );
            break;
        case DAC2:
            HAL_TIMER_SetPWMPulse(TIMER9,TIM_CHANNEL_2 ,ref  );
            break;
        case DAC3:
            HAL_TIMER_SetPWMPulse(TIMER9,TIM_CHANNEL_3 ,ref  );
            break;
    }
}

float USER_AOUT_GET(u8 channel)
{
    return (AOUTDATA[channel]);
}

