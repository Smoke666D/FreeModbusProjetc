/*
 * ADC.H
 *
 *  Created on: Feb 5, 2024
 *      Author: i.dymov
 */

#ifndef USER_ADC_H_
#define USER_ADC_H_

#include "main.h"


#define I2C_SENSOR_1          0
#define I2C_SENSOR_2          1
#define I2C_SENSOR_CNT        2
#define ADC1_CH_COUNT         2
#define DC_CHANNEL            7
#define DC_CONVERSION_NUMBER  3
#define AC_CONVERION_NUMBER   1500
#define ADC_CHANNEL           2
#define R2                    75
#define R1                    470
#define CALIB_COUNT           10
#define ADC1_DATA_READY       0x04
#define ADC2_DATA_READY       0x02
#define DC_24_BufferSize      3
#define DC_AIN_BufferSize     10
#define Sens_BufferSize_MAX   400
#define SENOR_MAX_DATA        1000
#define AIN1_ERROR            0x01
#define AIN2_ERROR            0x02
#define AIN3_ERROR            0x04
#define MIN_BAR_VALUE         -2500
#define MAX_BAR_VALUE         2500
#define MIN_VDD_VALUE         0.177
#define MAX_VDD_VALUE         2.7
#define SICLE_I2C             100
#define VDD_RANGE             (MAX_VDD_VALUE - MIN_VDD_VALUE)
#define DELTA                 ((MAX_VDD_VALUE -MIN_VDD_VALUE )/(MIN_BAR_VALUE -  MAX_BAR_VALUE))

//#define NEW_PRESS_SENS


#ifdef NEW_PRESS_SENS

#define SENSOR_ADDR 0x58
#define CMD_REG_ADDR 0x01
#define STATUS_MASK 0x20
#define DATA_READY_MASK STATUS_MASK
#define START_COMMAND 0x01
#define DALA_MSB 0x04
#define DATA_CSB 0x05
#define DATA_LSB 0X06
#define TEMM_MSB 0x07
#define TEMP_LSB 0x08
#define TEMP_COOF_MSB 0x20
#define TEMP_COOF_LSB 0x21
#define BYTE1_INDEX 8
#define BYTE2_INDEX 9
#define PMAX  2500.0
#define PMIN  -2500.0

#else
#define SENSOR_ADDR 0x6D
#define CMD_REG_ADDR 0x30
#define STATUS_MASK 0x08
#define DATA_READY_MASK 0
#define START_COMMAND 0x0A
#define DALA_MSB 0x06
#define DATA_CSB 0x07
#define DATA_LSB 0X08
#define TEMM_MSB 0x09
#define TEMP_LSB 0x0A
#endif


typedef enum
{
    SENSOR_START_CONVERSION = 0,
    SENSOR_GET_STATUS = 1,
    SENSOR_GET_PERS_1 = 2,
    SENSOR_GET_PERS_2 = 3,
    SENSOR_GET_PERS_3 = 4,
    SENSOR_GET_TEMP_1 = 5,
    SENSOR_GET_TEMP_2 = 6,
    SENSOR_IDLE = 7,
    SENSOR_TIME_OUT = 8,
    SENSOR_READ_TEMP_COOF_1 = 9,
    SENSOR_READ_TEMP_COOF_2 = 10,
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
  DCAIN5  = 7,
  DIG_TEMP = 8,
  DIG_PRES = 9,
  AC220   = 10,
  DIG2_PRES =11,
  DIG2_TEMP =12,
  AC220_CONTROL = 13,
  EXT5 = 14,

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
#define R1_5V  20000.0
#define R2_5V  30000.0
#define COOF_5V ((R1_5V+R2_5V)/R2_5V)
#define R1_10V 56000.0
#define R2_10V  10000.0
#define RTC_COOF  10000
#define COOF_10V ((R1_10V+R2_10V)/R2_10V)
#define COOF_NTC ((R1_10V+R2_10V)/R2_10V)

SemaphoreHandle_t * xGetADCSemaphore();
u8 CalibrationZeroWhait();
void vSetCount( u16 coount);
void CalibrateZeroStart();
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
