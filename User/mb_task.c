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


#if REG_COILS_NREGS%8 && REG_COILS_NREGS>8
UCHAR    ucSCoilBuf[REG_COILS_NREGS/8+1];
#else
UCHAR    ucSCoilBuf[REG_COILS_NREGS/8];
#endif
#if REG_DISCRETE_NREGS%8 && REG_DISCRETE_NREGS>8
UCHAR ucSDiscInBuf[REG_DISCRETE_NREGS/8+1];
#else
UCHAR ucSDiscInBuf[REG_DISCRETE_NREGS/8];
#endif

#define REG_INPUT_START 0x01
#define REG_INPUT_NREGS 40
#define REG_HOLDING_START 0x01


#define REG_HOLDING_START 0x01
#define REG_HOLDING_NREGS 151


static USHORT usRegInputStart = REG_INPUT_START;
static USHORT usRegInputBuf[REG_INPUT_NREGS];
static USHORT usRegHoldingStart = REG_HOLDING_START;
static USHORT usRegHoldingBuf[REG_HOLDING_NREGS];

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



#define INP_MODE_MB      24
#define INP_MH_H_MB      25
#define INP_MH_M_MB      26
#define INP_STATE_MB     27
#define ERROR_STATE_MB   28
#define AOUT1_MB         29
#define AOUT2_MB         31
#define AOUT3_MB         33


#define COMMON_REG_COUNT (AOUT3_C_MB+3)


//妓快忍我扼找把抑 FMCH
#define FMCH_OFFSET       0
#define FMCH_COUNT        ( 14 )
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

#define CDV_OFFSET          100
#define CDV_COUNT         31
//妓快忍我扼找把抑 CDV
#define CDV_KOOF_P_MB           200
#define CDV_KOOF_I_MB           202
#define CDV_KOOF_K_MP           204
#define CDV_KOOF_I1_MB          208
#define CDV_KOOF_P1_MB          210
#define CDV_KOOF_I2_MB          212
#define CDV_KOOF_P2_MB          214
#define CDV_KOOF_I3_MB          216
#define CDV_KOOF_P3_MB          218
#define CDV_AFZONE_SETTING_MB   220
#define CDV_CH_COUNT_MB         221
#define CDV_MEASERING_UNIT      222
#define CDV_OFFSET_CH2          223
#define CDV_PRIOR_SENS          225
#define CDV_CLEAN_TIMER         226
#define CDV_ZERO_POINT_TIMEOUT  227
#define CDV_KK_SENSOR_TYPE      228
#define CDV_CO2_SENSOR_TYPE     229
#define CDV_H_SENSOR_TYPE       230


#define BP_OFFSET          200
#define BP_COUNT           22
//妓快忍我扼找把抑 BP
#define BP_KOOF_P_MB            300
#define BP_KOOF_I_MB            302
#define BP_KOOF_K_MB            304
#define BP_KOOF_I1_MB           308
#define BP_KOOF_P1_MB           310
#define BP_KOOF_I2_MB           312
#define BP_KOOF_P2_MB           314
#define BP_KOOF_I3_MB           316
#define BP_KOOF_P3_MB           318
#define BP_AFZONE_SETTING_MB    320
#define BP_CH_COUNT_MB          321
#define BP_MEASERING_UNIT       322
#define BP_OFFSET_CH2           323
#define BP_PRIOR_SENS           325
#define BP_CLEAN_TIMER          326
#define BP_ZERO_POINT_TIMEOUT   327
#define BP_KK_SENSOR_TYPE       328
#define BP_CO2_SENSOR_TYPE      329
#define BP_H_SENSOR_TYPE        330

//#define SENS_COOF  18
//#define DAC_DATA   19
#define SENS_PERIOD 58
#define SENS_COUNT  59


/*均忱把快扼忘 INPUTS 把快忍我扼找抉把志*/


#define DAC_CAL_POINTS 10
uint16_t point[DAC_CAL_POINTS][2] ={ {0,0},{ 140,100},
        {230,200},
        {350,300},
        {450,400},
        {570,500},
        {680,600},
        {800,700},
        {915,800},
        {1030,900}};

uint16_t ConvertData( uint16_t input)
{


   return ( input/(float)(1000.0/984.0));


}
uint16_t sens_coof = 2048;

uint16_t GetSensCoof()
{
    return sens_coof;
}

static u8 WORK_MODE =0;


u8 MB_TASK_GetMode()
{
    return (WORK_MODE);
}



static u8 getRegID( u16 mb_reg_address)
{

    if (mb_reg_address >= (CDV_OFFSET+100))
    {
        switch (mb_reg_address)
        {
                case (CDV_KOOF_I1_MB +1):
                case (BP_KOOF_I1_MB +1):
                                        return  (COOF_I1);
                case (CDV_KOOF_P1_MB+1):
                case (BP_KOOF_P1_MB+1):
                                          return  (COOF_P1);
                case (CDV_KOOF_I2_MB +1):
                case (BP_KOOF_I2_MB +1):
                                         return  (COOF_I2);
                case (CDV_KOOF_P2_MB +1):
                case (BP_KOOF_P2_MB +1):
                                         return  (COOF_P2);
                case (CDV_KOOF_I3_MB +1):
                case (BP_KOOF_I3_MB +1):
                                        return  (COOF_I3);
                case (CDV_KOOF_P3_MB +1):
                case (BP_KOOF_P3_MB +1):
                                        return  (COOF_P3);
                case CDV_AFZONE_SETTING_MB:
                case BP_AFZONE_SETTING_MB:
                                        return (AFTER_ZONE_SETTING);
                case CDV_CH_COUNT_MB:
                case BP_CH_COUNT_MB:
                                        return ( CDV_BP_CH_COUNT );
                case CDV_MEASERING_UNIT:
                case BP_MEASERING_UNIT:
                                        return  (MEASERING_UNIT);
                case  CDV_OFFSET_CH2+1:
                case  BP_OFFSET_CH2+1:
                                        return  ( OFFSET_CH2);
                case BP_PRIOR_SENS:
                case CDV_PRIOR_SENS:
                                        return (PRIOR_SENSOR );
                case CDV_CLEAN_TIMER:
                case BP_CLEAN_TIMER:
                                      return (CLEAN_TIMER);
                case CDV_ZERO_POINT_TIMEOUT:
                case BP_ZERO_POINT_TIMEOUT:
                                    return (ZERO_POINT_TIMEOUT);
                case CDV_KK_SENSOR_TYPE:
                case BP_KK_SENSOR_TYPE:
                                        return (KK_SENSOR_TYPE);
                case CDV_CO2_SENSOR_TYPE:
                case BP_CO2_SENSOR_TYPE:
                                        return (CO2_SENSOR_TYPE);
                case CDV_H_SENSOR_TYPE:
                case BP_H_SENSOR_TYPE:
                                        return (H_SENSOR_TYPE);
        }
    }
    else

    switch (mb_reg_address)
    {
        case CONTR_MB:          return (CONTRAST);
        case MB_ADDRES_MB:      return (MB_RTU_ADDR);
        case TIME_OUT_MB:       return (MOD_BUS_TIMEOUT);
        case COMMAND_REG:
        case INP_STATE_MB:      return (SYSTEM_START);
        case INP_MODE_MB:       return (MODE);
        case IP_PORT_MB:        return (IP_PORT);
        case LIGTH_REG_MB:      return (LIGTH);
        case MODE_REG_MB:       return (MODE);
        case SET_MOD1_MB:       return (SETTING1);
        case SET_MOD2_MB:       return (SETTING2);
        case FILTER_LOW_MB:     return (FILTER_LOW);
        case FILTER_HIGH_MB:    return (FILTER_HIGH);
        case TIME_SENS_MB:      return (SENSOR_COUNT);
        case TIME_FAN_STOP_MB:  return (FAN_START_TIMEOUT);
        case (KOOF_P_MB+1):
        case (CDV_KOOF_P_MB+1):
        case (BP_KOOF_P_MB+1):
                                return  (COOF_P);
        case (KOOF_I_MB+1):
        case (CDV_KOOF_I_MB+1):
        case (BP_KOOF_I_MB+1):
                                return  (COOF_I);
        case (KOOF_K_MP+1):
        case (CDV_KOOF_K_MP+1):
        case (BP_KOOF_K_MB+1):
                                return  (KOOFKPS);
        case V_MIN_ON:          return (LOW_VOLTAGE_ON);
        case V_MIN_OFF:         return (LOW_VOLTAGE_OFF);
        case V_MAX_ON:          return (HIGH_VOLTAGE_ON);
        case V_MAX_OFF:          return (HIGH_VOLTAGE_OFF);
        case CONTROL_TYPE_MB :   return (CONTROL_TYPE);
        case PROTOCOL_TYPE_MB:  return (MB_PROTOCOL_TYPE);

    }
    return 0;
}

void vSetRegData( u16 adress)
{

   u8 set_time_flag = 0;
   u8 set_date_flag = 0;
   float data;
   u16 byte_data =(u16)usRegHoldingBuf[adress];
   u8 reg_addr = getRegID(adress);
   switch (adress)
   {

       case ZERO_MB:
           if  (byte_data !=0)
           {
               if (USER_GetProccesState() == USER_PROCCES_IDLE)
                   CalibrateZeroStart();
               else
               {
                   usRegHoldingBuf[adress] = 0;
               }
           }
           break;
       case CDV_KK_SENSOR_TYPE:
       case BP_KK_SENSOR_TYPE:
       case CDV_H_SENSOR_TYPE:
       case BP_H_SENSOR_TYPE:
       case CDV_CO2_SENSOR_TYPE:
       case BP_CO2_SENSOR_TYPE:
       case CDV_AFZONE_SETTING_MB:
       case BP_AFZONE_SETTING_MB:
       case CDV_CH_COUNT_MB:
       case BP_CH_COUNT_MB:
       case LIGTH_REG_MB:
       case MODE_REG_MB:
       case CDV_MEASERING_UNIT:
       case BP_MEASERING_UNIT:
       case CDV_PRIOR_SENS:
       case BP_PRIOR_SENS:
            VerifyAndSetReg8(reg_addr, (uint8_t) byte_data );
            break;
       case COMMAND_REG:
           USER_SetControlState( (uint8_t) byte_data );
           break;
       case MODE_MB:
           if (byte_data  > 3)
               usRegHoldingBuf[adress] = WORK_MODE;
           else
           {
               printf("set mode %i\r\n",byte_data);
               WORK_MODE = byte_data ;
               if (WORK_MODE ==3 )
               {
                   NVIC_SystemReset();
               }
           }
           break;
       case AOUT1_C_MB+1:
           if (WORK_MODE ==2)
          {

               data = convert_int_to_float( &usRegHoldingBuf[adress-1]);
               printf("set data %f\r\n",data);
               USER_AOUT_SET(DAC1,data);
         }

          break;
       case AOUT2_C_MB+1:
       if (WORK_MODE ==2)
                 {
                      data = convert_int_to_float( &usRegHoldingBuf[adress-1]);
                      USER_AOUT_SET(DAC2,data);
                }
          break;
       case AOUT3_C_MB+1:
       if (WORK_MODE ==2)
                 {
                      data = convert_int_to_float( &usRegHoldingBuf[adress-1]);
                      USER_AOUT_SET(DAC3,data);
                }
       break;
       case SENS_COUNT:
           vSetCount(usRegHoldingBuf[adress]);
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
       case (KOOF_I_MB+1):
       case (KOOF_K_MP+1):
       case (KOOF_P_MB+1):
       case (CDV_KOOF_I_MB+1):
       case (CDV_KOOF_K_MP+1):
       case (CDV_KOOF_P_MB+1):
       case (CDV_KOOF_I1_MB+1):
       case (CDV_KOOF_P1_MB+1):
       case (CDV_KOOF_I2_MB+1):
       case (CDV_KOOF_P2_MB+1):
       case (CDV_KOOF_I3_MB+1):
       case (CDV_KOOF_P3_MB+1):
       case (BP_KOOF_I_MB+1):
       case (BP_KOOF_K_MB+1):
       case (BP_KOOF_P_MB+1):
       case (BP_KOOF_I1_MB+1):
       case (BP_KOOF_P1_MB+1):
       case (BP_KOOF_I2_MB+1):
       case (BP_KOOF_P2_MB+1):
       case (BP_KOOF_I3_MB+1):
       case (BP_KOOF_P3_MB+1):
       case (CDV_OFFSET_CH2+1):
       case (BP_OFFSET_CH2+1):
           data = convert_int_to_float( &usRegHoldingBuf[adress-1]);
           saveRegFloat(reg_addr, data);
           break;
       case MB_ADDRES_MB:
       case CONTROL_TYPE_MB:
       case PROTOCOL_TYPE_MB:
           if (WORK_MODE)
           {
                SaveReg8(reg_addr, byte_data);
           }
           break;
       case TIME_H_MB:
           if (byte_data <24) set_time_flag = 1;
           break;
       case TIME_M_MB:
       case TIME_S_MB:
           if (byte_data <60) set_time_flag = 1;
           break;
       case DATE_D_MB:
           if ((byte_data <32) && (byte_data>0)) set_date_flag = 1;
           break;
       case DATE_M_MB:
           if ((byte_data <13) && (byte_data>0)) set_date_flag = 1;
           break;
       case DATE_Y_MB:
           if (byte_data <99) set_date_flag = 1;
           break;
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
               SaveReg8(IP_1 + (adress - IP_1_MB), (uint8_t)usRegHoldingBuf[adress]);
           }
           break;
       case IP_PORT_MB:
           if (WORK_MODE)
           {
               saveReg16(reg_addr, usRegHoldingBuf[adress]);
           }
           break;
       case  SET_MOD1_MB:
       case  SET_MOD2_MB:
       case  FILTER_LOW_MB:
       case  FILTER_HIGH_MB:
       case CDV_ZERO_POINT_TIMEOUT:
       case BP_ZERO_POINT_TIMEOUT:

           saveReg16(reg_addr, usRegHoldingBuf[adress]);
           break;
       case CDV_CLEAN_TIMER:
       case BP_CLEAN_TIMER:
       case CONTR_MB:
       case TIME_SENS_MB:
       case TIME_FAN_STOP_MB:
       case TIME_OUT_MB:
           SaveReg8(reg_addr,byte_data);
           break;
   }
   if (set_time_flag)
   {
       HAL_TimeConfig_T time;
       time.hours = usRegHoldingBuf[TIME_H_MB];
       time.minutes = usRegHoldingBuf[TIME_M_MB];
       time.seconds = usRegHoldingBuf[TIME_S_MB];
       HAL_RTC_ConfigTime(&time);
   }
   if (set_date_flag)
   {
       HAL_DateConfig_T date;
       date.date  = usRegHoldingBuf[DATE_D_MB];
       date.month = usRegHoldingBuf[DATE_M_MB];
       date.year =  usRegHoldingBuf[DATE_Y_MB];
       HAL_RTC_ConfigDate(&date);
   }
}


static const AIN_CHANNEL_t AINS[]={DIG_TEMP,DIG_PRES,SENS1,DIG2_TEMP,DIG2_PRES,SENS2,DCAIN1,DCAIN2,DCAIN3,DCAIN4,DC24};

static void MB_TASK_INPUTS_UDATE()
{
    int32_t tempdata;
    for (u8 i =0; i<11;i++)
    {
        tempdata =(int32_t) (getAIN(AINS[i])*1000);
        *((float*) (usRegInputBuf+2*i)) =  (float)tempdata/1000.0;
    }
    tempdata =(uint32_t) getAIN(AC220);
    *((float*) (usRegInputBuf+22)) =  tempdata;
    usRegInputBuf[INP_MODE_MB]    = getReg8(getRegID(INP_MODE_MB));
    usRegInputBuf[INP_MH_H_MB]    = vRTC_TASK_GetHoure();
    usRegInputBuf[INP_MH_M_MB ]   = vRTC_TASK_GetMinute();
    usRegInputBuf[INP_STATE_MB]   = getReg8(getRegID(INP_STATE_MB));
    usRegInputBuf[ERROR_STATE_MB] = USER_GerErrorState();
    convert_float_to_int(USER_AOUT_GET(DAC1),&usRegInputBuf[AOUT1_MB]);
    convert_float_to_int(USER_AOUT_GET(DAC2),&usRegInputBuf[AOUT2_MB]);
    convert_float_to_int(USER_AOUT_GET(DAC3),&usRegInputBuf[AOUT3_MB]);


}
#define CDV_BP_REG8_SEQ_COUNT 8
#define CDV_BP_REG_SEQ_COUNT 1

#define REG_SEQ_COUNT 5
#define REG8_SEQ_COUNT 12


static const u8 CDV_BP_REGS8[CDV_BP_REG8_SEQ_COUNT]={CDV_AFZONE_SETTING_MB,CDV_CH_COUNT_MB ,CDV_MEASERING_UNIT,CDV_PRIOR_SENS,CDV_CLEAN_TIMER,CDV_KK_SENSOR_TYPE,CDV_CO2_SENSOR_TYPE,CDV_H_SENSOR_TYPE};
static const u8 CDV_BP_REGS[CDV_BP_REG_SEQ_COUNT]={CDV_ZERO_POINT_TIMEOUT};
static const u8 REGS[REG_SEQ_COUNT]={IP_PORT_MB,SET_MOD1_MB,SET_MOD2_MB,FILTER_LOW_MB,FILTER_HIGH_MB};
static const u8 REGS8[REG8_SEQ_COUNT]={COMMAND_REG,
                                       TIME_SENS_MB,
                                       TIME_FAN_STOP_MB,
                                       V_MIN_ON,
                                       V_MIN_OFF,
                                       V_MAX_ON,
                                       V_MAX_OFF,
                                       CONTROL_TYPE_MB,
                                       PROTOCOL_TYPE_MB,
                                       MB_ADDRES_MB,
                                       TIME_OUT_MB,
                                       CONTR_MB};


static u16 const device_specific_reg_offset[] =  { FMCH_OFFSET ,CDV_OFFSET, BP_OFFSET };
static u16 const  device_specific_reg_count[] =  { FMCH_COUNT  ,CDV_COUNT , BP_COUNT  };

void MB_TASK_HOLDING_UDATE()
{
    static HAL_TimeConfig_T time;
    static HAL_DateConfig_T date;
    u16 reg_offet = 100;
    int32_t tempdata;
    usRegHoldingBuf[MODE_MB] = WORK_MODE;

    tempdata =(int32_t) (getRegFloat(COOF_P)*1000);
    convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[KOOF_P_MB]);
    tempdata =(int32_t) (getRegFloat(COOF_I)*1000);
    convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[KOOF_I_MB]);
    tempdata =(int32_t) (getRegFloat(KOOFKPS)*1000);
    convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[KOOF_K_MP]);
    //if  ( (DEVICE_TYPE_t)getReg8(DEVICE_TYPE) ==DEV_FMCH )
   // {
//
   // }

    if ( ( (DEVICE_TYPE_t)getReg8(DEVICE_TYPE) ==DEV_CDV ) || ( (DEVICE_TYPE_t)getReg8(DEVICE_TYPE) ==DEV_BP ) )
    {
          tempdata =(int32_t) (getRegFloat(COOF_P1)*1000);
          convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[CDV_KOOF_P1_MB-reg_offet]);
          tempdata =(int32_t) (getRegFloat(COOF_I1)*1000);
          convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[CDV_KOOF_I1_MB-reg_offet]);
          tempdata =(int32_t) (getRegFloat(COOF_P2)*1000);
          convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[CDV_KOOF_P2_MB-reg_offet]);
          tempdata =(int32_t) (getRegFloat(COOF_I2)*1000);
          convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[CDV_KOOF_I2_MB-reg_offet]);
          tempdata =(int32_t) (getRegFloat(COOF_P3)*1000);
          convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[CDV_KOOF_P3_MB-reg_offet]);
          tempdata =(int32_t) (getRegFloat(COOF_I3)*1000);
          convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[CDV_KOOF_I3_MB-reg_offet]);
          tempdata =(int32_t) (getRegFloat(OFFSET_CH2)*1000);
          convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[CDV_KOOF_I3_MB-reg_offet]);
          for (u8 i=0;i<CDV_BP_REG8_SEQ_COUNT;i++)                                      //妝忘扭抉抖扶攸快技  8 忌我找扶抑快 把快忍我扼找把抑 扼扭快
          {
               u8 index = getRegID(CDV_BP_REGS8[i]);
               usRegHoldingBuf[CDV_BP_REGS8[i] -CDV_OFFSET ]      = getReg8(index);
          }
          for (u8 i=0;i<CDV_BP_REG_SEQ_COUNT;i++)                                      //妝忘扭抉抖扶攸快技  16 忌我找扶抑快 把快忍我扼找把抑 扼扭快
          {
               u8 index = getRegID(CDV_BP_REGS[i]);
               usRegHoldingBuf[CDV_BP_REGS[i]]      = getReg16(index);
          }
    }

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
    for (u8 i=0;i<12;i++)
         usRegHoldingBuf[IP_1_MB+i]       = getReg8(IP_1+i);
    for (u8 i=0;i<REG_SEQ_COUNT;i++)
    {
        u8 index = getRegID(REGS[i]);
        usRegHoldingBuf[REGS[i]]      = getReg16(index);
    }
    for (u8 i=0;i<REG8_SEQ_COUNT;i++)
    {
        u8 index = getRegID(REGS8[i]);
        usRegHoldingBuf[REGS8[i]]      = getReg8(index);
    }
}

eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int             iRegIndex;

  if( ( usAddress >= REG_INPUT_START ) && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
  {
    iRegIndex = ( int )( usAddress - usRegInputStart );
    MB_TASK_INPUTS_UDATE();
    while( usNRegs > 0 )
    {
        *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
        *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
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
  u16 reg_offet = device_specific_reg_offset[getReg8(DEVICE_TYPE)];
  u16 reg_count = device_specific_reg_count[getReg8(DEVICE_TYPE)];

  if (( ( usAddress >= REG_HOLDING_START ) && ( usAddress + usNRegs <= COMMON_REG_COUNT ) ) ||  ( ( usAddress >= 100 + reg_offet ) && ( (usAddress + usNRegs) <= (reg_offet+ 100 + reg_count+1) ) ))
  {
    iRegIndex = ( int )( usAddress - usRegHoldingStart );
    switch ( eMode )
    {
    case MB_REG_READ:
      MB_TASK_HOLDING_UDATE();
      u16 offset=0;
      while( usNRegs > 0 )
      {
        if (iRegIndex >=(100+ reg_offet)) offset = reg_offet; else offset = 0;
        *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex-offset] >> 8 );
        *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex-offset] & 0xFF );
        iRegIndex++;
        usNRegs--;
      }
      break;

    case MB_REG_WRITE:
       while( usNRegs > 0 )
      {
        if (iRegIndex >=(100+ reg_offet)) offset = reg_offet; else offset = 0;
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
        USHORT          iRegIndex , iRegBitIndex , iNReg;
        UCHAR *         pucCoilBuf  = ucSCoilBuf;
        USHORT          usCoilStart = REG_COILS_START;
        iNReg =  usNCoils / 8 + 1;

        /* it already plus one in modbus function method. */
        usAddress--;

        if( ( usAddress >= REG_COILS_START) &&  ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_NREGS ) )
        {
            iRegIndex = (USHORT) (usAddress - usCoilStart) / 8;
            iRegBitIndex = (USHORT) (usAddress - usCoilStart);
          //  iRegBitIndex++;
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
    USHORT          iRegIndex , iRegBitIndex , iNReg;
    UCHAR *         pucDiscreteInputBuf;
    USHORT          usDiscreteInputStart;
    iNReg =  usNDiscrete / 8 + 1;

    pucDiscreteInputBuf = ucSDiscInBuf;

    usDiscreteInputStart = REG_DISCRETE_START;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress >= REG_DISCRETE_START )
            && (usAddress + usNDiscrete    <= REG_DISCRETE_START  + REG_DISCRETE_NREGS))
    {
        iRegIndex = (USHORT) (usAddress - usDiscreteInputStart) / 8;
        iRegBitIndex = (USHORT) (usAddress - usDiscreteInputStart) % 8;
        xGetDins(  ucSDiscInBuf);
        while (iNReg > 0)
        {
            *pucRegBuffer++ = xMBUtilGetBits(&pucDiscreteInputBuf[iRegIndex++],
                    iRegBitIndex, 8);
            iNReg--;
        }
        pucRegBuffer--;
        /* last discrete */
        usNDiscrete = usNDiscrete % 8;
        /* filling zero to high bit */
        *pucRegBuffer = *pucRegBuffer << (8 - usNDiscrete);
        *pucRegBuffer = *pucRegBuffer >> (8 - usNDiscrete);
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}


void MBTCP_task(void *pvParameters)
{
     eMBErrorCode    xStatus;

     for( ;; )
     {
         vTaskDelay(1);
         if( eMBTCPInit( MB_TCP_PORT_USE_DEFAULT ) != MB_ENOERR )
         {
            printf("can't initialize modbus stack!\r\n" );
         }
         else if( eMBEnable(  ) != MB_ENOERR )
         {

         }
         else
         {
                do
                {
                    xStatus = eMBPoll(  );
                }
                while( xStatus == MB_ENOERR );
         }
     }
     ( void )eMBDisable(  );
     ( void )eMBClose(  );
}

void MBRTU_task(void *pvParameters)
{
     u8 mb_ready = 0;
     eMBErrorCode    xStatus;
     for( ;; )
     {
         if (getReg8(MB_PROTOCOL_TYPE)== MKV_MB_RTU)
         {

             if (eMBInit(MB_RTU,getReg8(MB_RTU_ADDR),HAL_USART4,19200,MB_PAR_ODD ) == MB_ENOERR )
             {
                mb_ready = 1;
             }
         }
         else
         {
             printf("tcp\r\n");
         if( eMBTCPInit( MB_TCP_PORT_USE_DEFAULT ) == MB_ENOERR )
        {

            mb_ready = 1;
        }
         }
         if (mb_ready)
         {
             if( eMBEnable(  ) == MB_ENOERR )
             {
                do
                {
                    vTaskDelay(1);
                    xStatus = eMBPoll(  );

                }
                while( xStatus == MB_ENOERR );
             }
         }
     }
     ( void )eMBDisable(  );
     ( void )eMBClose(  );
}
