/*
 * mb_task.c
 *
 *  Created on: Aug 4, 2024
 *      Author: i.dymov
 */

#include "mb_task.h"
#include "hw_lib_din.h"
#include "mb.h"
#include "hal_timers.h"
#include "ADC.H"
#include "eth_driver.h"
#include "wchnet.h"
#include "main.h"
#include "string.h"
#include "task.h"
#include "event_groups.h"
#include "hw_lib_adc.h"
#include "hal_usart.h"
#include "data_model.h"
#include "user_process.h"
#include "system_types.h"
#include "user_process_service.h"


static UCHAR  ucSCoilBuf[REG_COILS_NREGS/8+1];
static UCHAR  ucSDiscInBuf[REG_DISCRETE_NREGS/8+1];


#define REG_INPUT_START   0x01
#define REG_HOLDING_START 0x01
#define REG_HOLDING_START 0x01

#define REG_HOLDING_NREGS 160
#define REG_INPUTS_NREGS  120

static USHORT usRegInputStart = REG_INPUT_START;
static USHORT usRegHoldingStart = REG_HOLDING_START;


static USHORT usRegHoldingBuf[REG_HOLDING_NREGS];
static USHORT usRegInputBuf[REG_INPUTS_NREGS];

#define MODE_MB          0
#define CONTROL_TYPE_MB  1
#define PROTOCOL_TYPE_MB 2
#define TIME_H_MB        3
#define TIME_M_MB        4
#define TIME_S_MB        5
#define DATE_D_MB        6
#define DATE_M_MB        7
#define DATE_Y_MB        8
#define V_MIN_ON         9
#define V_MIN_OFF        10
#define V_MAX_ON         11
#define V_MAX_OFF        12
#define CONTR_MB         13
#define MB_ADDRES_MB     14
#define TIME_OUT_MB      15
#define IP_1_MB          16
#define IP_2_MB          17
#define IP_3_MB          18
#define IP_4_MB          19
#define GATE_1_MB        20
#define GATE_2_MB        21
#define GATE_3_MB        22
#define GATE_4_MB        23
#define MASK_1_MB        24
#define MASK_2_MB        25
#define MASK_3_MB        26
#define MASK_4_MB        27
#define IP_PORT_MB       28
#define TIME_SENS_MB     29
#define ZERO_MB          30
#define AOUT1_C_MB       31
#define AOUT2_C_MB       33
#define AOUT3_C_MB       35
#define AIN1_TYPE_MB     37
#define AIN2_TYPE_MB     38
#define AIN3_TYPE_MB     39

#define COMMON_REG_COUNT (AIN3_TYPE_MB+3)

#define INP_MH_H_MB      26
#define INP_MH_M_MB      27
#define SENSOR_ERROR_MB  28
#define PID_OUT_MB       29

#define COMMON_REG_INPUT_NREGS ( PID_OUT_MB+2)



#define DEVICE_SPECIFIC_ADDRES 100

//§²§Ö§Ô§Ú§ã§ä§â§í FMCH
#define FMCH_OFFSET       0

#define KOOF_P_MB         100
#define KOOF_I_MB         102
#define KOOF_K_MP         104
#define FILTER_LOW_MB     106
#define FILTER_HIGH_MB    107
#define SET_MOD1_MB       108
#define SET_MOD2_MB       109
#define MODE_REG_MB       110
#define TIME_FAN_STOP_MB  111
#define COMMAND_REG       112
#define LIGTH_REG_MB      113
#define JOURNAL_SELECT_MB 114

#define FMCH_COUNT         ( JOURNAL_SELECT_MB - KOOF_P_MB + 1 )

#define FACT_RASH_MB            100
#define PROCESS_STATE          ( FACT_RASH_MB               + 1 )
#define ERROR_STATE_MB         ( PROCESS_STATE              + 1 )
#define JOURNAL_ERROR_COUNT_MB ( ERROR_STATE_MB             + 1 )
#define JOURNAL_CUR_DATE_MB    ( JOURNAL_ERROR_COUNT_MB     + 1 )
#define JOURNAL_CUR_MOUNTH_MB  ( JOURNAL_CUR_DATE_MB        + 1 )
#define JOURNAL_CUR_YEAR_MB    ( JOURNAL_CUR_MOUNTH_MB      + 1 )
#define JOURNAL_CUR_HOUR_MB    ( JOURNAL_CUR_YEAR_MB        + 1 )
#define JOURNAL_CUR_MIN_MB     ( JOURNAL_CUR_HOUR_MB        + 1 )
#define JOURNAL_CUR_SEC_MB     ( JOURNAL_CUR_MIN_MB         + 1 )
#define JOURNAL_CUR_E_CODE_MB  ( JOURNAL_CUR_SEC_MB         + 1 )
#define FILTER_STATE_MB        ( JOURNAL_CUR_E_CODE_MB      + 1 )


#define FMCH_INPUTS_COUNT      ( FILTER_STATE_MB - FACT_RASH_MB   + 1 )


#define CDV_OFFSET          100

//§²§Ö§Ô§Ú§ã§ä§â§í CDV
#define CDV_CH_COUNT_MB         200
#define CDV_KOOF_K_MP           201
#define CDV_KOOF_I_MB           205
#define CDV_KOOF_P_MB           203
#define CDV_KOOF_P1_MB          207
#define CDV_KOOF_I1_MB          209
#define CDV_KOOF_PSESN_MB       211
#define CDV_KOOF_ISENS_MB       213
#define CDV_INPUT_SENS_MB       215
#define CDV_SETTING_MIN_MB      216
#define CDV_SETTING_MID_MB      218
#define CDV_SETTING_MAX_MB      220
#define CDV_SETTING_ERROR1_MB   222
#define CDV_SETTING_ERROR2_MB   224
#define CDV_F_CHANNEL           226
#define CDV_OFFSET_CH2          228
#define CDV_CLEAN_TIMER         230
#define CDV_MEASERING_UNIT      231
#define CDV_ZERO_POINT_TIMEOUT  232
#define CDV_SETTING_TIMEOUT_MB  233
#define CDV_MODE_CONTROL        234
#define CDV_AFZONE_SETTING_MB   235
#define CDV_SENSOR1_MIN         236
#define CDV_SENSOR1_MAX         238
#define CDV_SENSOR2_MIN         240
#define CDV_SENSOR2_MAX         242
#define CDV_SENSOR3_MIN         244
#define CDV_SENSOR3_MAX         246
#define CDV_SENSOR1_OFS         248
#define CDV_SENSOR2_OFS         250
#define CDV_SENSOR3_OFS         252
#define CDV_SENSOR1_SET         254
#define CDV_SENSOR2_SET         256
#define CDV_SENSOR3_SET         258
#define CDV_ROOM_CHANNEL        260
#define CDV_PRIOR_SENS          261
#define CDV_KOOF_K_2            262
#define CDV_F_CHANNEL2          264
#define AUTO_CALIB_TIMER_MB     266
//#define CLEAR_TIMER_ON


#define CDV_COUNT              ( CDV_CH_COUNT_MB  - AUTO_CALIB_TIMER_MB  + 1)

#define CDV_FACT_1              200
#define CDV_FACT_2              202
#define CDV_T_SENSOR_MB         204
#define CDV_CO2_SENSOR_MB       206
#define CDV_H_SENSOR_MB         207
#define CDV_CUR_STATE           208
#define SETTING_FACT_CH1_MB     209
#define SETTING_FACT_CH2_MB     211

#define CDV_INPUTS_COUNT       ( SETTING_FACT_CH2_MB - CDV_FACT_1 + 2 )



//#define SENS_COOF  18
//#define DAC_DATA   19
#define SENS_PERIOD 58
#define SENS_COUNT  59


static u16 const device_specific_input_count[] =  { FMCH_INPUTS_COUNT  ,CDV_INPUTS_COUNT   };
static u16 const device_specific_reg_offset[]  =  { FMCH_OFFSET + 100 ,CDV_OFFSET + 100};
static u16 const device_specific_reg_count[]   =  { FMCH_COUNT  ,CDV_COUNT  };
static u16 const SensorPRegMap[]={COOF_PT,COOF_PCO2,COOF_PH};
static u16 const SensorIRegMap[]={COOF_IT,COOF_ICO2,COOF_IH};
static u8 first_start = 1;
static u8 WORK_MODE =0;


uint16_t ConvertData( uint16_t input)
{
   return ( input/(float)(1000.0/984.0));
}

void  setWorkMode(  )
{
    if (getReg8(TEST_MODE)) WORK_MODE = 2;
    else {
        WORK_MODE = 0;
    }
}

u8 MB_TASK_GetMode()
{
    return (WORK_MODE);
}



static const u16 COMMON_REGS_MAP[] ={
                                        0,                      //0
                                        CONTROL_TYPE,           //1
                                        MB_PROTOCOL_TYPE,       //2
                                        0,                      //3
                                        0,                      //4
                                        0,                      //5
                                        0,                      //6
                                        0,                      //7
                                        0,                      //8
                                        LOW_VOLTAGE_ON,         //9
                                        LOW_VOLTAGE_OFF,        //10
                                        HIGH_VOLTAGE_ON,        //11
                                        HIGH_VOLTAGE_OFF,       //12
                                        CONTRAST,               //13
                                        MB_RTU_ADDR,            //14
                                        MOD_BUS_TIMEOUT,        //15
                                        IP_1,                   //16
                                        IP_2,                   //17
                                        IP_3,                   //18
                                        IP_4,                   //19
                                        GATE_1,                 //20
                                        GATE_2,                 //21
                                        GATE_3,                 //22
                                        GATE_4,                 //23
                                        MASK_1,                 //24
                                        MASK_2,                 //25
                                        MASK_3,                      //26
                                        MASK_4,                      //27
                                        IP_PORT,                //28
                                        SENSOR_COUNT,           //29
                                        0,                      //30
                                        DAC1,                      //31
                                        DAC1,                      //32
                                        DAC2,                      //33
                                        DAC2,                      //34
                                        DAC3,                      //35
                                        DAC3,                      //36
                                        AIN1_TYPE,              //37
                                        AIN2_TYPE,              //38
                                        AIN3_TYPE               //39
};

static const u16 FMCH_REGS_MAP[] ={
                                        COOF_P,                      //0
                                        COOF_P,           //1
                                        COOF_I,       //2
                                        COOF_I,                      //3
                                        KOOFKPS,                      //4
                                        KOOFKPS,                      //5
                                        FILTER_LOW,                //6
                                        FILTER_HIGH,               //7
                                        SETTING1,                      //8
                                        SETTING2,                      //9
                                        MODE,        //10
                                        FAN_START_TIMEOUT,        //11
                                        SYSTEM_START,       //12
                                        LIGTH,               //13
                                        0,            //14

};


static const u16 CDV_REGS_MAP[] = {
                                               CDV_BP_CH_COUNT,         //0
                                               KOOFKPS1,                //1
                                               KOOFKPS1,                //2
                                               COOF_P_CAV,              //3
                                               COOF_P_CAV,              //4
                                               COOF_I_CAV,              //5
                                               COOF_I_CAV,              //6
                                               COOF_P1,                 //7
                                               COOF_P1,                 //8
                                               COOF_I1,                 //9
                                               COOF_I1,                 //10
                                               0,                       //11
                                               0,                       //12
                                               0,                       //13
                                               0,                       //14
                                               INPUT_CONTROL_TYPE,      //15
                                               SETTING_MIN,             //16
                                               SETTING_MIN,             //17
                                               SETTING_MID,             //18
                                               SETTING_MID,             //19
                                               SETTING_MAX,             //20
                                               SETTING_MAX,             //21
                                               CH1_SETTING,             //22
                                               CH1_SETTING,             //23
                                               CH2_SETTING,             //24
                                               CH2_SETTING,             //25
                                               F_CHANNEL,               //26
                                               F_CHANNEL,               //27
                                               OFFSET_CH2,              //28
                                               OFFSET_CH2,              //29
                                               CLEAN_TIMER,             //30
                                               MEASERING_UNIT,          //31
                                               ZERO_POINT_TIMEOUT,      //32
                                               SETTING_TIMER,           //33
                                               MB_CDV_CONTROL,          //34
                                               AFTER_ZONE_SETTING,      //35
                                               MIN_SET1,                //36
                                               MIN_SET1,                //37
                                               MAX_SET1,                //38
                                               MAX_SET1,                //39
                                               MIN_SET2,          //40
                                               MIN_SET2,          //41
                                               MAX_SET2,          //42
                                               MAX_SET2,          //43
                                               MIN_SET3,          //44
                                               MIN_SET3,          //45
                                               MAX_SET3,          //46
                                               MAX_SET3,          //47
                                               SENS_OFS1,         //48
                                               SENS_OFS1,         //49
                                               SENS_OFS2,         //50
                                               SENS_OFS2,         //51
                                               SENS_OFS3,         //52
                                               SENS_OFS3,         //53
                                               SENS_SETTING1,     //54
                                               SENS_SETTING1,     //55
                                               SENS_SETTING2,     //56
                                               SENS_SETTING2,     //57
                                               SENS_SETTING3,     //58
                                               SENS_SETTING3,     //59
                                               ROOM_CHANNEL,      //60
                                               PRIOR_SENSOR,      //61
                                               KOOFKPS2,          //62
                                               KOOFKPS2,          //63
                                               F_CHANNEL2,        //64
                                               F_CHANNEL2,        //65
                                               AUTO_CALIB_TIMER,
};



void vSetRegData( u16 adress)
{
   float data;
   u8 dev_type = getReg8(DEVICE_TYPE);
   u16 byte_data;
   u16 reg_addr;
   if (adress < DEVICE_SPECIFIC_ADDRES)
   {
       reg_addr  = COMMON_REGS_MAP[ adress];
       byte_data = (u16)usRegHoldingBuf[adress];
       u8 date_time_flag = 0;
       switch (adress)
       {
             case ZERO_MB:
                 if  (byte_data != 0)
                 {
                     if  ((USER_GetProccesState() == USER_PROCCES_WORK) && ( dev_type==DEV_CAV_VAV_BP))
                     {
                         SystemCalibraionStart();
                     }
                     else

                     if ((USER_GetProccesState() == USER_PROCCES_IDLE) && ( dev_type==DEV_FMCH))
                    {

                         CalibrateZeroStart();
                    }
                     else
                     {
                         usRegHoldingBuf[adress] = 0;
                     }
                 }
                 break;
             case MODE_MB:
                 if (byte_data  > 3)
                     {
                       if ((WORK_MODE) && (byte_data == 0x7401))
                       {
                           SaveReg8(DEVICE_TYPE,0);
                           NVIC_SystemReset();
                       }
                       if ((WORK_MODE) && (byte_data == 0x7402))
                       {
                            SaveReg8(DEVICE_TYPE,1);
                            NVIC_SystemReset();
                        }
                        if (byte_data == 0x55) vDataModelResetJournal();
                        else
                        if (byte_data == 0xAA) ResetMotorHour();
                         usRegHoldingBuf[adress] = WORK_MODE;

                     }
                 else
                 {
                     WORK_MODE = byte_data ;
                     if (WORK_MODE ==3 )
                     {
                         SaveBeforePowerOff();
                         vTaskDelay(100);
                         NVIC_SystemReset();
                     }
                     if ((WORK_MODE == 0 ) && (getReg8(TEST_MODE) == 1 )) SaveReg8(TEST_MODE,0);
                     else
                     if ((WORK_MODE == 2 ) && (getReg8(TEST_MODE) == 0 )) SaveReg8(TEST_MODE,1);
                 }
                 break;
             case AOUT1_C_MB+1:
             case AOUT2_C_MB+1:
             case AOUT3_C_MB+1:
             if (WORK_MODE ==2)
                {
                    USER_AOUT_SET(reg_addr,convert_int_to_float( &usRegHoldingBuf[adress-1]));
                }
                break;
             case SENS_COUNT:
                 vSetCount(byte_data);
                 break;
             case V_MIN_ON:
                 if (byte_data <usRegHoldingBuf[V_MIN_OFF] ) SaveReg8(reg_addr,byte_data);
                 break;
             case V_MIN_OFF:
                 if ((byte_data > usRegHoldingBuf[V_MIN_ON] ) && (byte_data < usRegHoldingBuf[V_MAX_OFF] ) )  SaveReg8(reg_addr,byte_data);
                  break;
             case V_MAX_OFF:
                 if ((byte_data > usRegHoldingBuf[V_MIN_OFF] ) && (byte_data < usRegHoldingBuf[V_MAX_ON] ) )  SaveReg8(reg_addr,byte_data);
                 break;
             case V_MAX_ON:
                 if ( byte_data > usRegHoldingBuf[V_MAX_OFF] ) SaveReg8(reg_addr,byte_data);
                 break;
             case MB_ADDRES_MB:
             case CONTROL_TYPE_MB:
             case PROTOCOL_TYPE_MB:
             case IP_1_MB:
             case IP_2_MB:
             case IP_3_MB:
             case IP_4_MB:
             case GATE_1_MB:
             case GATE_2_MB:
             case GATE_3_MB:
             case GATE_4_MB:
             case MASK_1_MB:
             case MASK_2_MB:
             case MASK_3_MB:
             case MASK_4_MB:
                 if (WORK_MODE)
                 {
                      SaveReg8(reg_addr , byte_data);
                 }
                 break;
             case TIME_H_MB:
                 if (byte_data <24) date_time_flag = 1;
                 break;
             case TIME_M_MB:
             case TIME_S_MB:
                 if (byte_data <60) date_time_flag = 1;
                 break;
             case DATE_D_MB:
                 if ((byte_data <32) && (byte_data>0)) date_time_flag = 2;
                 break;
             case DATE_M_MB:
                 if ((byte_data <13) && (byte_data>0)) date_time_flag = 2;
                 break;
             case DATE_Y_MB:
                 if (byte_data <99) date_time_flag = 2;
                 break;
             case IP_PORT_MB:
                 if (WORK_MODE)
                 {
                     saveReg16(reg_addr, byte_data);
                 }
                 break;
             case CONTR_MB:
             case TIME_SENS_MB:
             case TIME_OUT_MB:
             case AIN1_TYPE_MB:
             case AIN2_TYPE_MB:
             case AIN3_TYPE_MB:
                 SaveReg8(reg_addr,byte_data);
                 break;
         }
          if (date_time_flag == 1 )
          {
              HAL_TimeConfig_T time;
              time.hours   = usRegHoldingBuf[TIME_H_MB];
              time.minutes = usRegHoldingBuf[TIME_M_MB];
              time.seconds = usRegHoldingBuf[TIME_S_MB];
              HAL_RTC_ConfigTime(&time);
          }
          else
          if (date_time_flag == 2 )
          {
              HAL_DateConfig_T date;
              date.date  = usRegHoldingBuf[DATE_D_MB];
              date.month = usRegHoldingBuf[DATE_M_MB];
              date.year =  usRegHoldingBuf[DATE_Y_MB];
              HAL_RTC_ConfigDate(&date);
          }
   }
   else
   {
       if (dev_type == DEV_FMCH)
       {

               byte_data = (u16)usRegHoldingBuf[adress];
               reg_addr = FMCH_REGS_MAP[adress- 100];
               switch (adress)
               {
                    case LIGTH_REG_MB:
                    case MODE_REG_MB:
                         VerifyAndSetReg8(reg_addr, (uint8_t) byte_data );
                         break;
                    case COMMAND_REG:
                         USER_SetControlState( (uint8_t) byte_data );
                         break;
                    case (KOOF_I_MB+1):
                    case (KOOF_K_MP+1):
                    case (KOOF_P_MB+1):
                         saveRegFloat(reg_addr, convert_int_to_float( &usRegHoldingBuf[adress-1]));
                         break;
                    case  SET_MOD1_MB:
                    case  SET_MOD2_MB:
                    case  FILTER_LOW_MB:
                    case  FILTER_HIGH_MB:
                          saveReg16(reg_addr, byte_data);
                          break;
                    case TIME_FAN_STOP_MB:
                         SaveReg8(reg_addr,byte_data);
                         break;
                    default:
                        break;
                }
       }
       else
       {
                u8 ch;
                float temp_float;
                byte_data = (u16)usRegHoldingBuf[adress-CDV_OFFSET];
                reg_addr = CDV_REGS_MAP[adress- 200];
                u16 * pFloatReg = &usRegHoldingBuf[adress-CDV_OFFSET -1];
                switch (adress)
                      {
                            case (CDV_KOOF_PSESN_MB+1):
                                 if (IsPISendScreenNreed())
                                 {
                                     reg_addr = SensorPRegMap[GetPIDSensorIndex()];
                                     saveRegFloat(reg_addr, convert_int_to_float( pFloatReg));
                                 }
                                 break;
                            case (CDV_KOOF_ISENS_MB+1):
                                     if (IsPISendScreenNreed())
                                     {
                                         reg_addr = SensorIRegMap[GetPIDSensorIndex()];
                                         saveRegFloat(reg_addr,convert_int_to_float( pFloatReg));
                                     }
                                     break;
                            case (CDV_SENSOR1_OFS+1):
                            case (CDV_SENSOR2_OFS+1):
                            case (CDV_SENSOR3_OFS+1):
                                data = convert_int_to_float( pFloatReg);
                                if (data <-9999.9) data  = -9999.9;
                                if (data > 9999.9 ) data = 9999.9;
                                saveRegFloat(reg_addr, data);
                                 break;
                            case (CDV_SENSOR1_MIN+1):
                                          data = convert_int_to_float( pFloatReg);
                                        if (data <-9999.9) data  = -9999.9;
                                        if (data > 9999.9 ) data = 9999.9;
                                        saveRegFloat(reg_addr, data);
                                        break;
                            case (CDV_SENSOR1_MAX+1):
                            case (CDV_SENSOR2_MIN+1):
                            case (CDV_SENSOR2_MAX+1):
                            case (CDV_SENSOR3_MIN+1):
                            case (CDV_SENSOR3_MAX+1):
                            case (CDV_SENSOR1_SET+1):
                            case (CDV_SENSOR2_SET+1):
                            case (CDV_SENSOR3_SET+1):
                                data = convert_int_to_float( pFloatReg);
                                if (data <0) data  = 0;
                                if (data > 9999.9 ) data = 9999.9;
                                saveRegFloat(reg_addr, data);
                                break;
                            case (CDV_KOOF_I_MB+1):
                            case (CDV_KOOF_K_MP+1):
                            case (CDV_KOOF_P_MB+1):
                            case (CDV_KOOF_I1_MB+1):
                            case (CDV_KOOF_P1_MB+1):
                            case (CDV_F_CHANNEL+1):
                            case (CDV_F_CHANNEL2+1):
                            case (CDV_KOOF_K_2+1):
                                     saveRegFloat(reg_addr, convert_int_to_float( pFloatReg));
                                     break;
                            case (CDV_OFFSET_CH2+1):

                                    if ((getReg8(MEASERING_UNIT))!=2 )
                                    {
                                         temp_float = convert_int_to_float( pFloatReg);
                                        if ( temp_float > 100.0 ) temp_float = 100.0;
                                        if ( temp_float < -100.0 ) temp_float = -100.0;
                                        saveRegFloat(reg_addr, temp_float);
                                    }
                                    else
                                    {
                                        temp_float = convert_int_to_float( pFloatReg);
                                        if ( temp_float > 2500.0 ) temp_float = 2500.0;
                                        if ( temp_float < -2500.0 ) temp_float = -2500.0;
                                        saveRegFloat(OFFSET_CH2_PA, temp_float);
                                    }
                                    break;
                            case (CDV_SETTING_MIN_MB+1 ):
                            case (CDV_SETTING_MID_MB +1 ):
                            case (CDV_SETTING_MAX_MB +1 ):
                            case (CDV_SETTING_ERROR1_MB + 1):
                            case (CDV_SETTING_ERROR2_MB + 1):
                                     ch = ((adress == CDV_OFFSET_CH2+1) || (adress == CDV_SETTING_ERROR2_MB +1))?1 : 0;
                                     data = convert_int_to_float(pFloatReg);
                                     switch ( ( MES_UNIT_t)getReg8(MEASERING_UNIT) )
                                     {
                                            case L_UNIT:
                                                data = DataModel_SetLToPressere(data,ch);
                                                break;
                                            case V_UNIT:
                                                data = DataModel_SetVToPressere(data,ch);
                                                break;
                                            case P_UNIT:
                                            default:
                                                break;
                                      }
                                      saveRegFloat( reg_addr , (data > 2500)? 2500 : data);
                                      break;
                             case CDV_MODE_CONTROL:
                                 VerifyAndSetReg8(reg_addr, (uint8_t) byte_data );
                                 break;
                            case CDV_PRIOR_SENS:
                                    SaveReg8(reg_addr,byte_data);
                                    vSetAfterZone( (byte_data == T_PRIOR) ? 1 : 0,getReg8(INPUT_CONTROL_TYPE));
                                    break;
                            case CDV_CH_COUNT_MB:
                                  if (byte_data == 24201 )
                                   SaveReg8(reg_addr,1);
                                  if (byte_data == 24202)
                                      SaveReg8(reg_addr,2);
                                  break;
                            case CDV_INPUT_SENS_MB :
                                switch (byte_data)
                                {
                                    case 24215:
                                        SaveReg8(reg_addr,0);
                                        break;
                                    case 24216:
                                        SaveReg8(reg_addr,1);
                                        break;
                                    case 24217:
                                        SaveReg8(reg_addr,2);
                                        break;
                                    case 24218:
                                        SaveReg8(reg_addr,3);
                                        break;
                                }
                                break;
                            case CDV_CLEAN_TIMER:
                            case CDV_SETTING_TIMEOUT_MB:
                            case CDV_MEASERING_UNIT:

                            case AUTO_CALIB_TIMER_MB:
                            case CDV_AFZONE_SETTING_MB:
                                   SaveReg8(reg_addr,byte_data);
                                   break;
                            case CDV_ZERO_POINT_TIMEOUT:
                                  saveReg16(reg_addr, byte_data);
                                  break;
                      }
           }
       }
}

static int iReadCurRecord= 0;

void UodateFMCHInputs()
{
    u8 temp_state;
    u16 temp_int = USER_GetFact(&temp_state);
    if (!temp_state) temp_int = 0;
    usRegInputBuf[ FACT_RASH_MB  ] = temp_int;
    usRegInputBuf[PROCESS_STATE]          = getProcessStateCode();
    usRegInputBuf[FILTER_STATE_MB]        = getReg8(RESURSE);
    usRegInputBuf[JOURNAL_ERROR_COUNT_MB] = getReg16(RECORD_COUNT); //§£§í§Ó§à§Õ §Ü§à§Ý-§Ó§à §Ù§Ñ§á§Ú§ã§Ö§Û §Ó §Ø§å§â§ß§Ñ§Ý§Ö
    u8 cur_journal_rec = usRegHoldingBuf[JOURNAL_SELECT_MB];
    if (usRegHoldingBuf[JOURNAL_SELECT_MB]==0)
                          memset(&usRegInputBuf[JOURNAL_CUR_DATE_MB],0,7*sizeof(uint16_t));  //§¦§ã§Ý§Ú §ä§Ö§Ü§å§ë§Ñ §Ó§í§Ò§â§Ñ§ß§Ñ§ñ §Ù§á§Ú§ã§î 0, §ä§à §Ù§Ñ§á§à§Ý§ß§ñ§Ö§Þ §â§Ö§Ô§Ú§ã§ä§â§í 0-§Þ§Ú
    else                                                    //§¦§ã§Ý§Ú §ß§à§Þ§Ö§â §Ù§Ñ§á§Ú§ã§Ú §Ñ§Ü§ä§å§Ñ§Ý§î§ß§Ö, §ä§à §Ó§í§Ó§à§Õ§Ú§Þ §ß§å§Ø§ß§í§Ö §Õ§Ñ§ß§ß§í§Ö §à §Ù§Ñ§á§Ú§ã§Ú
    {
        if (iReadCurRecord !=cur_journal_rec)
        { 
            static HAL_TimeConfig_T time;
            static HAL_DateConfig_T date;
            vGetRecord(cur_journal_rec -1 ,&temp_state,&time,&date);
            usRegInputBuf[JOURNAL_CUR_DATE_MB]   = date.date;
            usRegInputBuf[JOURNAL_CUR_MOUNTH_MB] = date.month;
            usRegInputBuf[JOURNAL_CUR_YEAR_MB]   = date.year;
            usRegInputBuf[JOURNAL_CUR_HOUR_MB]   = time.hours;
            usRegInputBuf[JOURNAL_CUR_MIN_MB]    = time.minutes;
            usRegInputBuf[JOURNAL_CUR_SEC_MB]    = time.seconds;
            usRegInputBuf[JOURNAL_CUR_E_CODE_MB] = temp_state;
            iReadCurRecord =cur_journal_rec;
        }
    }
}

void UpdateDCVInputs()
{
    int16_t temp_int;
    //temp_int  =  DataModelGetCDVSettings(getAIN(SENS1),CAV_VAV_CH1)*10;
    convert_float_to_int(DataModelGetCDVSettings(getAIN(SENS1),CAV_VAV_CH1), &usRegInputBuf[CDV_FACT_1-CDV_OFFSET]);
    //temp_int  =  DataModelGetCDVSettings(getAIN(SENS2),CAV_VAV_CH2)*10;
    convert_float_to_int(DataModelGetCDVSettings(getAIN(SENS2),CAV_VAV_CH2), &usRegInputBuf[CDV_FACT_2-CDV_OFFSET]);
    temp_int  =  (int16_t)(getTSensor()*10);
    convert_float_to_int((float)temp_int/10.0, &usRegInputBuf[CDV_T_SENSOR_MB-CDV_OFFSET]);
    usRegInputBuf[CDV_CO2_SENSOR_MB-CDV_OFFSET] = (uint16_t)getCO2Sensor();
    usRegInputBuf[CDV_H_SENSOR_MB-CDV_OFFSET]   = getHumanitySensor();
    usRegInputBuf[CDV_CUR_STATE  - CDV_OFFSET ] = getCurSettingState();
    float temp_float;
    DISCRET_STATE_t state    =  getCurSettingState();
    switch (state)
    {
        case SETTING_OPEN:
            temp_float = 0;
            break;
        case SETTING_CLOSE:
            temp_float = 0;
            break;
        case SETTING_MINIMUM:
            temp_float = DataModelGetCDVSettings( getRegFloat(SETTING_MIN),CAV_VAV_CH1);
            break;
        case SETTING_MAXIMUN:
            temp_float = DataModelGetCDVSettings( getRegFloat(SETTING_MAX),CAV_VAV_CH1);
             break;
        case SETTING_MIDIUM:
                               switch ((INPUT_SENSOR_t)getReg8(INPUT_CONTROL_TYPE))
                               {
                                   case DISCRETE_INPUT:
                                        temp_float = DataModelGetCDVSettings( getRegFloat(SETTING_MID),CAV_VAV_CH1);

                                        break;
                                   case ROOM_CONTROLLER:
                                        temp_float = DataModelGetCDVSettings(ComputeSetPoint(),CAV_VAV_CH1);

                                        break;
                                   case ANALOG_SENSOR:
                                       temp_float =fGetAnalogSetting();
                                        break;
                                   case STATIC_TERMSENSOR:
                                       temp_float =getRegFloat(SENS_SETTING1);
                                        break;
                                }
                                break;
    }

    convert_float_to_int( temp_float, &usRegInputBuf[SETTING_FACT_CH1_MB-CDV_OFFSET]);
    if ( state == SETTING_OPEN )temp_float = 0;
    else if ( state == SETTING_CLOSE ) temp_float = 0;
     else
     {
         temp_float = DataModelGetCDVSettings(GetChanne2Setting(),CAV_VAV_CH1);
                         if ((getReg8(MEASERING_UNIT))!=2 )
                         {
                             temp_float = temp_float + temp_float *( getRegFloat(OFFSET_CH2)/100.0);
                         }
                         else
                             temp_float = temp_float + getRegFloat(OFFSET_CH2_PA);

                     }



    convert_float_to_int( temp_float, &usRegInputBuf[SETTING_FACT_CH2_MB-CDV_OFFSET]);

}

static const AIN_CHANNEL_t AINS[]={DIG_TEMP,DIG_PRES,SENS1,DIG2_TEMP,DIG2_PRES,SENS2,DCAIN1,DCAIN2,DCAIN3,DCAIN4,DCAIN5,DC24,AC220};

// §°§Ò§ß§à§Ó§Ý§Ö§ß§Ú§Ö §ã§à§ã§à§ä§à§ñ§ß§Ú§Û INPUTS
static void MB_TASK_INPUTS_UDATE(u16 start_reg_index )
{
    if  (start_reg_index < DEVICE_SPECIFIC_ADDRES )   //§¦§ã§Ý§Ú §á§Ö§â§í§Û §Ñ§Õ§â§Ö§ã §Ó §Õ§Ú§Ñ§á§Ñ§Ù§à§ß§Ö §à§Ò§ë§Ú§ç §â§Ö§Ô§Ú§ã§ä§â§à§Ó
    {
        int32_t tempdata;
        for (u8 i =0; i<13;i++)
        {
            tempdata =(int32_t) (getAIN(AINS[i])*1000);
            convert_float_to_int((float)tempdata/1000.0, &usRegInputBuf[2*i]);
        }
        usRegInputBuf[INP_MH_H_MB]      = vRTC_TASK_GetHoure();
        usRegInputBuf[INP_MH_M_MB ]     = vRTC_TASK_GetMinute();
        usRegInputBuf[ERROR_STATE_MB]   = USER_GerErrorState();
        usRegInputBuf[SENSOR_ERROR_MB]  = getReg8(SENSOR_ERROR);


        convert_float_to_int(getSETPOINT(), &usRegInputBuf[PID_OUT_MB]);

    }
    else                                              //§¦§ã§Ý§Ú §á§Ö§â§Ó§í§Û §Ñ§Õ§â§Ö§ã §Ó §Õ§Ú§Ñ§á§Ñ§Ù§à§ß§Ö §ã§á§Ö§è§Ú§æ§Ú§é§Ö§ã§Ü§Ú§ç §Õ§Ý§ñ §å§ã§ä§â§à§Û§ã§ä§Ó§Ñ §â§Ö§Ô§ã§Ú§ä§â§à§Ó
    {
       if ((DEVICE_TYPE_t)getReg8(DEVICE_TYPE) == DEV_FMCH )
           UodateFMCHInputs();
       else
           UpdateDCVInputs();
    }
}
#define CDV_BP_REG8_SEQ_COUNT 8
#define CDV_BP_REG_SEQ_COUNT 1

#define REG_SEQ_COUNT 5
#define REG8_SEQ_COUNT 13



static const u16 REGS8[REG8_SEQ_COUNT]={
                                       TIME_SENS_MB,
                                       V_MIN_ON,
                                       V_MIN_OFF,
                                       V_MAX_ON,
                                       V_MAX_OFF,
                                       CONTROL_TYPE_MB,
                                       PROTOCOL_TYPE_MB,
                                       MB_ADDRES_MB,
                                       TIME_OUT_MB,
                                       CONTR_MB,
                                       AIN1_TYPE_MB,
                                       AIN2_TYPE_MB,
                                       AIN3_TYPE_MB,};

static const u16 REGS[]             = { SET_MOD1_MB, SET_MOD2_MB, FILTER_LOW_MB, FILTER_HIGH_MB};
static const u16 REGS_FMCH_16[]     = { COMMAND_REG, MODE_REG_MB, LIGTH_REG_MB, TIME_FAN_STOP_MB};
static const u16 REGS_FMCH_FLOAT[]  = { KOOF_P_MB, KOOF_I_MB , KOOF_K_MP };


void UpdateFMCHHoldings()
{
   for (u8 i=0;i<4;i++)
   {
       usRegHoldingBuf[REGS[i]]           = getReg16( FMCH_REGS_MAP [ REGS[i]        - 100 ]);
       usRegHoldingBuf[REGS_FMCH_16[i]]   = getReg8 ( FMCH_REGS_MAP [ REGS_FMCH_16[i]- 100 ]);
   }
   for (u8 i =0;i<3;i++)
   {
       u16 reg_addr = REGS_FMCH_FLOAT[i];
       convert_float_to_int(getRegFloat( FMCH_REGS_MAP[  reg_addr- 100 ] ), &usRegHoldingBuf[reg_addr]);
   }
   if  (usRegHoldingBuf[JOURNAL_SELECT_MB] > getReg16(RECORD_COUNT))
            usRegHoldingBuf[JOURNAL_SELECT_MB] = getReg16(RECORD_COUNT);

}

static const u16 CDV_BP_REGS8[]={CDV_INPUT_SENS_MB ,
                                  CDV_CH_COUNT_MB,
                                  CDV_SETTING_TIMEOUT_MB,
                                  CDV_AFZONE_SETTING_MB,
                                  CDV_MODE_CONTROL,
                                  CDV_MEASERING_UNIT,
                                  CDV_PRIOR_SENS,
                                  CDV_CLEAN_TIMER,
                                  CDV_ROOM_CHANNEL,
                                  AUTO_CALIB_TIMER_MB};
static const u16 CDV_BP_REGS[CDV_BP_REG_SEQ_COUNT]={CDV_ZERO_POINT_TIMEOUT};
static const u16 SettingRegsMap[]={CDV_SETTING_MIN_MB,CDV_SETTING_MID_MB,CDV_SETTING_MAX_MB,CDV_SETTING_ERROR1_MB,CDV_SETTING_ERROR2_MB};
static const u16 REGS_CVB_FLOAT[]={ CDV_KOOF_P_MB, CDV_KOOF_I_MB, CDV_KOOF_K_MP ,CDV_KOOF_P1_MB ,CDV_KOOF_I1_MB,CDV_KOOF_K_2};
static const u16 REGS_CDV_SENS_FLOAT[]={
        CDV_SENSOR1_MIN,
        CDV_SENSOR1_MAX,
        CDV_SENSOR2_MIN,
        CDV_SENSOR2_MAX,
        CDV_SENSOR3_MIN,
        CDV_SENSOR3_MAX,
        CDV_SENSOR1_OFS,
        CDV_SENSOR2_OFS,
        CDV_SENSOR3_OFS,
        CDV_SENSOR1_SET,
        CDV_SENSOR2_SET,
        CDV_SENSOR3_SET,
        CDV_OFFSET_CH2,
} ;

void UpdateCAV_VAV_BPHoldign()
{
    for (u8 i=0;i<6;i++)
    {
        u16 reg_addr = REGS_CVB_FLOAT[i];
        convert_float_to_int(getRegFloat(CDV_REGS_MAP[reg_addr-200]), &usRegHoldingBuf[reg_addr-100]);
    }

    convert_float_to_int(getRegFloat(CDV_REGS_MAP[CDV_F_CHANNEL-200]), &usRegHoldingBuf[CDV_F_CHANNEL-100]);
    convert_float_to_int(getRegFloat(CDV_REGS_MAP[CDV_F_CHANNEL2-200]), &usRegHoldingBuf[CDV_F_CHANNEL2-100]);

     float pdata = 0;
     float idata = 0;

    if (IsPISendScreenNreed())
    {
        u8 index = GetPIDSensorIndex();
        pdata =  getRegFloat(SensorPRegMap[index ]);
        idata =  getRegFloat(SensorIRegMap[index ]);
    }
    convert_float_to_int(pdata, &usRegHoldingBuf[CDV_KOOF_PSESN_MB -100]);
    convert_float_to_int(idata, &usRegHoldingBuf[CDV_KOOF_ISENS_MB- 100]);
    float temp_float;
   for (u8 i=0;i<13;i++)
    {
        u16 reg_addr =REGS_CDV_SENS_FLOAT[i];
        if ((reg_addr == CDV_OFFSET_CH2) &&  (getReg8(MEASERING_UNIT)==2 ))
                {
                    temp_float = getRegFloat(OFFSET_CH2_PA);
                }
                else
                    temp_float = getRegFloat(CDV_REGS_MAP[reg_addr-200]);
        convert_float_to_int(temp_float, &usRegHoldingBuf[reg_addr-100]);
    }

    CAV_VAV_CH_t ch;
    for (u8 i = 0; i < 5; i++)
    {
         u16 reg_addr = SettingRegsMap[i];
         ch = ((reg_addr == CDV_SETTING_ERROR2_MB) || (reg_addr == CDV_OFFSET_CH2)) ? CAV_VAV_CH2 :CAV_VAV_CH1;

         temp_float =(DataModelGetCDVSettings(getRegFloat(CDV_REGS_MAP[reg_addr-200]),ch));
         convert_float_to_int(temp_float, &usRegHoldingBuf[reg_addr -100]);
    }


    for (u8 i=0;i<10;i++)                                      //§©§Ñ§á§à§Ý§ß§ñ§Ö§Þ  8 §Ò§Ú§ä§ß§í§Ö §â§Ö§Ô§Ú§ã§ä§â§í §ã§á§Ö
    {
         usRegHoldingBuf[CDV_BP_REGS8[i] -100 ]      = getReg8(CDV_REGS_MAP[CDV_BP_REGS8[i] -200]);
    }
    for (u8 i=0;i<CDV_BP_REG_SEQ_COUNT;i++)                                      //§©§Ñ§á§à§Ý§ß§ñ§Ö§Þ  16 §Ò§Ú§ä§ß§í§Ö §â§Ö§Ô§Ú§ã§ä§â§í §ã§á§Ö
    {
        usRegHoldingBuf[CDV_BP_REGS[i]  -100  ]      = getReg16(CDV_REGS_MAP[CDV_BP_REGS[i] -200]);
    }
}

void LoadMBControl()
{
    if (getReg8(DEVICE_TYPE) == DEV_CAV_VAV_BP)
    {
        setReg8(MB_CDV_CONTROL, getReg8(CONTROL_MB_SETTING));
    }
}

void MB_TASK_HOLDING_UDATE( u16 start_reg_index )
{
    if (start_reg_index < DEVICE_SPECIFIC_ADDRES)
    {
        static HAL_TimeConfig_T time;
        static HAL_DateConfig_T date;
        usRegHoldingBuf[MODE_MB] = WORK_MODE;
        convert_float_to_int(USER_AOUT_GET(DAC1),&usRegHoldingBuf[AOUT1_C_MB]);
        convert_float_to_int(USER_AOUT_GET(DAC2),&usRegHoldingBuf[AOUT2_C_MB]);
        convert_float_to_int(USER_AOUT_GET(DAC3),&usRegHoldingBuf[AOUT3_C_MB]);
        HAL_RTC_ReadTime(&time);
        HAL_RTC_ReadDate(&date);
        usRegHoldingBuf[LIGTH_REG_MB]  = eGetDOUT(OUT_2);
        usRegHoldingBuf[TIME_H_MB]     = time.hours;
        usRegHoldingBuf[TIME_M_MB]     = time.minutes;
        usRegHoldingBuf[TIME_S_MB]     = time.seconds;
        usRegHoldingBuf[DATE_D_MB]     = date.date;
        usRegHoldingBuf[DATE_M_MB]     = date.month;
        usRegHoldingBuf[DATE_Y_MB]     = date.year;
        usRegHoldingBuf[IP_PORT_MB] =  getReg16(IP_PORT);

        for (u8 i=0;i<12;i++)
                 usRegHoldingBuf[IP_1_MB+i]      = getReg8(IP_1+i);
        for (u8 i=0;i<REG8_SEQ_COUNT;i++)
        {
            usRegHoldingBuf[REGS8[i]]      = getReg8(COMMON_REGS_MAP[REGS8[i]]);
        }

    }
    else
    {
        if  ((DEVICE_TYPE_t)getReg8(DEVICE_TYPE) == DEV_FMCH)
            UpdateFMCHHoldings();
        else
        {
            if (first_start )
                       {
                           first_start = 0;
                           LoadMBControl();
                           printf("control %i\r\n",getReg8(CONTROL_MB_SETTING));
                       }
            UpdateCAV_VAV_BPHoldign();
        }

    }
    if (getReg8(DEVICE_TYPE) == DEV_FMCH)
    {
        usRegHoldingBuf[ZERO_MB] = (CalibrationZeroWhait() == 1) ? 0 :1;
    }
    if (getReg8(DEVICE_TYPE) == DEV_CAV_VAV_BP)
        {

                usRegHoldingBuf[ZERO_MB] = (USER_GetProccesState() == USER_PROCESS_ZERO_CALIB) ? 1 :0;
        }
}





eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int             iRegIndex;
  u16 reg_offet = device_specific_reg_offset[getReg8(DEVICE_TYPE)];
  u16 reg_count = device_specific_input_count[getReg8(DEVICE_TYPE)];

  if ((  usAddress + usNRegs <= REG_INPUT_START + COMMON_REG_INPUT_NREGS  )

      ||  ( ( usAddress >=  reg_offet ) && ( (usAddress + usNRegs) <= (reg_offet + reg_count+1) ) ))
  {
    iRegIndex = ( int )( usAddress - usRegInputStart );
    MB_TASK_INPUTS_UDATE(  iRegIndex  );
    u16 offset=0;
    while( usNRegs > 0 )
    {
        if (iRegIndex >=(reg_offet)) offset = reg_offet-100; else offset = 0;
        *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex-offset] >> 8 );
        *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex-offset] & 0xFF );
        iRegIndex++;
        usNRegs--;
     }
  }
  else
  {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}



eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int             iRegIndex;
  u16 offset;
  u16 reg_offet = device_specific_reg_offset[getReg8(DEVICE_TYPE)];
  u16 reg_count = device_specific_reg_count[getReg8(DEVICE_TYPE)];

  if ( ( usAddress + usNRegs <= COMMON_REG_COUNT  )
          ||  ( ( usAddress >=  reg_offet ) && ( (usAddress + usNRegs) <= (reg_offet+ reg_count+1) ) ))
  {
    iRegIndex = ( int )( usAddress - usRegHoldingStart );
    switch ( eMode )
    {
    case MB_REG_READ:
      MB_TASK_HOLDING_UDATE(  iRegIndex  );

      while( usNRegs > 0 )
      {
        if (iRegIndex >=(reg_offet)) offset = reg_offet-100; else offset = 0;
        *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex-offset] >> 8 );
        *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex-offset] & 0xFF );
        iRegIndex++;
        usNRegs--;
      }
      break;
    case MB_REG_WRITE:
       while( usNRegs > 0 )
      {
        if (iRegIndex >= reg_offet) offset = reg_offet-100; else offset = 0;
        usRegHoldingBuf[iRegIndex-offset ] = *pucRegBuffer++ << 8;
        usRegHoldingBuf[iRegIndex-offset ] |= *pucRegBuffer++;
        vSetRegData(iRegIndex);
        iRegIndex++;
        usNRegs--;
      }
    }
  }
  else
  {
    eStatus = MB_ENOREG;
  }

  return eStatus;
}





#include "mbutils.h"

eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
        eMBErrorCode    eStatus = MB_ENOERR;
        USHORT          iRegBitIndex;
        UCHAR *         pucCoilBuf  = ucSCoilBuf;
        USHORT          usCoilStart = REG_COILS_START;
        /* it already plus one in modbus function method. */
        usAddress--;

        if(  ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_NREGS ) )
        {
            iRegBitIndex = (USHORT) (usAddress - usCoilStart);
            switch ( eMode )
            {
            /* read current coil values from the protocol stack. */
            case MB_REG_READ:
                xGetOut( pucCoilBuf);
                while ( usNCoils > 0)
                {
                    UCHAR ucResult = xMBUtilGetBits( pucCoilBuf, iRegBitIndex, 1 );
                    xMBUtilSetBits( pucRegBuffer, iRegBitIndex, 1, ucResult );
                    iRegBitIndex++;
                    usNCoils--;
                }
                break;
                /* write current coil values with new values from the protocol stack. */
            case MB_REG_WRITE:
                while ( usNCoils > 0 )
                {
                    UCHAR ucResult = pucRegBuffer[0];
                    eSetDUT(iRegBitIndex,ucResult);
                    iRegBitIndex++;
                    usNCoils--;
                }
                break;
            }
        }
        else
        {
            eStatus = MB_ENOREG;
        }
        return eStatus;
}

/**
 * Modbus slave discrete callback function.
 *
 * @param pucRegBuffer discrete buffer
 * @param usAddress discrete address
 * @param usNDiscrete discrete number
 *
 * @return result
 */
eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegBitIndex; 
    /* it already plus one in modbus function method. */
    usAddress--;

    if (usAddress + usNDiscrete    <= REG_DISCRETE_START  + REG_DISCRETE_NREGS)
    {
        iRegBitIndex = (USHORT) (usAddress - REG_DISCRETE_START);
        xGetDins(  ucSDiscInBuf );
        while (usNDiscrete > 0)
        {
            UCHAR ucResult = xMBUtilGetBits( ucSDiscInBuf, iRegBitIndex, 1 );
            xMBUtilSetBits( pucRegBuffer, iRegBitIndex, 1, ucResult );
            iRegBitIndex++;
            usNDiscrete--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}


void MBRTU_task(void *pvParameters)
{
     eMBErrorCode    xStatus;
     for( ;; )
     {
         if (getReg8(MB_PROTOCOL_TYPE)== MKV_MB_RTU)
         {
             xStatus =  eMBInit(MB_RTU,getReg8(MB_RTU_ADDR),HAL_USART4,19200,MB_PAR_ODD );
         }
         else
         {
             xStatus =  eMBTCPInit( MB_TCP_PORT_USE_DEFAULT );

         }
         if (xStatus == MB_ENOERR)
         {
             if( eMBEnable() == MB_ENOERR )
             {
                do
                {
                    xStatus = eMBPoll(  );

                }
                while( xStatus == MB_ENOERR );
             }
         }
     }
     ( void )eMBDisable(  );
     ( void )eMBClose(  );
}
