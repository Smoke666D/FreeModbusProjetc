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
  DC24  = 3,
  DCAIN1  = 4,
  DCAIN2  = 5,
  DCAIN3  = 6,
  DCAIN4  = 7,
  AC220   = 8,
} AIN_CHANNEL_t;
#define K   ( 3.3 / 0xFFF )

#define R1_24V 1000000
#define R2_24V  150000
#define COOF_24V ((R1_24V+R2_24V)/R2_24V)

TaskHandle_t * getADCTaskHandle();
void ADC_TestInit();
void ADC_task(void *pvParameters);
void ADC1_Init();
uint16_t GetTempData();
#endif /* USER_ADC_H_ */
