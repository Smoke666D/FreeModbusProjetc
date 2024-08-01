/*
 * hw_lib_din.c
 *
 *  Created on: 9 апр. 2024 г.
 *      Author: i.dymov
 */

#include "hw_lib_din.h"
#include "system_init.h"
#include "hal_timers.h"

static DinConfig_t 				 xDinConfig[ DIN_COUNT] __SECTION(RAM_SECTION_CCMRAM);
static median_filter_data_t      RPM_MIDIAN_FILTER_STRUC[RPM_CHANNEL_COUNT];
static aver_filter_data_t        RPM_AVER_FILTER_STRUC  [RPM_CHANNEL_COUNT];
static RPM_Struct                RPM[RPM_CHANNEL_COUNT] __SECTION(RAM_SECTION_CCMRAM);;
static DoutCinfig_t              xDoutConfig[DOUT_COUNT] __SECTION(RAM_SECTION_CCMRAM);;




void InitDinStcurt()
{
	for (uint8_t i = 0; i <DIN_COUNT; i++)
	{
		xDinConfig[i].eInputType = DIN_IDLE;
	}
}

DIN_INPUT_TYPE xGetDinType(DIN_INPUT_NAME ucCh)
{
	return (xDinConfig[ucCh].eInputType);
}

void vRecinfigDin(  DIN_INPUT_NAME ucCh, DinConfig_t * config    )
{
	xDinConfig[ucCh].ulHighCounter = config->ulHighCounter;
	xDinConfig[ucCh].ulLowCounter  = config->ulLowCounter;
	xDinConfig[ucCh].ulCounter = 0;
	if (xDinConfig[ucCh].eInputType != config->eInputType)
	{
		xDinConfig[ucCh].eInputType = config->eInputType;
		xDinConfig[ucCh].ucValue = (xDinConfig[ucCh].ucValue == 1) ? 0 : 1U;
		xDinConfig[ucCh].ucTempValue = xDinConfig[ucCh].ucValue;
	}
}


void InitFilters()
{
	for (uint8_t i = 0; i < RPM_CHANNEL_COUNT; i++)
	{
		vInitMedianFilter(&RPM_MIDIAN_FILTER_STRUC[i]);
		vInitRunAverga(&RPM_AVER_FILTER_STRUC[i],0.5);
	}
}
/*
 *  DOUT
 */
void vSetRPMConfig(uint8_t ch, float coof, float filter_coof)
{

	xDinConfig[ch].RPMDATA->config_coof = coof;

	if ((filter_coof!=0) && (filter_coof<=1.0))
	{
		vInitRunAverga(&RPM_AVER_FILTER_STRUC[(ch == INPUT_9) ? 0 : 1], filter_coof);
	}

}



DIN_FUNCTION_ERROR_t eDOUTConfigWtihStruct(OUT_NAME_TYPE ucCh, DoutCinfig_t * config)
{
    DIN_FUNCTION_ERROR_t eRes = DIN_CONFIG_OK;
#if DIN_PARAM_CHECK == 1
    if ( ucCh <DOUT_COUNT)
    {
#endif

        xDoutConfig[ucCh].ucValue = 0;
        xDoutConfig[ucCh].setPortCallback = config->setPortCallback;

#if DIN_PARAM_CHECK == 1
    }
    else {

        Res  = DIN_WRONG_CHANNEL_NUMBER ;
    }
#endif
    return ( eRes );
}

DIN_FUNCTION_ERROR_t eSetDUT(OUT_NAME_TYPE ucCh, uint8_t state )
{
    DIN_FUNCTION_ERROR_t eRes = DIN_CONFIG_OK ;
    #if DIN_PARAM_CHECK == 1
        if ( ucCh <DOUT_COUNT)
        {
    #endif

            xDoutConfig[ucCh].ucValue = state;

    #if DIN_PARAM_CHECK == 1
        }
        else {

              Res  = DIN_WRONG_CHANNEL_NUMBER ;
          }
    #endif
        return ( eRes );
}
/*
 *
 */
uint8_t eGrtDUT(OUT_NAME_TYPE ucCh)
{
    #if DIN_PARAM_CHECK == 1
        if ( ucCh <DOUT_COUNT)
        {
    #endif
          return ( xDoutConfig[ucCh].ucValue);
    #if DIN_PARAM_CHECK == 1
        }
        else {

              return RESET ;
          }
    #endif
}
/*
 *
 */
DIN_FUNCTION_ERROR_t eDinConfigWtihStruct(DIN_INPUT_NAME ucCh, DinConfig_t * config)
{
    DIN_FUNCTION_ERROR_t eRes = DIN_WRONG_CHANNEL_NUMBER ;
    xDinConfig[ucCh].eInputType = config->eInputType;
    xDinConfig[ucCh].ucValue    = (xDinConfig[ucCh].eInputType == DIN_CONFIG_POSITIVE ) ? 0U : 1U;
    xDinConfig[ucCh].ulHighCounter = config->ulHighCounter;
    xDinConfig[ucCh].ulLowCounter = config->ulLowCounter;
    xDinConfig[ucCh].getPortCallback = config->getPortCallback;
    xDinConfig[ucCh].ucTempValue = xDinConfig[ucCh].ucValue;
    xDinConfig[ucCh].ulCounter = 0;
    eRes = DIN_CONFIG_OK;
    return ( eRes );

}

/*
 * Функция возващает текущие значение дискрентого вохода
 */
uint8_t uGetDIN(DIN_INPUT_NAME ucCh)
{
    return (xDinConfig[ucCh].ucValue);
}

DIN_FUNCTION_ERROR_t xGetDIN(DIN_INPUT_NAME ucCh, uint8_t * state)
{
    if ( xDinConfig[ucCh].ucValue != xDinConfig[ucCh].ucReadBit )
    {
        *state =   xDinConfig[ucCh].ucValue;
        xDinConfig[ucCh].ucReadBit = xDinConfig[ucCh].ucValue;
        return (DIN_CHANGE);
    }
    return (DIN_NOT_CHANGE);

}

/*
 *
 */
u16 * uGetRPMBuffer(DIN_INPUT_NAME ucCh)
{
    return (xDinConfig[ucCh].RPMDATA->Data);
}

/*
 *
 */
DIN_FUNCTION_ERROR_t eRPMConfig(DIN_INPUT_NAME ucCh, uint8_t ucRPM)
{
    DIN_FUNCTION_ERROR_t eRes = DIN_WRONG_CHANNEL_NUMBER;
#if DIN_PARAM_CHECK == 1
    if (( ucCh <DIN_COUNT) && ( ucRPM < RPM_CHANNEL_COUNT ))
    {
#endif
       xDinConfig[ucCh].eInputType = RPM_CONFIG;
       xDinConfig[ucCh].RPMDATA = &RPM[ucRPM];
       xDinConfig[ucCh].ucValue = 0;
       xDinConfig[ucCh].ucReadBit = 0;
       xDinConfig[ucCh].ucTempValue = 0;
       RPM[ucRPM].bValidFlag = RPM_INVALID;
       memset(RPM[ucRPM ].Data,0,CC_BUFFER_SIZE);
       RPM[ucRPM].BufferData = 0;
       RPM[ucRPM].config_coof = 1;
#if DIN_PARAM_CHECK == 1
    }
#endif
    return ( eRes );
}
/*
 *
 */
void vAddRPMData(DIN_INPUT_NAME ucCh, uint16_t data  )
{
	u8 Index = xDinConfig[ucCh].ucTempValue;
	xDinConfig[ucCh].RPMDATA->Data[Index] = data;
	if (++xDinConfig[ucCh].ucTempValue >= CC_BUFFER_SIZE)
	{
		xDinConfig[ucCh].ucTempValue = 0;
		RMPDataConvert(ucCh);
	}
	xDinConfig[ucCh].RPMDATA->bValidFlag =  RPM_VALID;
}



void RMPDataConvert(DIN_INPUT_NAME ucCh)
{
    int32_t u16temp;
    xDinConfig[ucCh].RPMDATA->BufferData = 0;
    for (u8 i=1;i< CC_BUFFER_SIZE;i++)
    {
       u16temp = xDinConfig[ucCh].RPMDATA->Data[i] -xDinConfig[ucCh].RPMDATA->Data[i-1];
       if (u16temp <0 )
       {
           u16temp = ( CC_PERIOD -  xDinConfig[ucCh].RPMDATA->Data[i-1]) + xDinConfig[ucCh].RPMDATA->Data[i];
       }
       xDinConfig[ucCh].RPMDATA->BufferData =+ u16temp;
    }
    xDinConfig[ucCh].RPMDATA->BufferData = (xDinConfig[ucCh].RPMDATA->BufferData/CC_BUFFER_SIZE)<<1;
    xDinConfig[ucCh].RPMDATA->BufferData =  MedianFilter(xDinConfig[ucCh].RPMDATA->BufferData,&RPM_MIDIAN_FILTER_STRUC[ucCh]);
    xDinConfig[ucCh].RPMDATA->BufferData =  RunAvrageFilter(xDinConfig[ucCh].RPMDATA->BufferData,&RPM_AVER_FILTER_STRUC[ucCh] );
    xDinConfig[ucCh].RPMDATA->bValidFlag =  RPM_VALID;
}

/*
 *
 */
static void vCheckRPM( DIN_INPUT_NAME ucCh )
{
    xDinConfig[ucCh].ucValue++;
   if (xDinConfig[ucCh].ucValue  ==100)
   {
       xDinConfig[ucCh].ucValue  = 0;
       if (xDinConfig[ucCh].RPMDATA->bValidFlag == RPM_VALID)
       {
           xDinConfig[ucCh].RPMDATA->bValidFlag = RPM_INVALID;
       }
       else
       {
           xDinConfig[ucCh].RPMDATA->BufferData  = 0;
       }
   }
   return;
}

/*
 *
 */
uint16_t GetRPM( DIN_INPUT_NAME ucCh )
{
    uint16_t usTemp = 0U;
    if (( xDinConfig[ucCh].RPMDATA != 0) && (xDinConfig[ucCh].eInputType==RPM_CONFIG))
    {
    	if (xDinConfig[ucCh].RPMDATA->BufferData!= 0)
    	{
    		usTemp =(uint16_t)(1.0/ (xDinConfig[ucCh].RPMDATA->BufferData * 0.0001 ))*60;
    		usTemp = usTemp* xDinConfig[ucCh].RPMDATA->config_coof;
    	}
    }
    return (  usTemp  );
}
/*
 *
 */
DIN_FUNCTION_ERROR_t xGetRPM( DIN_INPUT_NAME ucCh, uint16_t * data)
{

    if (xDinConfig[ucCh].RPMDATA->BufferData != xDinConfig[ucCh].ulHighCounter)
    {
        *data =(uint16_t)(1.0/ (xDinConfig[ucCh].RPMDATA->BufferData * 0.0001 ))*60;
        *data = (*data)* xDinConfig[ucCh].RPMDATA->config_coof;
        xDinConfig[ucCh].ulHighCounter = xDinConfig[ucCh].RPMDATA->BufferData;
        return (DIN_CHANGE);
    }
    return (DIN_NOT_CHANGE);
}
/*
 *
 */
void vDinInitStateProcess()
{

	for (uint8_t i = 0; i <DIN_COUNT; i++)
	    {
	         if ( xDinConfig[i].eInputType != RPM_CONFIG )
	         {
	             uint8_t ucDinState =  xDinConfig[i].getPortCallback(i);
	             if (ucDinState != xDinConfig[i].ucTempValue )
	             {
	                  xDinConfig[i].ucValue     = ucDinState ^ ( (~xDinConfig[i].eInputType) & 0x1);
	                  xDinConfig[i].ucTempValue = ucDinState ;

	             }
	         }
	     }
}
/*
 *
 */
void vDinDoutProcess()
{
    for (uint8_t i = 0; i <DIN_COUNT; i++)
    {
         if ( xDinConfig[i].eInputType != RPM_CONFIG )
         {
             uint8_t ucDinState =  xDinConfig[i].getPortCallback(i);
             if (ucDinState != xDinConfig[i].ucTempValue )
             {
                   if (++xDinConfig[i].ulCounter > ( (xDinConfig[i].ucTempValue == RESET) ? xDinConfig[i].ulHighCounter : xDinConfig[i].ulLowCounter ) )
                   {
                        xDinConfig[i].ucValue     = ucDinState ^ ( (~xDinConfig[i].eInputType) & 0x1);
                        xDinConfig[i].ucTempValue = ucDinState ;
                   }
             }
             else
                 xDinConfig[i].ulCounter = 0U;
         }
         else
         {
             vCheckRPM(i);
         }
     }
    for (uint8_t i=0; i < DOUT_COUNT; i++)
    {
        xDoutConfig[i].setPortCallback(i, xDoutConfig[i].ucValue);
    }
}
/*
 *   Возвращает состония порта
*/
uint8_t ucDinGet( DIN_INPUT_NAME eChNum )
{
#if DIN_PARAM_CHECK == 1
    if ( ucCh >= DIN_COUNT)
     return (RESET);
#endif
    return (xDinConfig[eChNum].ucValue);

}
/*
 *  Функция, запаковывает текущие состония входов в 32-битное слово.
 */
uint32_t uiGetDinMask()
{
	uint32_t uiMask = 0;
	for (int8_t i = (DIN_COUNT -1);  i > -1 ; i--)
	{
		uiMask <<=1;
		uiMask |= ( xDinConfig[ i ].ucValue & 0x01 );
	}
	return ( uiMask );
}
