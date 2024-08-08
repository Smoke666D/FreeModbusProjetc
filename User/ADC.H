/*
 * ADC.H
 *
 *  Created on: Feb 5, 2024
 *      Author: i.dymov
 */

#ifndef USER_ADC_H_
#define USER_ADC_H_

#include "main.h"

#define DC_CHANNEL 7
#define DC_CONVERSION_NUMBER 3
#define AC_CONVERION_NUMBER 1500
#define ADC_CHANNEL 2
#define R2   75
#define R1   470

#define ADC1_DATA_READY 0x04
#define ADC2_DATA_READY 0x02

typedef enum
{
  SENS1 = 0,
  SENS2 = 1,
  DC24  = 2,
  DCAIN1  = 3,
  DCAIN2  = 4,
  DCAIN3  = 5,
  DCAIN4  = 6,
  DIG_TEMP = 7,
  DIG_PRES = 8,
  AC220   = 9,
} AIN_CHANNEL_t;

typedef struct
{
    uint16_t * pBuff;
    uint16_t ConversionalSize;
    uint16_t pIndex;
} ADC_Conversionl_Buf_t;

#define KK  ( 3.3 / 4095.0 )

#define R1_24V 1000000.0
#define R2_24V  150000.0
#define COOF_24V ((R1_24V+R2_24V)/R2_24V)
#define R1_10V 56000.0
#define R2_10V  10000.0
#define COOF_10V ((R1_10V+R2_10V)/R2_10V)


TaskHandle_t * getADCTaskHandle();
void ADC_TestInit();
void ADC_task(void *pvParameters);
void ADC1_Init();
float getAIN( AIN_CHANNEL_t channel);
uint16_t GetTempData();
#endif /* USER_ADC_H_ */
