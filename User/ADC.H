/*
 * ADC.H
 *
 *  Created on: Feb 5, 2024
 *      Author: i.dymov
 */

#ifndef USER_ADC_H_
#define USER_ADC_H_

#include "main.h"

#define DC_CHANNEL 6
#define DC_CONVERSION_NUMBER 3
#define AC_CONVERION_NUMBER 1500
#define ADC_CHANNEL 2
#define R2   75
#define R1   470

#define ADC1_DATA_READY 0x04
#define ADC2_DATA_READY 0x02

typedef enum
{
  SENSOR_START_CONVERSION = 0,
  SENSOR_GET_STATUS       = 1,
  SENSOR_GET_PERS_1       = 2,
  SENSOR_GET_PERS_2       = 3,
  SENSOR_GET_PERS_3       = 4,
  SENSOR_GET_TEMP_1       = 5,
  SENSOR_GET_TEMP_2       = 6,
  SENSOR_IDLE             = 7,
} SENSOR_FSM_t;

typedef enum
{
   I2C_GET_BUSY      = 0,
   I2C_EVT_5         = 1,
   I2C_EVT_5_R      = 10,
   I2C_EVT_6        =   2,

   I2C_EVT8    = 3,
   I2C_START_READ    = 4,
   I2C_SEND_ADDR_R   = 5,
   I2C_CHECK_ACK_R   = 6,
   I2C_EVT_6_1   = 7,
   I2C_EVT_8_2    = 8,

} I2C_FSM_t;

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
  DIG2_PRES =10,
  DIG2_TEMP =11,

} AIN_CHANNEL_t;

typedef struct
{
    int16_t * pBuff;
    uint16_t ConversionalSize;
    uint16_t ViewDataCount;
    uint16_t pIndex;
    int16_t offset;
} ADC_Conversionl_Buf_t;



#define KK  ( 3.3 / 4095.0 )

#define R1_24V 1000000.0
#define R2_24V  150000.0
#define COOF_24V ((R1_24V+R2_24V)/R2_24V)
#define R1_10V 56000.0
#define R2_10V  10000.0
#define COOF_10V ((R1_10V+R2_10V)/R2_10V)

void vSetCount( u16 coount);
void CalibrateZeroStart();
uint8_t getACVoltage();
void I2C_task(void *pvParameters);
TaskHandle_t * getI2CTaskHandle();
void vSetPeriod( u8  per);
u8 uGetPeriod();
u16 uGetConversionCount();
TaskHandle_t * getADCTaskHandle();
void ADC_TestInit();
void ADC_task(void *pvParameters);
void ADC1_Init();
float getAIN( AIN_CHANNEL_t channel);
uint16_t GetTempData();
void vDataBufferInit();
#endif /* USER_ADC_H_ */
