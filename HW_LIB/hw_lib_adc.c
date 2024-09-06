/*
 * hw_lib_adc.c
 *
 *  Created on: 9 апр. 2024 г.
 *      Author: i.dymov
 */

#include "hw_lib_adc.h"
#include "system_init.h"
#include "drivers_config.h"


static AIN_DATA_t xAinData[AIN_NUMBER] 							__SECTION(RAM_SECTION_CCMRAM);
static AIN_DATA_t xDacData[DAC_NUMBER];
static uint16_t muRawDacData[DAC_NUMBER]                      __SECTION(RAM_SECTION_CCMRAM);
static uint16_t muRawDacVData[DAC_NUMBER]                   __SECTION(RAM_SECTION_CCMRAM);
static uint16_t usDacMaxIndex;

static LIN_COOF   xKoofData[ MAX_COOF_COUNT] 					__SECTION(RAM_SECTION_CCMRAM);
static LIN_COOF   xDacKoofData[ MAX_COOF_COUNT]                    __SECTION(RAM_SECTION_CCMRAM);
static uint16_t usCurMaxIndex 									__SECTION(RAM_SECTION_CCMRAM);
#ifdef IPS_OUTS
	static uint16_t muRawCurData[OUT_COUNT] 					__SECTION(RAM_SECTION_CCMRAM);
	static uint16_t muRawOldOutCurData[OUT_COUNT] 				__SECTION(RAM_SECTION_CCMRAM);
#endif
static uint16_t muRawVData[AIN_NUMBER + 2]  					__SECTION(RAM_SECTION_CCMRAM);
static uint16_t muRawOldVData[AIN_NUMBER + 2]					__SECTION(RAM_SECTION_CCMRAM);
int16_t            ADC1_IN_Buffer[ADC_FRAME_SIZE*ADC1_CHANNELS] __SECTION(RAM_SECTION_RAM );  //ADC1 input data buffer
int16_t            ADC2_IN_Buffer[ADC_FRAME_SIZE*ADC2_CHANNELS] __SECTION(RAM_SECTION_RAM );   //ADC2 input data buffer
int16_t            ADC3_IN_Buffer[ADC_FRAME_SIZE*ADC3_CHANNELS] __SECTION(RAM_SECTION_RAM );

void vAINInit()
{
    for (uint8_t i = 0; i < AIN_NUMBER; i++)
    {
        xAinData[i].coof_count = 0U;
        xAinData[i].index = 0U;
    }
#ifdef IPS_OUTS
    for (uint8_t i = 0; i < OUT_COUNT; i++)
    {
    	muRawOldOutCurData[i]  = 0;
    	muRawCurData[i] 		= 0;
    }
#endif
    for (uint8_t i = 0; i< AIN_NUMBER + 2;i++)
    {
    	muRawVData[i] 	 = 0;
    	muRawOldVData[i] = 0;
     }
    usCurMaxIndex = 0;
    return;
}

void vDacInit()
{
    for (uint8_t i = 0; i < DAC_NUMBER; i++)
        {
            xDacData[i].coof_count = 0U;
            xDacData[i].index = 0U;
        }

        for (uint8_t i = 0; i< DAC_NUMBER;i++)
        {
            muRawDacData[i]    = 0;
            muRawDacVData[i] = 0;
         }
        usDacMaxIndex = 0;
        return;
}

#ifdef IPS_OUTS
uint16_t ucGetRawData( uint8_t ch, uint8_t filter)
{
	if (filter)
	{
		muRawCurData[ch] = vRCFilter(muRawCurData[ch],&muRawOldOutCurData[ch]);
	}
	return (muRawCurData[ch]);
}
#endif

/*
 * Функция преобразования данных аналогово канала по клаиборвочым коофициентам
 */
static float fConvertCalData( AIN_NAME_t name, float in_data )
{
    float out_data = 0;
    for (uint16_t i =  xAinData[ name ].index; i < (xAinData[ name ].index + xAinData[ name ].coof_count ); i++ )
    {
        if ( (i ==  (xAinData[ name ].index + xAinData[ name ].coof_count)) || (in_data <= xKoofData[i].data ) )
        {
            out_data = xKoofData[i].k * in_data + xKoofData[i].b;
        }
    }
    return ( out_data );
}
/*
 *
 */
float fGetAinCalData( AIN_NAME_t name, float raw_data)
{
     float out_data = 0U;
     if ( name < AIN_NUMBER )
     {
          if ( xAinData[ name ].coof_count == 0 )
           {
               out_data = raw_data;
           }
           else
           {
               out_data = fConvertCalData( name, raw_data);
           }
     }
     return ( out_data );
}






CAL_ERROR_CODE  eAinCalDataConfig(AIN_NAME_t name, uint8_t cal_point_count )
{
    CAL_ERROR_CODE res = CAL_SUCCESS;
    if (cal_point_count == 1)
    {
        res = CAL_POINT_COUNT_ERROR;
    }
    else
    {
        if (  xAinData[ name ].coof_count == 0 )
        {
            if  ( (cal_point_count +  usCurMaxIndex) > MAX_TOTAL_CAL_POINT )
            {
                res = CAL_MEMORY_FULL;
            }
            else
            {
                xAinData[ name ].coof_count = cal_point_count;
                xAinData[ name ].index = usCurMaxIndex;
                usCurMaxIndex += cal_point_count;
            }
        }
        else
        {
                if ( xAinData[ name ].coof_count != ( cal_point_count- 1))
                {
                    res = CAL_OVERWRITE_ERROR;
                }
        }
    }
    return ( res );
}

/*
 * Настройка количества калибровочных точек для DAC
 */
CAL_ERROR_CODE  eDacCalDataConfig(DAC_NAME_t name, uint8_t cal_point_count )
{
    CAL_ERROR_CODE res = CAL_SUCCESS;
    if (cal_point_count == 1)
    {
        res = CAL_POINT_COUNT_ERROR;
    }
    else
    {
        if (  xDacData[ name ].coof_count == 0 )  //Если создаем новые калиброчные точки
        {
            if  ( (cal_point_count +  usDacMaxIndex) > MAX_DAC_TOTAL_CAL_POINT ) //Смотрим, можем ли бы их размеситить в памяти
            {
                res = CAL_MEMORY_FULL;
            }
            else
            {
                xDacData[ name ].coof_count = cal_point_count-1;
                xDacData[ name ].index = usCurMaxIndex;
                usCurMaxIndex += cal_point_count-1;
            }
        }
        else
        {
                if ( xDacData[ name ].coof_count != ( cal_point_count- 1)) //Если перезаписываем, то кол-во точен не должно меняться
                {
                    res = CAL_OVERWRITE_ERROR;
                }
        }
    }
    return ( res );
}


CAL_ERROR_CODE  eSetDacCalPoint(DAC_NAME_t name, POINT_t * cal_point, uint16_t PointNumber )
{
    CAL_ERROR_CODE res = CAL_SUCCESS;

    if ( PointNumber <= xDacData[ name ].coof_count + 1)
    {
        for (uint8_t i = 0; i < (PointNumber-1); i++)
        {
            uint16_t index = i + xDacData[ name ].index;
            vABLineKoofFinde ( &xDacKoofData[index].k, &xDacKoofData[index].b,
                                                cal_point[i ].X, cal_point[ i+1 ].X, cal_point[i].Y,cal_point[i+1].Y);
            xDacKoofData[index].data = cal_point[ i ].X;
        }

    }
    else
    {
        res = CAL_OVERWRITE_ERROR;
    }
    return (res);
}

/*
 * Функция преобразования данных аналогово канала по клаиборвочым коофициентам
 */
static float fDacConvertCalData( DAC_NAME_t name, float in_data )
{
    float out_data = 0;
    for (uint16_t i =  (xDacData[ name ].index + xDacData[ name ].coof_count -1  ); i >= (xDacData[ name ].index  ); i--)
    {
        if ( (i ==  xDacData[ name ].index ) || (in_data >= xDacKoofData[i].data ) )
        {
            out_data = xDacKoofData[i].k * in_data + xDacKoofData[i].b;
            break;
        }
    }
    return ( out_data );
}




float fGetDacCalData( DAC_NAME_t name, float raw_data)
{
     float out_data = 0U;
     if ( name < DAC_NUMBER )
     {
          if ( xDacData[ name ].coof_count == 0 )
           {
               out_data = raw_data;
           }
           else
           {
               out_data = fDacConvertCalData( name, raw_data);
           }
     }
     return ( out_data );
}
/*
 *
 */
CAL_ERROR_CODE  eSetAinCalPoint(AIN_NAME_t name, POINT_t * cal_point, uint16_t PointNumber )
{
	CAL_ERROR_CODE res = CAL_SUCCESS;

	if ( PointNumber < xAinData[ name ].coof_count)
	{
		uint16_t index = PointNumber + xAinData[ name ].index;
		vABLineKoofFinde ( &xKoofData[index].k, &xKoofData[index].b,
									cal_point[0].X, cal_point[ 1 ].X, cal_point[0].Y,cal_point[1].Y);
		xKoofData[index].data = cal_point[ 1 ].X;
	}
	else
	{
		res = CAL_OVERWRITE_ERROR;
	}
	return (res);
}

/*
 *
 */
void vABLineKoofFinde(float * k, float * b,  float x1, float x2, float y1, float y2)
{
		 *k = (float)( y1 - y2 ) /(float) (x1 - x2);
		 *b = (float)(y1*x2 - y2*x1) / (float) (x2-x1);
		 return;
}

/*
 *
 */

/* Напряжение на аналогвом входе
*/
float fAinGetState ( AIN_NAME_t channel )
{

 float res = fGetAinCalData( channel , (float) muRawVData[channel] *  AINCOOF2 );

return res;// ( (channel < AIN_COUNT) ? (float) muRawVData[channel] *  AINCOOF1 : 0U ) ;
}
/*
*
*/
float fBatteryGet ( void )
{
	 return (float)muRawVData[4] * AINCOOF1 + INDIOD;
}
/*
*
*/

float fTemperatureGet ( uint8_t i )
{
	 return ((float)muRawVData[3] * K - 0.7782) /0.0024 + 28;	//	HAL_GetTempSens( muRawVData[4])	;
}

 float fGetDataFromRaw( float fraw, LIN_COOF *  CSC)
 {
 	float fRes;
 	for (uint8_t r = 0; r < ( KOOF_COUNT -1 ); r++)
     {
 		if (( fraw < CSC[r].data ) || (r == (KOOF_COUNT -2)) )
 		{
 			fRes =  fraw * CSC[r].k;
 			fRes += CSC[r].b ;
 			fRes *= fraw/RR;
 			break;
 		}
 	 }
 	return ( fRes );
 }

float usGetCurrentToFloat( uint16_t rawdata, LIN_COOF * xOut )
{

   return fGetDataFromRaw( ((float) rawdata *K ) , xOut );
}

static void vGetAverDataFromRAW(uint16_t * InData, uint16_t *OutData, uint8_t InIndex, uint8_t OutIndex, uint8_t Size, uint16_t BufferSize);

int16_t * getADC1Buffer()
{
	return (ADC1_IN_Buffer);
}
int16_t * getADC2Buffer()
{
	return (ADC2_IN_Buffer);
}
int16_t * getADC3Buffer()
{
	return (ADC3_IN_Buffer);
}

void vInitADCDATA()
{
#ifdef IPS_OUTS
	 for (int i = 0; i< OUT_COUNT;i++)
	   {
		   muRawOldOutCurData[i] = 0;
	   }
#endif
	   for (int i=0; i< AIN_NUMBER + 2;i++ )
	   {
		   muRawOldVData[i] = 0;
	   }
}

#ifdef IPS_OUTS
void vDataConvertToFloat( void)
{
	 // Полчени из буфера ADC 1 данныех каналов каналов тока 7-8
	 vGetAverDataFromRAW((uint16_t *)&ADC1_IN_Buffer, (uint16_t *)&muRawCurData, 0U, 6U, 2U , ADC1_CHANNELS);
	 // Полчени из буфера ADC 1 данныех каналов каналов тока 19-20
	 vGetAverDataFromRAW((uint16_t *)&ADC1_IN_Buffer, (uint16_t *)&muRawCurData, 2U, 18U, 2U , ADC1_CHANNELS);
	 // Полчени из буфера ADC 1 данныех каналов каналов AIN
	 vGetAverDataFromRAW((uint16_t *)&ADC1_IN_Buffer, (uint16_t *)&muRawVData, 4U, 0U, 5U , ADC1_CHANNELS);
	 // Полчени из буфера ADC 2 данныех каналов каналов тока 4-6
	 vGetAverDataFromRAW((uint16_t *)&ADC2_IN_Buffer, (uint16_t *)&muRawCurData,0U, 3U, 3U , ADC2_CHANNELS);
	 // Полчени из буфера ADC 2 данныех каналов каналов тока 9-12
	 vGetAverDataFromRAW((uint16_t *)&ADC2_IN_Buffer, (uint16_t *)&muRawCurData, 3U, 8U, 4U , ADC2_CHANNELS);
	 // Полчени из буфера ADC 3 данныех каналов каналов тока 1-3
	 vGetAverDataFromRAW((uint16_t *)&ADC3_IN_Buffer, (uint16_t *)&muRawCurData, 0U, 0U, 3U , ADC3_CHANNELS);
	 // Полчени из буфера ADC 3 данныех каналов каналов тока 13-18
	 vGetAverDataFromRAW((uint16_t *)&ADC3_IN_Buffer, (uint16_t *)&muRawCurData, 3U, 12U, 6U , ADC3_CHANNELS);
	 for (int i = 0; i<AIN_NUMBER + 2;i++ )
	 {
		 muRawVData[i] = vRCFilter(  muRawVData[i] ,&muRawOldVData[i]);
	 }
	return;
}
#endif


/*
 * Функция вытаскивает из входного буфера Indata  (размером FrameSize*BufferSize) со смещением InIndex FrameSize отсчетов,
 * счетает среднее арефмитическое и записывает в буффер OutData со смещением OutIndex
 */
static void vGetAverDataFromRAW(uint16_t * InData, uint16_t *OutData, uint8_t InIndex, uint8_t OutIndex, uint8_t Size, uint16_t BufferSize)
{
	volatile uint32_t temp;
	for (uint8_t i=0; i<Size; i++ )
	{
		temp = 0;
		for (uint8_t j=0;j < ADC_FRAME_SIZE; j++ )
		{
		  temp += (InData[ InIndex + i + j * BufferSize ]);
		}
		OutData[ OutIndex + i ] = temp / ADC_FRAME_SIZE;

	}
	return;
}


