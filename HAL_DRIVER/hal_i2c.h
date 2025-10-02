/*
 * hal_i2c.h
 *
 *  Created on: 20 апр. 2024 г.
 *      Author: i.dymov
 */

#ifndef HAL_HAL_I2C_H_
#define HAL_HAL_I2C_H_

#include "main.h"


#if CORE == APM32
#include "apm32f4xx_i2c.h"

#endif


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



#if CORE == APM32
#define I2C_1  I2C1
#define I2C_2  I2C2
#define  I2C_NAME_t I2C_T*
#endif
#if CORE == WCH32V2 || CORE == WCH32V3
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
#define STAR1_RXNE_FLAG    0x0040
#define STAR2_BUSY_FLAG   0x0002

#endif


 typedef enum {
     HAL_I2C_OK,
     HAL_I2C_ERROR,
 } I2C_ERROR_CODE_t;


 typedef struct
 {
   void (* datacallback) ( void );
   void (* errorcallback)(void );
 } HAL_I2C_t;




 uint16_t HAL_I2C_GET_STAR2(I2C_NAME_t i2c ) ;
 uint16_t HAL_I2C_GET_STAR1(I2C_NAME_t i2c ) ;
 void HAL_I2C_ENABLE( I2C_NAME_t i2c ) ;
 void HAL_I2C_DISABLE( I2C_NAME_t i2c ) ;
 void I2C_IT_ENABLE( I2C_NAME_t i2c , u16 DATA ) ;
 void I2C_IT_DISABLE(I2C_NAME_t i2c , u16 DATA );
 void HAL_I2C_STOP(I2C_NAME_t i2c )  ;
 void HAL_I2C_START(I2C_NAME_t i2c ) ;
 void HAL_I2C_SEND( I2C_NAME_t i2c, u8 DATA) ;
 void  HAL_I2C_SEND_ADDR_TRANS(I2C_NAME_t i2c,  u8 DATA);
 void  HAL_I2C_SEND_ADDR_RECIEVE(I2C_NAME_t i2c,  u8 DATA);
 void HAL_I2C_ACK_ENABLE(I2C_NAME_t i2c ) ;
 void HAL_I2C_ACK_DISABLE(I2C_NAME_t i2c ) ;
 void HAL_I2C_ClearFlag( I2C_NAME_t i2c , uint32_t flag);
 u16  HAL_I2C_READ_DATA(I2C_NAME_t i2c);

 void InitI2CIT( I2C_NAME_t i2c,HAL_I2C_InitTypeDef * I2C_InitStruct, uint8_t prior, uint8_t subprior, void (* fdata) ( void ), void (* ferror) ( void ));
uint16_t HAL_I2C_GET_STAT1(I2C_NAME_t i2c);
uint16_t HAL_I2C_GET_STAT2(I2C_NAME_t i2c);
void HAL_I2C_SEND(I2C_NAME_t i2c , u8 DATA);
void HAL_I2C_SEND_ADDR_TRANS(I2C_NAME_t i2c ,u8 DATA);
void HAL_I2C_SEND_ADDR_RECIEVE(I2C_NAME_t i2c , u8 DATA);
void HAL_I2C_STOP(I2C_NAME_t i2c ) ;
void HAL_I2C_START(I2C_NAME_t i2c );
uint8_t HAL_I2C_GET_DATA(I2C_NAME_t i2c);
void HAL_I2C_ACK_ENABLE(I2C_NAME_t i2c );
void HAL_I2C_ACK_DISABLE(I2C_NAME_t i2c );
//uint16_t HAL_GetI2CBusy( I2C_NAME_t i2c);
//I2C_ERROR_CODE_t I2C_Master_ReviceIT( I2C_NAME_t i2c, u8 DevAdrees, u16 data_addres,  u8 * data, u16 data_size, u32 timeout,uint8_t TNI  );
// I2C_ERROR_CODE_t I2C_Master_TransmitIT( I2C_NAME_t i2c,  u8 DevAdrees, u16 data_addres,  u8 * data, u16 data_size, u32 timeout,uint8_t TNI  );
void HAL_I2C_ENABLE( I2C_NAME_t i2c )  ;
 void HAL_I2C_Init( I2C_NAME_t i2c, HAL_I2C_InitTypeDef * I2C_InitStruct);
void I2C1_EV_IRQHandler( void );
void I2C1_ER_IRQHandler ( void );
void I2C2_EV_IRQHandler( void );
void I2C2_ER_IRQHandler ( void );


#endif /* HAL_HAL_I2C_H_ */
