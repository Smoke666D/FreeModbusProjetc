/*
 * ADC.H
 *
 *  Created on: Feb 5, 2024
 *      Author: i.dymov
 */

#ifndef USER_ADC_H_
#define USER_ADC_H_

#include "main.h"

#define DC_CHANNEL 4
#define DC_CONVERSION_NUMBER 3
#define AC_CONVERION_NUMBER 1500
#define ADC_CHANNEL 2
#define R2   75
#define R1   470

#define ADC1_DATA_READY 0x04
#define ADC2_DATA_READY 0x02


TaskHandle_t * getADCTaskHandle();
void ADC_TestInit();
void ADC_task(void *pvParameters);

uint16_t GetTempData();
#endif /* USER_ADC_H_ */
