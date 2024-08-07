/*
 * hw_lib_adc.h
 *
 *  Created on: 9 апр. 2024 г.
 *      Author: i.dymov
 */

#ifndef HW_LIB_HW_LIB_ADC_H_
#define HW_LIB_HW_LIB_ADC_H_


#include "main.h"
#include "drivers_config.h"
#include "filters.h"


#define R3  1500.0

#define K   ( 3.3 / 0xFFF )

#define RA1  60400.0
#define RA2  3000.0
#define RA3  10000.0
#define AINCOOF1  ( ( RA1 + RA3 ) /RA3) * K
#define AINCOOF2  ( ( RA3 + RA3 ) /RA3) * K


#define INDIOD  0.45


#define MAX_CAL_POINT  20
#define MAX_TOTAL_CAL_POINT  ( AIN_NUMBER * MAX_CAL_POINT  ) //Суммароне количество калибровочных точек
#define MAX_DAC_TOTAL_CAL_POINT  ( DAC_NUMBER * MAX_CAL_POINT  ) //Суммароне количество калибровочных точек
#define MAX_COOF_COUNT       ( AIN_NUMBER * ( MAX_CAL_POINT -1 ) )

/*Коофиценты для кривых */
typedef struct
{
    float data;
    float k;
    float b;
}   LIN_COOF;

typedef enum {
  AIN1 = 0,
  AIN2 = 1,
  AIN3 = 2,
  AIN4 = 3,
  AIN5 = 4,
  AIN6 = 5,
  AIN7 = 6,
  AIN8 = 7,
  AIN9 = 8,
  AIN10 = 9,
  AIN11 = 10,
  AIN12 = 11,
  AIN13 = 12,
  AIN14 = 13,
  AIN15 = 14,
  AIN16 = 15,
  AIN17 = 16,
} AIN_NAME_t;


typedef enum {
  DAC1 = 0,
  DAC2 = 1,
  DAC3 = 2,
  DAC4 = 3,
  DAC5 = 4,
  DAC6 = 5,
  DAC7 = 6,
  DAC8 = 7,
  DAC9 = 8,
  DAC10 = 9,
} DAC_NAME_t;

typedef struct
{
    uint16_t KOOF;
    float Data;
} KAL_DATA;


typedef struct
{
    float Y;
    float X;
} POINT_t;

typedef  struct
{
    uint8_t     coof_count;
    uint8_t     index;

} AIN_DATA_t;

#define KOOF_COUNT 4



typedef enum {
 CAL_SUCCESS= 0,
 CAL_MEMORY_FULL = 1,
 CAL_OVERWRITE_ERROR = 2,
 CAL_POINT_COUNT_ERROR = 3
} CAL_ERROR_CODE;

float fAinGetState ( AIN_NAME_t channel );
float fBatteryGet ( void );
float fTemperatureGet ( uint8_t i );
CAL_ERROR_CODE  eAinCalDataConfig(AIN_NAME_t name, uint8_t cal_point_count );
CAL_ERROR_CODE  eSetAinCalPoint(AIN_NAME_t name, POINT_t * cal_point, uint16_t PointNumber );
CAL_ERROR_CODE  eSetAinCal(AIN_NAME_t name, POINT_t * cal_point, uint8_t cal_point_count);
void vABLineKoofFinde(float * k, float * b,  float x1, float x2, float y1, float y2);
float fGetAinCalData( AIN_NAME_t name, float raw_data);
void vAINInit();

#ifdef IPS_OUTS
	uint16_t ucGetRawData( uint8_t ch, uint8_t filter);
	void vDataConvertToFloat( void);
#endif
float usGetCurrentToFloat( uint16_t rawdata, LIN_COOF * xOut );

/*
 */
//#define RR  300.0
#define ADC_FRAME_SIZE     3U

//float fGetDataFromRaw( float fraw, LIN_COOF *  CSC);
uint16_t usGetRawCurDataFilterd( u8 ch);
int16_t * getADC1Buffer();
int16_t * getADC2Buffer();
int16_t * getADC3Buffer();

void vDacInit();
float fGetDacCalData( DAC_NAME_t name, float raw_data);
CAL_ERROR_CODE  eSetDacCalPoint(DAC_NAME_t name, POINT_t * cal_point, uint16_t PointNumber );
CAL_ERROR_CODE  eDacCalDataConfig(DAC_NAME_t name, uint8_t cal_point_count );

#endif /* HW_LIB_HW_LIB_ADC_H_ */
