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
#define REG_HOLDING_NREGS 51

static USHORT usRegInputStart = REG_INPUT_START;
static USHORT usRegInputBuf[REG_INPUT_NREGS];
static USHORT usRegHoldingStart = REG_HOLDING_START;
static USHORT usRegHoldingBuf[REG_HOLDING_NREGS];

#define MODE_MB          0
#define KOOF_P_MB        1
#define KOOF_I_MB        3
#define KOOF_K_MP        5
#define CONTROL_TYPE_MB  7
#define PROTOCOL_TYPE_MB 8
#define TIME_H_MB        9
#define TIME_M_MB        10
#define TIME_S_MB        11
#define DATE_D_MB        12
#define DATE_M_MB        13
#define DATE_Y_MB        14
#define V_MIN_ON         15
#define V_MIN_OFF        16
#define V_MAX_ON         17
#define V_MAX_OFF        18
#define CONTR_MB         19
#define MB_ADDRES_MB     20
#define TIME_OUT_MB      21
#define IP_1_MB          22
#define IP_2_MB          23
#define IP_3_MB          24
#define IP_4_MB          25
#define GATE_1_MB        26
#define GATE_2_MB        27
#define GATE_3_MB        28
#define GATE_4_MB        29
#define MASK_1_MB        30
#define MASK_2_MB        31
#define MASK_3_MB        32
#define MASK_4_MB        33
#define IP_PORT_MB       34
#define SET_MOD1_MB      35
#define SET_MOD2_MB      36
#define FILTER_LOW_MB    37
#define FILTER_HIGH_MB   38
#define TIME_SENS_MB     39
#define TIME_FAN_STOP_MB 40
#define COMMAND_REG      41
#define LIGTH_REG_MB     42
#define MODE_REG_MB      43
#define ZERO_MB          44


#define INP_MODE_MB      24
#define INP_MH_H_MB      25
#define INP_MH_M_MB      26
#define INP_STATE_MB     27
#define ERROR_STATE_MB   28
#define AOUT1_MB         29
#define AOUT2_MB         31
#define AOUT3_MB         33

#define DAC1_ADDR  53
#define DAC2_ADDR  55
#define DAC3_ADDR  57
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






static u8 getRegID( u8 mb_reg_address)
{
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
        case (KOOF_P_MB+1):     return  (COOF_P);
        case (KOOF_I_MB+1):     return  (COOF_I);
        case (KOOF_K_MP+1):     return  (KOOFKPS);
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
   u16   ref;
   u16 byte_data =(u16)usRegHoldingBuf[adress];
   u8 reg_addr = getRegID(adress);
   switch (adress)
   {
       case ZERO_MB:
           if  (byte_data !=0) CalibrateZeroStart();
           break;
       case LIGTH_REG_MB:
       case MODE_REG_MB:
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
               WORK_MODE = byte_data ;
               switch (WORK_MODE)
               {
                   case 1:
                       break;
                   case 2:
                       break;
                   case 3:
                       break;
               }
           }
           break;
       case DAC1_ADDR+1:
          data = *((float *)&usRegHoldingBuf[adress-1]);
          ref = (uint16_t)fGetDacCalData(DAC1,data);
          HAL_TIMER_SetPWMPulse(TIMER9,TIM_CHANNEL_1 ,ref *10);
          HAL_TIMER_EnablePWMCH(TIMER9);
          break;
       case DAC2_ADDR+1:
           data = *((float *)&usRegHoldingBuf[adress-1]);
           ref = (uint16_t)fGetDacCalData(DAC2,data);
           HAL_TIMER_SetPWMPulse(TIMER9,TIM_CHANNEL_2 ,ref *10 );
           HAL_TIMER_EnablePWMCH(TIMER9);
          break;
       case DAC3_ADDR+1:
           data = *((float *)&usRegHoldingBuf[adress-1]);
           ref = (uint16_t)fGetDacCalData(DAC3,data);
           HAL_TIMER_SetPWMPulse(TIMER9,TIM_CHANNEL_3 ,ref *10 );
           HAL_TIMER_EnablePWMCH(TIMER9);
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
           saveReg16(reg_addr, usRegHoldingBuf[adress]);
           break;
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
#define REG_SEQ_COUNT 5
#define REG8_SEQ_COUNT 12
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


void MB_TASK_HOLDING_UDATE()
{
    static HAL_TimeConfig_T time;
    static HAL_DateConfig_T date;
    int32_t tempdata;
    usRegHoldingBuf[MODE_MB] = WORK_MODE;
    tempdata =(int32_t) (getRegFloat(COOF_P)*1000);

    convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[KOOF_P_MB]);

   // *((float*) (usRegHoldingBuf+KOOF_P_MB )) =  (float)tempdata/1000.0;
    tempdata =(int32_t) (getRegFloat(COOF_I)*1000);
    convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[KOOF_I_MB]);
   // *((float*) (usRegHoldingBuf+KOOF_I_MB )) =  (float)tempdata/1000.0;
    tempdata =(int32_t) (getRegFloat(KOOFKPS)*1000);
  //  *((float*) (usRegHoldingBuf+KOOF_K_MP)) =  (float)tempdata/1000.0;
    convert_float_to_int((float)tempdata/1000.0, &usRegHoldingBuf[KOOF_K_MP]);


    HAL_RTC_ReadTime(&time);
    HAL_RTC_ReadDate(&date);
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

  if( ( usAddress >= REG_HOLDING_START ) && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
  {
    iRegIndex = ( int )( usAddress - usRegHoldingStart );
    switch ( eMode )
    {
    case MB_REG_READ:
      MB_TASK_HOLDING_UDATE();
      while( usNRegs > 0 )
      {
        *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] >> 8 );
        *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] & 0xFF );
        iRegIndex++;
        usNRegs--;
      }
      break;

    case MB_REG_WRITE:
       while( usNRegs > 0 )
      {
        usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
        usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
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
            iRegBitIndex = (USHORT) (usAddress - usCoilStart) % 8;
            switch ( eMode )
            {
            /* read current coil values from the protocol stack. */
            case MB_REG_READ:
                //vUPDATECoils(0);
                xGetOut(ucSCoilBuf);
                while (iNReg > 0)
                {
                    *pucRegBuffer++ = xMBUtilGetBits(&pucCoilBuf[iRegIndex++],
                            iRegBitIndex, 8);
                    iNReg--;
                }
                pucRegBuffer--;
                /* last coils */
                usNCoils = usNCoils % 8;
                /* filling zero to high bit */
                *pucRegBuffer = *pucRegBuffer << (8 - usNCoils);
                *pucRegBuffer = *pucRegBuffer >> (8 - usNCoils);

                break;

                /* write current coil values with new values from the protocol stack. */
            case MB_REG_WRITE:
                while (iNReg > 1)
                {
                    xMBUtilSetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, 8,
                            *pucRegBuffer++);
                    iNReg--;
                }
                /* last coils */
                usNCoils = usNCoils % 8;
                /* xMBUtilSetBits has bug when ucNBits is zero */
                if (usNCoils != 0)
                {
                    xMBUtilSetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, usNCoils,
                            *pucRegBuffer++);
                }
                xSetOut( ucSCoilBuf );
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
     vSetRegData(DAC1_ADDR+1);
     vSetRegData(DAC2_ADDR+1);
     vSetRegData(DAC3_ADDR+1);
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
     vSetRegData(DAC1_ADDR+1);
     vSetRegData(DAC2_ADDR+1);
     vSetRegData(DAC3_ADDR+1);
     for( ;; )
     {
         if (getReg8(MB_PROTOCOL_TYPE)== MKV_MB_RTU)
         {

             if (eMBInit(MB_RTU,1,HAL_USART4,19200,MB_PAR_ODD ) == MB_ENOERR )
             {
                mb_ready = 1;
             }
         }
         else
         if( eMBTCPInit( MB_TCP_PORT_USE_DEFAULT ) == MB_ENOERR )
        {
             printf("tcp\r\n");
            mb_ready = 1;
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
