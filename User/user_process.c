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

#include "hw_lib_adc.h"
#include "hal_timers.h"

#include "stdlib.h"
#include "math.h"
#include "system_types.h"

static const u16 PCOOFMAP[] ={COOF_P_CAV,COOF_PT,COOF_PCO2,COOF_PH};
static const u16 ICOOFMAP[] ={COOF_I_CAV,COOF_IT,COOF_ICO2,COOF_IH};
static uint32_t zero_calibration_timer = 0;
static  DISCRET_STATE_t temp_control_state = SETTING_CLOSE;
static const uint16_t SettingRegMap[]={SENS_SETTING1,SENS_SETTING2,SENS_SETTING3};

#define FILTER_WARNINR_VALUE 90
static PID_TypeDef TPID;
static PID_TypeDef TPID2;
static PID_TypeDef TPID3;
static u8 setting_change_flag =0;    //Флаг измения значения устаки, нужен для изменения отображения на индикаторе текущей уставки
static TaskHandle_t processTaskHandle;
static USER_PROCESS_FSM_t task_fsm;
static float setpoint;
static long temp_counter;
static long mb_time_out = 0;
static float SET_POINT;
static float SET_POINT1;
static float SET_POINT3;
static u8 error_state;
static float Temp;
static float PIDOut;
static float PIDOut2;
static float PIDOut3;
static u8 cur_state = 0;
static u8 system_start =  MKV_MB_RTU;
static u8 din_state_update = 0;
static u32 start_clear_timer =0;

static void __switch_to_calibration_state(void);
static float CumputeChannel2Setpoit(float setpoint);

float GetChanne2Setting()
{
   static float setpoit;
   if  (getReg8(INPUT_CONTROL_TYPE) == DISCRETE_INPUT)
     setpoit = SET_POINT;
   else
     setpoit = getAIN(SENS1);

   return (setpoit);
}

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
        case USER_PROCESS_WORK_TIME_OUT:
        case USER_PROCESS_ZERO_CALIB:
           return (CALIBRATE_CODE);
       case USER_PROCCES_WORK:
           return (WORK_CODE);
     }
}

uint16_t USER_GetFact(u8 * state)
{
 if ( task_fsm == USER_PROCCES_WORK )
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

static void USER_SETTING_CHECK(u8 control_type, FMCH_Device_t * dev)
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
          task_fsm = USER_PROCESS_WORK_TIME_OUT;
      }
      if ( (start==0) && (task_fsm != USER_PROCCES_IDLE) && (task_fsm != USER_PROCESS_ALARM))
      {
           task_fsm = USER_PROCCES_IDLE;
      }
      if ( control_type == MKV_MB_DIN) setReg8(MODE,ucDinGet(INPUT_3));
      if ((getReg8(MODE ) != dev->Setting_old) || (setting_change_flag))
      {
          setting_change_flag  = 0;
          dev->Setting_old = getReg8(MODE );
          setpoint = getReg16((dev->Setting_old==0)? SETTING1 : SETTING2);
          SET_POINT =pow(setpoint/getRegFloat(KOOFKPS),2);
      }
}

float UPDATE_PRES_COOFCAV()
{
    PID_SetTunings2(&TPID2,getRegFloat(COOF_P1),getRegFloat(COOF_I1), 0);
    PID_SetTunings2(&TPID,getRegFloat(PCOOFMAP[0]),getRegFloat(ICOOFMAP[0]), 0);
    return ( getAIN(SENS1) );
}

float UPDATE_COOFCAV( INPUT_SENSOR_t inp_sensor, DISCRET_STATE_t control_state)
{
   // vAnalogSensorFSM( );
    u8 index =0;
    PRIOR_SENSOR_t prior = getReg8(PRIOR_SENSOR);
    if ((inp_sensor == STATIC_TERMSENSOR) || (prior==T_PRIOR))
    {
        index = 1;
    }
    else if (prior==H_PRIOR)
    {
        index = 3;
     }
     else
    {
        index = 2;
    }

    PID_SetTunings2(&TPID3,getRegFloat(PCOOFMAP[index]),getRegFloat(ICOOFMAP[index]), 0);
    PID_SetOutputLimits(&TPID3,(float)getRegFloat(SETTING_MIN),getRegFloat(SETTING_MAX));
    u8 after_zone = 0;
    float input_data = GetSensor(&after_zone, inp_sensor);
    if ( after_zone )
    {
        switch ( getReg8(AFTER_ZONE_SETTING ))
        {
           default:
               case 0:
                PID_SetControllerDirection(&TPID3,_PID_CD_DIRECT );
                   break;
              case 1:
                 PID_SetControllerDirection(&TPID3,_PID_CD_REVERSE );
                 break;
                }
     }
     else
        PID_SetControllerDirection(&TPID3,_PID_CD_REVERSE );
     SET_POINT3 = fGetAnalogSetting();
     PID_Compute(&TPID3,input_data);
     return (PIDOut3);
}

void vFMCH_FSM( FMCH_Device_t * dev)
{
    u8 c_type  = getReg8( CONTROL_TYPE );
    if (c_type == MKV_MB_DIN ) setReg8(LIGTH, ucDinGet(INPUT_2));  //Проверияем состояние сигнала включения света
    user_dout_set(OUT_2, getReg8(LIGTH));                                //Закидываем его на реле света
    USER_SETTING_CHECK(c_type,  dev);


    if ( error_state & (LOW_VOLTAGE_ERROR | HIGH_VOLTAGE_ERROR))
        task_fsm = USER_PROCESS_ALARM; //Если ошибка напряжения сразу же накидываем аварийный режим

    // Если засоренность фильта больше значения устваки, то выставляем предупрежние и делаем запись в журнал
     USER_FilterState(dev);
     if  ((dev->FilterState >=FILTER_WARNINR_VALUE) && ( dev->HEPA_CONTROL_FLAG))
     {
          if ((error_state & FILTER_ERROR) == 0)
          {
               if (++ (dev->Filter_Warning_Timeout) >= 18000)
               {
                   vADDRecord(FILTER_ERROR);
                   error_state |=FILTER_ERROR;
               }
         }
      }
      else
      {
          error_state &=~FILTER_ERROR;
          dev->Filter_Warning_Timeout = 0;
      }
        //Свет
      if (MB_TASK_GetMode() && (task_fsm != USER_PROCCES_IDLE))
      {
           task_fsm = USER_PROCCES_IDLE;
           HAL_SetBit(CRACH_Port,  CRACH_Pin);
           user_dout_set(OUT_3,FALSE);
      }
      if ((task_fsm != USER_PROCESS_ALARM) && (task_fsm != USER_PROCCES_IDLE) && ucDinGet(INPUT_4))
      {
          if ((error_state &  PRE_FILTER_ERROR) == 0)
          {
              if (++ (dev->PreFilter_Warning_Timeout) >= 18000)
              {
                   vADDRecord( PRE_FILTER_ERROR);
                   error_state |= PRE_FILTER_ERROR;
              }
          }
      }
      else
      {
          error_state &= ~PRE_FILTER_ERROR;
          dev->PreFilter_Warning_Timeout = 0;
     }

      switch (task_fsm)
      {
          case USER_PROCCES_IDLE:
              dev->HEPA_CONTROL_FLAG= 0;
              user_dout_set(OUT_1,FALSE);
              PIDOut = 0;
              USER_AOUT_SET(DAC1,0);
              USER_AOUT_SET(DAC2,0);
              USER_AOUT_SET(DAC3,0);
              dev->start_timeout = 0;
              temp_counter = 0;
              error_state = 0;
              break;
         case USER_PROCESS_WORK_TIME_OUT:   //Запускаем таймер остановки вентилятора
              dev->HEPA_CONTROL_FLAG= 0;    //И запускаем автокалиборвку датчиков
              if ( (++dev->start_timeout)> ( getReg8(FAN_START_TIMEOUT)*100))
              {
                  task_fsm = USER_PROCESS_ZERO_CALIB;
                  CalibrateZeroStart();
              }
              break;
         case USER_PROCESS_ZERO_CALIB:
              if (CalibrationZeroWhait())   //Проверяем закончилась ли калиборвка
              {
                     PIDOut = 0;
                     PID_SetTunings2(&TPID,getRegFloat(COOF_P), getRegFloat(COOF_I), 0);
                     PID_Init(&TPID,0,0);
                     task_fsm = USER_PROCCES_WORK;
              }
              break;
        case USER_PROCCES_WORK:
              if (++dev->pid_counter >=10)
              {
                  dev->pid_counter = 0;
                  Temp = getAIN(SENS1);
                  PID_Compute(&TPID,Temp);
                  float PID_Out = PIDOut/1000.0;
                  USER_AOUT_SET(DAC2,PID_Out);
                  if ( ((PID_Out) >=9.5) && (Temp < SET_POINT ) )
                  {
                      if  ((error_state & SETTING_ERROR )==0 )
                      {
                          if ( ++(dev->Setting_Warning_Timeout) >= 1800 )
                          {
                              vADDRecord(SETTING_ERROR);
                              error_state |= SETTING_ERROR;
                          }
                      }
                  }
                  else
                  {
                      dev->Setting_Warning_Timeout = 0;
                      error_state &= ~SETTING_ERROR;
                  }
                  dev->HEPA_CONTROL_FLAG = (fabs(SET_POINT-Temp) <= ( SET_POINT*0.02) ) ? 1 : 0 ;
               }
               user_dout_set(OUT_1,TRUE);
               break;
         case USER_PROCESS_ALARM:
               if ( ( error_state & (LOW_VOLTAGE_ERROR | HIGH_VOLTAGE_ERROR)) == 0 )  task_fsm = USER_PROCCES_IDLE;
               PIDOut = 0;
               USER_AOUT_SET(DAC2,0);
               user_dout_set(OUT_1,FALSE);
               error_state &= ~SETTING_ERROR;
               dev->HEPA_CONTROL_FLAG = 0;
               dev->start_timeout = 0;
               break;
     }
}

void SystemCalibraionStart()
{
   __switch_to_calibration_state();
}
void SystemCalibraionStop()
{
   task_fsm = USER_PROCCES_WORK;
}

void vCDV_SetpointCheck(   u32 * timeout  )
{
    DISCRET_STATE_t  current_state = getReg8(CDV_CONTOROL);
    if ( getReg8( CONTROL_TYPE )==  MKV_MB_DIN)
    {
        if (system_start != MKV_MB_DIN)
        {
            setReg8(CDV_CONTOROL,SETTING_CLOSE);
            system_start =  MKV_MB_DIN;
            din_state_update = 0;
            cur_state = 0;
        }
        u8 new_state =( (u8)uiGetDinMask() & 0x0F ) ; //Проверяем режим работы
        if (( new_state!= cur_state ) && (din_state_update ==0))
        {
            din_state_update = 1;
            *timeout = 0;
        }
        if (din_state_update)
        {
            printf("%i  %i\r\n",*timeout,getReg8(SETTING_TIMER)*100 );
            if ( ++(*timeout) >= getReg8(SETTING_TIMER)*100 )
            {
                error_state &=~DIN_ERROR;
                din_state_update = 0;
                cur_state = new_state;
                switch (cur_state )
                {
                    case 0:
                        setReg8(CDV_CONTOROL,SETTING_CLOSE);
                        break;
                    case 1:
                        setReg8(CDV_CONTOROL,SETTING_MINIMUM);
                        break;
                    case 3:
                        setReg8(CDV_CONTOROL,SETTING_MIDIUM);
                        break;
                    case 7:
                        setReg8(CDV_CONTOROL,SETTING_MAXIMUN);
                        break;
                    case 0xF:
                        setReg8(CDV_CONTOROL,SETTING_OPEN);
                        break;
                    default:
                        error_state |=DIN_ERROR;
                        setReg8(CDV_CONTOROL,SETTING_CLOSE);
                        return;
                        break;
                }
            }
        }
    }
    else
    {
        system_start =  MKV_MB_RTU;
        error_state &=~DIN_ERROR; //Сбрасываем ошибку дискретных входов
        setReg8(CDV_CONTOROL,getReg8(MB_CDV_CONTROL));

    }
  //  if (++start_clear_timer >100)
   //        {
    CleanTimerFuncton((current_state !=getReg8(CDV_CONTOROL ) || getReg8(CDV_CONTOROL )==SETTING_CLOSE) );
  //  start_clear_timer = 100;
 //            }
    return ;
}

u8 getStateDCV()
{
    return getReg8(CDV_CONTOROL);
}

u8 getStateVAV()
{
    return getReg8(CDV_CONTOROL);
}

void vBP()
{

}

static float CumputeChannel2Setpoit( float setpoint)
{
    float temp_f = DataModelGetCDVSettings(setpoint, CAV_VAV_CH1);
    switch (getReg8(MEASERING_UNIT))
    {
        case 0:
            temp_f = DataModel_SetLToPressere(temp_f,CAV_VAV_CH2);
            break;
       case 1:
            temp_f = DataModel_SetVToPressere(temp_f,CAV_VAV_CH2);
            break;
       case 2:
            break;
    }
    return (temp_f );
}

void Channel2Reg(  float setpoint )
{
    static float PID_Out;
    static float temp_f;
    DISCRET_STATE_t state = getReg8(CDV_CONTOROL);
    u8 ch_count = getReg8(CDV_BP_CH_COUNT);

    if (ch_count  == 2)
    {
         if (state == SETTING_CLOSE) PID_Out = 0.0;
         else if (state== SETTING_OPEN ) PID_Out = 10.0;
         else
         {
            if ((getReg8(MEASERING_UNIT))!=2 )
            {
                temp_f = DataModelGetCDVSettings(setpoint, CAV_VAV_CH1);
                temp_f = temp_f + temp_f * ( getRegFloat(OFFSET_CH2)/100.0);
                SET_POINT1  = DataModelGetPressureSettings( temp_f ,  CAV_VAV_CH2) ;
            }
            else
            {
                SET_POINT1 = setpoint + getRegFloat(OFFSET_CH2_PA);
            }//temp_f + getRegFloat(OFFSET_CH2);
            PID_Compute(&TPID2,getAIN(SENS2));
            PID_Out = PIDOut2/1000.0;
         }

        USER_AOUT_SET(DAC2,PID_Out);
    }
    else if (ch_count  == 0) vBP();
    else
        USER_AOUT_SET(DAC2,0);
}

float fGetAnalogSetting()
{
    return getRegFloat(SettingRegMap[getReg8(PRIOR_SENSOR)]);
}

float vAnalogSensorFSM( )
{
    float PID_Out;
    float temp_float = getAIN(SENS1);
    if (temp_float <= getRegFloat(SETTING_MIN))
    {
          PID_SetOutputLimits(&TPID, USER_AOUT_GET(DAC1),TPID.OutMax);
    }
    if (temp_float >= getReg16(SETTING_MAX))
    {
         PID_SetOutputLimits(&TPID, TPID.OutMin, USER_AOUT_GET(DAC1));
    }
    PID_Out = getRegFloat(SettingRegMap[getReg8(PRIOR_SENSOR)]);
    return (PID_Out);
}

DISCRET_STATE_t getCurSettingState()
{
    return temp_control_state;
}

float getSETPOINT()
{
    return (SET_POINT);
}

void vCDV_FSM(u8 * cal_flag, FMCH_Device_t * dev)
{
    if (task_fsm != USER_PROCESS_ZERO_CALIB)
    {
        vCDV_SetpointCheck( &dev->start_timeout);
        ErrorSensorCheck(&error_state);
        vCheckDoubleChannelAlarm(&error_state);
    }
    switch (task_fsm)
    {
        case USER_PROCCES_IDLE:
            error_state = 0;
            dev->start_timeout = 0;
            InitCleanTimer();
            task_fsm = USER_PROCCES_WORK;
            break;
        case USER_PROCCES_WORK:
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
                if (++dev->pid_counter >=10)
                {
                    INPUT_SENSOR_t  temp_inp_sens_type = getReg8(INPUT_CONTROL_TYPE);
                    temp_control_state = getReg8(CDV_CONTOROL);
                    float pid_input = UPDATE_PRES_COOFCAV();   //)(temp_inp_sens_type , temp_control_state);
                    dev->pid_counter = 0;
                    u8 compute_falg = 1;
                    if (getReg8(CLEAR_TIMER_STATE) == 1 )  //Если включился таймер уборки и состоние системы не выкл.
                    {
                        temp_control_state = SETTING_MAXIMUN;  //То переводим системы в состния максимальной уставки
                    }
                    switch (temp_control_state)
                    {
                        case SETTING_OPEN:
                            USER_AOUT_SET(DAC1,10.0);
                            compute_falg = 0;
                            break;
                        case SETTING_MINIMUM:
                            SET_POINT = getRegFloat(SETTING_MIN);
                            break;
                        case SETTING_MAXIMUN:
                            SET_POINT = getRegFloat(SETTING_MAX);
                            break;
                        default:
                        case SETTING_CLOSE:
                            USER_AOUT_SET(DAC1,0.0);
                            compute_falg = 0;
                            break;
                        case SETTING_MIDIUM:
                            switch (temp_inp_sens_type)
                            {
                                case DISCRETE_INPUT:
                                    SET_POINT =getRegFloat(SETTING_MID);
                                    break;
                                case ROOM_CONTROLLER:
                                    SET_POINT = ComputeSetPoint();
                                    break;
                                case ANALOG_SENSOR:
                                case STATIC_TERMSENSOR:
                                    SET_POINT = UPDATE_COOFCAV (temp_inp_sens_type , temp_control_state);
                                    break;
                            }
                    }
                    if (compute_falg)
                    {
                        PID_Compute(&TPID,pid_input);
                        float PID_Out = PIDOut/1000.0;
                        USER_AOUT_SET(DAC1,PID_Out);
                    }
                    Channel2Reg(GetChanne2Setting());
                }
            }
            if ( ++zero_calibration_timer >= ( getReg8(AUTO_CALIB_TIMER)*3600*100))
            {              
                __switch_to_calibration_state();      
            }
            break;
        case USER_PROCESS_ZERO_CALIB:                
            if (start_clear_timer >= (getReg16(ZERO_POINT_TIMEOUT) * 100) )
            {
                if (*cal_flag == 0)
                {
                    CalibrateZeroStart();
                    *cal_flag = 1;
                }
                else
                {
                    if (CalibrationZeroWhait())
                    {
                        *cal_flag = 0;                                                   
                        user_dout_set(OUT_2, 0);
                        task_fsm = USER_PROCCES_WORK;
                        //xTaskNotifyIndexed(*(getLCDTaskHandle()), 0, LCD_REINIT, eSetValueWithOverwrite);
                    }
                }
            }
            else
            {
                start_clear_timer++;
                user_dout_set(OUT_2, 1);
                //xTaskNotifyIndexed(*(getLCDTaskHandle()), 0, LCD_REINIT, eSetValueWithOverwrite);
            }
            break;
        case USER_PROCESS_ALARM:
            if ((error_state & (DIN_ERROR | ANALOG_SENSOR_ERROR)) == 0)
            {
                task_fsm = USER_PROCCES_IDLE;
            }
            else
            {
                USER_AOUT_SET(DAC1,0.0);
                USER_AOUT_SET(DAC2,0.0);
            }
            break;
        case USER_PROCESS_DOUBLE_CHANNEL_ERROR:
            printf("dcherr\r\n");
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

static void __switch_to_calibration_state()
{
    task_fsm = USER_PROCESS_ZERO_CALIB;                    
    start_clear_timer = 0;
    zero_calibration_timer = 0;
}

void VoltageControlCheck( AC_VOLTAGE_CONTROL_t * ac_control)
{
    if (ac_control->power_on_flag)
    {
        if (ac_control->Voltage >= (uint16_t)getReg8(HIGH_VOLTAGE_ON))
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
        if (ac_control->Voltage < 20)
        {
            if  (++ac_control->power_off_timeout> 2)
            {
                HAL_ResetBit(LDCDATA_2_3_E_REW_CD_LED_Port,  LCDLED_Pin);
                SaveBeforePowerOff();
                ac_control->power_on_flag = 0;
                ac_control->power_off_flag = 1;
                task_fsm = USER_PROCCES_IDLE;
            }
        }
        else 
        {
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
   FMCH_Device_t  Dev= {2,0,0,0,0,.pid_counter = 0,.start_timeout = 0, . hepa_counter = 0, .FilterState = 0};
   static AC_VOLTAGE_CONTROL_t ac_contorl = {0,0,0,0,0,0 };
   static u8 flag = 0;
   u8 process_mode = getReg8(DEVICE_TYPE);
   error_state = 0;
   task_fsm = USER_PROCCES_IDLE;
   PID(&TPID,  &PIDOut, &SET_POINT, getRegFloat(COOF_P), getRegFloat(COOF_I), 0, _PID_CD_DIRECT);
   if ( process_mode == DEV_FMCH )
       PID_SetOutputLimits(&TPID,(float)1000.0,(float)10000.0);
   else
       PID_SetOutputLimits(&TPID,(float)0.0,(float)10000.0);
   PID(&TPID2, &PIDOut2, &SET_POINT1, getRegFloat(COOF_P1), getRegFloat(COOF_I1), 0, _PID_CD_DIRECT);
   PID_SetOutputLimits(&TPID2,(float)0000.0,(float)10000.0);
   PID(&TPID3,  &PIDOut3, &SET_POINT3, getRegFloat(COOF_P), getRegFloat(COOF_I), 0, _PID_CD_DIRECT);
   PID_SetOutputLimits(&TPID3,(float)0000.0,(float)10000.0);
   while(1)
   {
        vTaskDelay(10);
        if (MB_TASK_GetMode()!=2)
        {
            ac_contorl.Voltage = (uint16_t)getAIN(AC220);
            VoltageControlCheck(&ac_contorl);
            if (process_mode == DEV_FMCH )
            {
                vFMCH_FSM(  &Dev );
            }
            else 
            {
                vCDV_FSM(&flag, &Dev  );
            }
            //Ecли есть ошибка включаем реле и зажигаем светодиод
            if ( error_state )
            {
                HAL_ResetBit(CRACH_Port,  CRACH_Pin);
                user_dout_set(OUT_3,TRUE);
            }
            else
            {
                HAL_SetBit(CRACH_Port,  CRACH_Pin);
                user_dout_set(OUT_3,FALSE);
            }
        }
    }
}

/// Функция установки состония дискрентого выхода и перезапуска индикатора, если состоние поменялось
void user_dout_set(OUT_NAME_TYPE ucCh, uint8_t state)
{
    if (eGetDOUT(ucCh) != state)
    {
        xTaskNotifyIndexed(*(getLCDTaskHandle()), 0, LCD_REINIT, eSetValueWithOverwrite);
    }
    eSetDUT(ucCh,state);
}