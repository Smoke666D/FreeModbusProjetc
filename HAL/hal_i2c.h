/*
 * hal_i2c.h
 *
 *  Created on: 20 апр. 2024 г.
 *      Author: i.dymov
 */

#ifndef HAL_HAL_I2C_H_
#define HAL_HAL_I2C_H_

#include "main.h"
#include "system_init.h"
#include "hal_config.h"



#if MCU == APM32
#include "apm32f4xx_i2c.h"

#endif


typedef enum
{
  I2C_IDLE   		= 0,
  I2C_MASTER_RECIVE_START  = 1,
  I2C_MASTER_RECIVE_WRITE_ADDR =2,
  I2C_MASTER_RECIVE_DA_WRITE = 3,
  I2C_MASTER_RECIVE_ADDR = 4,
  I2C_MASTER_RECIVE_MODE = 5,
  I2C_MASTER_RECIEVE 	 = 6,
  I2C_MASTER_TRANSMIT_START  = 7,
  I2C_MASTER_TRANSMIT_ADDR = 8,
  I2C_MASTER_TRANSMIT_NEXT  = 9,
  I2C_MASTER_TRANSMIT_LAST = 10,

} I2C_STATE_t;

#define MODE_DMA 1
#define MODE_IT  2

#define MODE_I2C  MODE_IT

typedef struct
{
  uint32_t I2C_ClockSpeed;          /* Specifies the clock frequency.
                                       This parameter must be set to a value lower than 400kHz */

  uint16_t I2C_Mode;                /* Specifies the I2C mode.
                                       This parameter can be a value of @ref I2C_mode */

  uint16_t I2C_DutyCycle;           /* Specifies the I2C fast mode duty cycle.
                                       This parameter can be a value of @ref I2C_duty_cycle_in_fast_mode */

  uint16_t I2C_OwnAddress1;         /* Specifies the first device own address.
                                       This parameter can be a 7-bit or 10-bit address. */

  uint16_t I2C_Ack;                 /* Enables or disables the acknowledgement.
                                       This parameter can be a value of @ref I2C_acknowledgement */

  uint16_t I2C_AcknowledgedAddress; /* Specifies if 7-bit or 10-bit address is acknowledged.
                                       This parameter can be a value of @ref I2C_acknowledged_address */
}HAL_I2C_InitTypeDef;



#if MCU == APM32
#define I2C_1  I2C1
#define I2C_2  I2C2
#define  I2C_NAME_t I2C_T*
#endif
#if MCU == CH32V2 || MCU == CH32V3
typedef enum
      {
    I2C_1  =0,
    I2C_2  =1,
      }
 I2C_NAME_t;

#define STAR1_SB_FLAG     0x0001
#define STAR1_ADDR_FLAG   0x0002
#define STAR1_BTF_FLAG    0x0004
#define STAR1_TXE_FLAG    0x0080
#define STAR2_BUSY_FLAG   0x0002

#endif


 typedef enum {
     HAL_I2C_OK,
     HAL_I2C_ERROR,
 } I2C_ERROR_CODE_t;


 typedef struct
 {
    uint8_t ucTaskNatificationIndex;
    uint8_t BusyFlag;
    uint8_t DataLength;
    uint8_t Index;
    uint8_t ADDR[2];
    uint8_t DevAdrres;
    I2C_STATE_t I2C_State;
    uint16_t data_address;
    uint8_t * ReciveBuffer;
    I2C_NAME_t dev;
    TaskHandle_t NotifyTaskHeandle;
  //  EERPOM_ERROR_CODE_t (*I2C_Master_Recive_func) (  u8 , u16,  u8 * , u16 , u32 ,u8 );
  //  EERPOM_ERROR_CODE_t (*I2C_Master_Transmit_func)( u8 , u16,  u8 * , u16 , u32 ,u8 );
 } I2C_TypeDef_T;

 I2C_ERROR_CODE_t I2C_Master_TransmitIT( I2C_NAME_t i2c,  u8 DevAdrees, u16 data_addres,  u8 * data, u16 data_size, u32 timeout,uint8_t TNI  );
void HAL_I2C_ENABLE( I2C_NAME_t i2c )  ;
 void HAL_I2C_InitIT( I2C_NAME_t i2c, HAL_I2C_InitTypeDef * I2C_InitStruct, uint8_t prior, uint8_t subprior);
void I2C1_EV_IRQHandler( void );
void I2C1_ER_IRQHandler ( void );
void I2C2_EV_IRQHandler( void );
void I2C2_ER_IRQHandler ( void );
void DMA1_STR3_IRQHandler( void );
void DMA1_STR7_IRQHandler( void );

#endif /* HAL_HAL_I2C_H_ */
