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
#define REG_INPUT_NREGS 10
#define REG_HOLDING_START 0x01


#define REG_HOLDING_START 0x01
#define REG_HOLDING_NREGS 40

static USHORT usRegInputStart = REG_INPUT_START;
static USHORT usRegInputBuf[REG_INPUT_NREGS];
static USHORT usRegHoldingStart = REG_HOLDING_START;
static USHORT usRegHoldingBuf[REG_HOLDING_NREGS];


#define DAC1_ADDR  20
#define DAC2_ADDR  22
#define DAC3_ADDR  24
#define SENS_COOF  18
#define DAC_DATA   19
#define SENS_PERIOD 28
#define SENS_COUNT  29
#define CONTR      30


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

void vSetRegData( u16 adress)
{
   float data;
   u16   ref;
   switch (adress)
   {
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
       case  SENS_COOF:
           sens_coof = usRegHoldingBuf[adress] ;
           break;
       case DAC_DATA :
           HAL_TIMER_SetPWMPulse(TIMER9,TIM_CHANNEL_1 ,usRegHoldingBuf[adress]);
           HAL_TIMER_EnablePWMCH(TIMER9);
           break;

           break;
       case SENS_COUNT:
           vSetCount(usRegHoldingBuf[adress]);
           break;
       case CONTR:
           DAC_SetChannel1Data(DAC_Align_12b_R, usRegHoldingBuf[adress]);
           break;

   }
}


eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int             iRegIndex;

  if( ( usAddress >= REG_INPUT_START ) && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
  {
    iRegIndex = ( int )( usAddress - usRegInputStart );
    while( usNRegs > 0 )
    {
      *pucRegBuffer++ =
        ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
      *pucRegBuffer++ =
        ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
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
  int32_t tempdata;
  if( ( usAddress >= REG_HOLDING_START ) && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
  {
    iRegIndex = ( int )( usAddress - usRegHoldingStart );
    switch ( eMode )
    {
    case MB_REG_READ:
        for (uint8_t i=0;i< DC_CHANNEL+3;i++)
        {
            tempdata =(int32_t) (getAIN(i)*1000);
            *((float*) (usRegHoldingBuf+i*2)) =  (float)tempdata/1000.0;
        }

        tempdata =(uint32_t) getAIN(AC220);
        *((float*) (usRegHoldingBuf+26)) =  tempdata;
        usRegHoldingBuf[18] = GetSensCoof();
        usRegHoldingBuf[29] = uGetConversionCount();
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
