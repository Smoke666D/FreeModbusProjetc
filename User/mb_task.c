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

#define REG_HOLDING_START 0x01


#define REG_HOLDING_START 0x01
#define REG_HOLDING_NREGS 151
#define REG_INPUTS_NREGS  151

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
#define INP_MH_H_MB      26
#define INP_MH_M_MB      27
#define SENSOR_ERROR_MB  28

#define COMMON_REG_INPUT_NREGS ( SENSOR_ERROR_MB+1)

#define COMMON_REG_COUNT (AIN3_TYPE_MB+3)

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
#define CDV_KOOF_I_MB           203
#define CDV_KOOF_P_MB           205
#define CDV_KOOF_I1_MB          207
#define CDV_KOOF_P1_MB          209
#define CDV_SETTING_MIN_MB      211
#define CDV_SETTING_MID_MB      213
#define CDV_SETTING_MAX_MB      215
#define CDV_SETTING_ERROR1_MB   217
#define CDV_SETTING_ERROR2_MB   219
#define CDV_F_CHANNEL           221
#define CDV_OFFSET_CH2          223
#define CDV_CLEAN_TIMER         225
#define CDV_MEASERING_UNIT      226
#define CDV_ZERO_POINT_TIMEOUT  227
#define CDV_SETTING_TIMEOUT_MB  228
#define CDV_MODE_CONTROL        229
#define CDV_AFZONE_SETTING_MB   230
#define CDV_PRIOR_SENS          231
#define CDV_KK_SENSOR_TYPE      232
#define CDV_CO2_SENSOR_TYPE     233
#define CDV_H_SENSOR_TYPE       234
#define INPUT_SENSOR_TYPE_MB    235


#define CDV_FACT_1              200
#define CDV_FACT_2              202
#define CDV_CUR_STATE           204

#define CDV_COUNT             ( CDV_CH_COUNT_MB  - INPUT_SENSOR_TYPE_MB + 1)


#define CDV_INPUTS_COUNT         ( CDV_CUR_STATE - CDV_FACT_1   + 1 )

#define BP_OFFSET          200
#define BP_COUNT           22
//§²§Ö§Ô§Ú§ã§ä§â§í BP
#define BP_KOOF_P_MB            300
#define BP_KOOF_I_MB            302
#define BP_KOOF_K_MB            304
#define BP_KOOF_I1_MB           306
#define BP_KOOF_P1_MB           308
#define BP_KOOF_I2_MB           310
#define BP_KOOF_P2_MB           312
#define BP_KOOF_I3_MB           314
#define BP_KOOF_P3_MB           316
#define BP_AFZONE_SETTING_MB    318
#define BP_CH_COUNT_MB          319
#define BP_MEASERING_UNIT       320
#define BP_OFFSET_CH2           321
#define BP_PRIOR_SENS           323
#define BP_CLEAN_TIMER          324
#define BP_ZERO_POINT_TIMEOUT   325
#define BP_KK_SENSOR_TYPE       326
#define BP_CO2_SENSOR_TYPE      327
#define BP_H_SENSOR_TYPE        328
#define PB_F_CHANNEL            329

#define BP_INPUTS_COUNT          0

//#define SENS_COOF  18
//#define DAC_DATA   19
#define SENS_PERIOD 58
#define SENS_COUNT  59



static u16 const device_specific_reg_offset[] =  { FMCH_OFFSET ,CDV_OFFSET,CDV_OFFSET, BP_OFFSET };
static u16 const  device_specific_reg_count[] =  { FMCH_COUNT  ,CDV_COUNT , CDV_COUNT, BP_COUNT  };
/*§¡§Õ§â§Ö§ã§Ñ INPUTS §â§Ö§Ô§Ú§ã§ä§à§â§Ó*/


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
                                        IP_1,                      //16
                                        IP_2,                      //17
                                        IP_3,                      //18
                                        IP_4,                      //19
                                        GATE_1,                      //20
                                        GATE_2,                      //21
                                        GATE_3,                      //22
                                        GATE_4,                      //23
                                        MASK_1,                      //24
                                        MASK_2,                      //25
                                        MASK_3,                      //26
                                        MASK_4,                      //27
                                        IP_PORT,                //28
                                        SENSOR_COUNT,           //29
                                        0,                      //30
                                        0,                      //31
                                        0,                      //32
                                        0,                      //33
                                        0,                      //34
                                        0,                      //35
                                        0,                      //36
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
                                               CDV_BP_CH_COUNT,  //0
                                               KOOFKPS,          //1
                                               KOOFKPS,          //2
                                               COOF_P,           //3
                                               COOF_P,           //4
                                               COOF_I,           //5
                                               COOF_I,           //6
                                               COOF_I1,          //7
                                               COOF_I1,          //8
                                               COOF_P1,          //9
                                               COOF_P1,          //10
                                               SETTING_MIN,       //11
                                               SETTING_MIN,       //12
                                               SETTING_MID,       //13
                                               SETTING_MID,       //14
                                               SETTING_MAX,       //15
                                               SETTING_MAX,       //16
                                               CH1_SETTING,       //17
                                               CH1_SETTING,       //18
                                               CH2_SETTING,       //19
                                               CH2_SETTING,       //20
                                               F_CHANNEL,         //21
                                               F_CHANNEL,         //22
                                               OFFSET_CH2,        //23
                                               OFFSET_CH2,        //24
                                               CLEAN_TIMER,       //25
                                               MEASERING_UNIT,    //26
                                               ZERO_POINT_TIMEOUT,//27
                                               SETTING_TIMER,     //28
                                               CDV_CONTOROL,      //29
                                               AFTER_ZONE_SETTING,//30
                                               PRIOR_SENSOR,      //31
                                               INPUT_SENSOR_MODE,    //32

                                               INPUT_SENSOR_TYPE, //35


};




void vSetRegData( u16 adress)
{
   float data;
   u8 dev_type = getReg8(DEVICE_TYPE);


   printf("dev1 = %i\r\n",getReg8(DEVICE_TYPE));
   if (adress < DEVICE_SPECIFIC_ADDRES)
   {
       u16 byte_data =(u16)usRegHoldingBuf[adress];
       u8 set_time_flag = 0;
       u8 set_date_flag = 0;
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
             case AIN1_TYPE_MB:
             case AIN2_TYPE_MB:
             case AIN3_TYPE_MB:
                  VerifyAndSetReg8(AIN1_TYPE + (AIN1_TYPE_MB - adress), (uint8_t) byte_data );
                  break;
             case MODE_MB:
                 if (byte_data  > 3)
                     {
                        if (0x55) vDataModelResetJournal();
                        if (0xAA) ResetMotorHour();
                         usRegHoldingBuf[adress] = WORK_MODE;
                     }
                 else
                 {

                     WORK_MODE = byte_data ;
                     if (WORK_MODE ==3 )
                     {
                         NVIC_SystemReset();
                     }
                     if ((WORK_MODE == 0 ) && (getReg8(TEST_MODE) == 1 )) SaveReg8(TEST_MODE,0);
                     else
                     if ((WORK_MODE == 2 ) && (getReg8(TEST_MODE) == 0 )) SaveReg8(TEST_MODE,1);
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
                 if (byte_data <usRegHoldingBuf[V_MIN_OFF] ) SaveReg8(LOW_VOLTAGE_ON,byte_data);
                 break;
             case V_MIN_OFF:
                 if ((byte_data > usRegHoldingBuf[V_MIN_ON] ) && (byte_data < usRegHoldingBuf[V_MAX_OFF] ) )  SaveReg8(LOW_VOLTAGE_OFF,byte_data);
                  break;
             case V_MAX_OFF:
                 if ((byte_data > usRegHoldingBuf[V_MIN_OFF] ) && (byte_data < usRegHoldingBuf[V_MAX_ON] ) )  SaveReg8(HIGH_VOLTAGE_ON,byte_data);
                 break;
             case V_MAX_ON:
                 if ( byte_data > usRegHoldingBuf[V_MAX_OFF] ) SaveReg8(HIGH_VOLTAGE_OFF,byte_data);
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
                      SaveReg8(COMMON_REGS_MAP[adress], byte_data);
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
             case IP_PORT_MB:
                 if (WORK_MODE)
                 {
                     saveReg16(COMMON_REGS_MAP[adress], usRegHoldingBuf[adress]);
                 }
                 break;
             case CONTR_MB:
             case TIME_SENS_MB:
             case TIME_OUT_MB:
                 SaveReg8(COMMON_REGS_MAP[adress],byte_data);
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
   else
   {
       u16 byte_data =(u16)usRegHoldingBuf[adress-device_specific_reg_offset[dev_type]];
       u16 reg_addr;
       switch ((DEVICE_TYPE_t)getReg8(DEVICE_TYPE))
       {
                  case DEV_FMCH:
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
                                      data = convert_int_to_float( &usRegHoldingBuf[adress-1]);
                                      saveRegFloat(reg_addr, data);
                                      break;
                           case  SET_MOD1_MB:
                           case  SET_MOD2_MB:
                           case  FILTER_LOW_MB:
                           case  FILTER_HIGH_MB:
                                      saveReg16(reg_addr, usRegHoldingBuf[adress]);
                                      break;
                           case TIME_FAN_STOP_MB:
                                      SaveReg8(reg_addr,byte_data);
                                      break;
                       }
                       break;
                  case DEV_CAV_VAV_BP:

                      reg_addr = CDV_REGS_MAP[adress- 200];
                      switch (adress)
                      {
                            case (CDV_KOOF_I_MB+1):
                            case (CDV_KOOF_K_MP+1):
                            case (CDV_KOOF_P_MB+1):
                            case (CDV_KOOF_I1_MB+1):
                            case (CDV_KOOF_P1_MB+1):
                            case (CDV_F_CHANNEL+1):
                                                data = convert_int_to_float( &usRegHoldingBuf[adress-device_specific_reg_offset[dev_type]-1]);
                                                saveRegFloat(reg_addr, data);
                                                break;
                            case (CDV_OFFSET_CH2+1):
                            case (CDV_SETTING_MIN_MB+1 ):
                            case (CDV_SETTING_MID_MB +1 ):
                            case (CDV_SETTING_MAX_MB +1 ):
                            case (CDV_SETTING_ERROR1_MB + 1):
                            case (CDV_SETTING_ERROR2_MB + 1):
                                     data = convert_int_to_float( &usRegHoldingBuf[adress-device_specific_reg_offset[dev_type]-1]);
                                     switch ( getReg8(MEASERING_UNIT) )
                                     {
                                            case 0:
                                                data = DataModel_SetLToPressere(data);
                                                break;
                                            case 1:
                                                data = DataModel_SetVToPressere(data);
                                                break;
                                             default:
                                                break;
                                      }
                                      saveReg16( reg_addr , (u16)data);
                                      break;
                             case CDV_MEASERING_UNIT:
                             case CDV_CH_COUNT_MB:
                                     SaveReg8(reg_addr, (uint8_t) byte_data);
                                 break;
                             case CDV_MODE_CONTROL:

                                                 VerifyAndSetReg8(reg_addr, (uint8_t) byte_data );
                                                 break;
                            case CDV_CLEAN_TIMER:
                            case CDV_SETTING_TIMEOUT_MB:

                                                SaveReg8(reg_addr,byte_data);
                                                break;


                            case CDV_ZERO_POINT_TIMEOUT:
                                         saveReg16(reg_addr, usRegHoldingBuf[adress]);
                                         break;

                                                     }
                      break;


                  default:

                      break;
       }
    }



}



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
    }
}

void UpdateDCVInputs()
{
    float temp_float;
    temp_float  =  DataModelGetCDVSettings(getAIN(SENS1));
    convert_float_to_int((float)temp_float, &usRegHoldingBuf[CDV_FACT_1-CDV_OFFSET]);
    temp_float  =  DataModelGetCDVSettings(getAIN(SENS2));
    convert_float_to_int((float)temp_float, &usRegHoldingBuf[CDV_FACT_2-CDV_OFFSET]);
    usRegInputBuf[CDV_CUR_STATE  - CDV_OFFSET ] = getStateDCV();

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
    }
    else                                              //§¦§ã§Ý§Ú §á§Ö§â§Ó§í§Û §Ñ§Õ§â§Ö§ã §Ó §Õ§Ú§Ñ§á§Ñ§Ù§à§ß§Ö §ã§á§Ö§è§Ú§æ§Ú§é§Ö§ã§Ü§Ú§ç §Õ§Ý§ñ §å§ã§ä§â§à§Û§ã§ä§Ó§Ñ §â§Ö§Ô§ã§Ú§ä§â§à§Ó
    {
       if ((DEVICE_TYPE_t)getReg8(DEVICE_TYPE) == DEV_FMCH )
       {
            UodateFMCHInputs();
       }
       else
       {
           UpdateDCVInputs();
       }
    }
}
#define CDV_BP_REG8_SEQ_COUNT 11
#define CDV_BP_REG_SEQ_COUNT 1

#define REG_SEQ_COUNT 5
#define REG8_SEQ_COUNT 13


static const u8 CDV_BP_REGS8[CDV_BP_REG8_SEQ_COUNT]={CDV_MODE_CONTROL,CDV_SETTING_TIMEOUT_MB,CDV_AFZONE_SETTING_MB,CDV_CH_COUNT_MB ,CDV_MEASERING_UNIT,CDV_PRIOR_SENS,CDV_CLEAN_TIMER,CDV_KK_SENSOR_TYPE,CDV_CO2_SENSOR_TYPE,CDV_H_SENSOR_TYPE,INPUT_SENSOR_TYPE_MB};
static const u8 CDV_BP_REGS[CDV_BP_REG_SEQ_COUNT]={CDV_ZERO_POINT_TIMEOUT};

static const u8 REGS8[REG8_SEQ_COUNT]={
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

static const u8 REGS[REG_SEQ_COUNT]={SET_MOD1_MB,SET_MOD2_MB,FILTER_LOW_MB,FILTER_HIGH_MB};




void UpdateFMCHHoldings()
{
    int32_t tempdata;
    for (u8 i=0;i<4;i++)
       usRegHoldingBuf[REGS[i]]      = getReg16(FMCH_REGS_MAP[REGS[i]- 100]);
   usRegHoldingBuf[COMMAND_REG] = getReg8(SYSTEM_START);
   usRegHoldingBuf[MODE_REG_MB ] = getReg8(MODE);
   usRegHoldingBuf[LIGTH_REG_MB] = getReg8(LIGTH);
   usRegHoldingBuf[TIME_FAN_STOP_MB] = getReg8(FAN_START_TIMEOUT);
   tempdata =(int32_t) (getRegFloat(COOF_P)*1000);
   convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[KOOF_P_MB]);
   tempdata =(int32_t) (getRegFloat(COOF_I)*1000);
   convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[KOOF_I_MB]);
   tempdata =(int32_t) (getRegFloat(KOOFKPS)*1000);
   convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[KOOF_K_MP]);
   if  (usRegHoldingBuf[JOURNAL_SELECT_MB] > getReg16(RECORD_COUNT))
            usRegHoldingBuf[JOURNAL_SELECT_MB] = getReg16(RECORD_COUNT);

}

static const u8 SettingRegsMap[]={CDV_OFFSET_CH2,CDV_SETTING_MIN_MB,CDV_SETTING_MID_MB,CDV_SETTING_MAX_MB,CDV_SETTING_ERROR1_MB,CDV_SETTING_ERROR2_MB};

void MB_TASK_HOLDING_UDATE( u16 start_reg_index )
{
    int32_t tempdata;
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
                 usRegHoldingBuf[REGS8[i]]      = getReg8(COMMON_REGS_MAP[REGS8[i]]);

    }
    else
    {
        u16 reg_offet;
        float temp_float;
        switch ((DEVICE_TYPE_t)getReg8(DEVICE_TYPE))
        {
            case DEV_CAV_VAV_BP:
                  reg_offet  =   200;
                  tempdata =(int32_t) (getRegFloat(COOF_P)*1000);
                  convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[CDV_KOOF_P_MB-CDV_OFFSET]);
                  tempdata =(int32_t) (getRegFloat(COOF_I)*1000);
                  convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[CDV_KOOF_I_MB-CDV_OFFSET]);
                  tempdata =(int32_t) (getRegFloat(KOOFKPS)*1000);
                  convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[CDV_KOOF_K_MP-CDV_OFFSET]);
                  tempdata =(int32_t) (getRegFloat(COOF_P1)*1000);
                  convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[CDV_KOOF_P1_MB-CDV_OFFSET]);
                  tempdata =(int32_t) (getRegFloat(COOF_I1)*1000);
                  convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[CDV_KOOF_I1_MB-CDV_OFFSET]);
                  tempdata =(int32_t) (getRegFloat(COOF_I1)*1000);
                  convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[CDV_F_CHANNEL-CDV_OFFSET]);
                  tempdata =(int32_t) (getRegFloat(F_CHANNEL)*1000);
                   convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[CDV_F_CHANNEL-CDV_OFFSET]);

                  for (u8 i = 0; i < 6; i++)
                  {
                      temp_float  =  DataModelGetCDVSettings(getReg16(CDV_REGS_MAP[SettingRegsMap[i]-reg_offet]));
                      convert_float_to_int((float)temp_float, &usRegHoldingBuf[SettingRegsMap[i]-CDV_OFFSET]);
                  }

                  for (u8 i=0;i<CDV_BP_REG8_SEQ_COUNT;i++)                                      //§©§Ñ§á§à§Ý§ß§ñ§Ö§Þ  8 §Ò§Ú§ä§ß§í§Ö §â§Ö§Ô§Ú§ã§ä§â§í §ã§á§Ö
                  {
                       usRegHoldingBuf[CDV_BP_REGS8[i] -CDV_OFFSET ]      = getReg8(CDV_REGS_MAP[CDV_BP_REGS8[i] -reg_offet]);
                  }
                  for (u8 i=0;i<CDV_BP_REG_SEQ_COUNT;i++)                                      //§©§Ñ§á§à§Ý§ß§ñ§Ö§Þ  16 §Ò§Ú§ä§ß§í§Ö §â§Ö§Ô§Ú§ã§ä§â§í §ã§á§Ö
                  {
                       usRegHoldingBuf[CDV_BP_REGS[i]  -CDV_OFFSET  ]      = getReg16(CDV_REGS_MAP[CDV_BP_REGS[i] -reg_offet]);
                  }
                  break;
            case DEV_FMCH:
                UpdateFMCHHoldings();
                break;
            }

    }

}


static u16 const  device_specific_input_count[] =  { FMCH_INPUTS_COUNT  ,CDV_INPUTS_COUNT , BP_INPUTS_COUNT  };


eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int             iRegIndex;
  u16 reg_offet = device_specific_reg_offset[getReg8(DEVICE_TYPE)];
  u16 reg_count = device_specific_input_count[getReg8(DEVICE_TYPE)];

  if ((( usAddress >= REG_INPUT_START ) && ( usAddress + usNRegs <= REG_INPUT_START + COMMON_REG_INPUT_NREGS ) )

      ||  ( ( usAddress >= 100 + reg_offet ) && ( (usAddress + usNRegs) <= (reg_offet+ 100 + reg_count+1) ) ))
  {
    iRegIndex = ( int )( usAddress - usRegInputStart );
    MB_TASK_INPUTS_UDATE(  iRegIndex  );
    u16 offset=0;
    while( usNRegs > 0 )
    {
        if (iRegIndex >=(100+ reg_offet)) offset = reg_offet; else offset = 0;
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
  u16 reg_offet = device_specific_reg_offset[getReg8(DEVICE_TYPE)];
  u16 reg_count = device_specific_reg_count[getReg8(DEVICE_TYPE)];

  if (( ( usAddress >= REG_HOLDING_START ) && ( usAddress + usNRegs <= COMMON_REG_COUNT ) ) ||  ( ( usAddress >= 100 + reg_offet ) && ( (usAddress + usNRegs) <= (reg_offet+ 100 + reg_count+1) ) ))
  {
    iRegIndex = ( int )( usAddress - usRegHoldingStart );
    switch ( eMode )
    {
    case MB_REG_READ:

      MB_TASK_HOLDING_UDATE(  iRegIndex  );
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
        USHORT          iRegBitIndex;
        UCHAR *         pucCoilBuf  = ucSCoilBuf;
        USHORT          usCoilStart = REG_COILS_START;
        /* it already plus one in modbus function method. */
        usAddress--;

        if( ( usAddress >= REG_COILS_START) &&  ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_NREGS ) )
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
