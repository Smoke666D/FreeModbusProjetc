/*
 * hal_gpio.h
 *
 *  Created on: 9 апр. 2024 г.
 *      Author: i.dymov
 */

#ifndef HAL_HAL_GPIO_H_
#define HAL_HAL_GPIO_H_

#include "hal_config.h"
#include "main.h"

#if MCU == APM32
#define TIMER1_PWM_AF   GPIO_AF_TMR1
#define TIMER2_PWM_AF   GPIO_AF_TMR2
#define TIMER3_PWM_AF   GPIO_AF_TMR3
#define TIMER4_PWM_AF   GPIO_AF_TMR4
#define TIMER8_PWM_AF   GPIO_AF_TMR8
#define TIMER12_PWM_AF  GPIO_AF_TMR12
#define TIMER10_AF      GPIO_AF_TMR10
#define TIMER9_AF       GPIO_AF_TMR9
#endif

typedef enum
{
  GPIO_IN,
  GPIO_OUT_NOPP,
  GPIO_OUT_PUP,
  GPIO_OUT_PDOWN,
} PortType_t;


typedef enum
{
#if MCU == APM32
  HAL_BIT_RESET = BIT_RESET,
  HAL_BIT_SET   = BIT_SET,
#endif
#if MCU == CH32V2 || MCU == CH32V3
  HAL_BIT_RESET = Bit_RESET,
  HAL_BIT_SET   = Bit_SET,
#endif
} BitState_t;



typedef enum
{
#if MCU == APM32
	MODE_OUT_PP = GPIO_OTYPE_PP,   /*!< GPIO push-pull mode */
	MODE_OUT_OD = GPIO_OTYPE_OD,
#endif
#if MCU == CH32V2 || MCU == CH32V3
	MODE_OUT_PP =  GPIO_Mode_AF_PP,   /*!< GPIO push-pull mode */
	MODE_OUT_OD =  GPIO_Mode_AF_OD ,
	MODE_IN_PU  =  GPIO_Mode_IPU,
	MODE_IN_PD  =  GPIO_Mode_IPD
#endif
} GPIO_MODE_t;

void HAL_InitGpioIn(PortName_t PORT, uint16_t Pin);
void HAL_InitGpioAF(PortName_t PORT, uint16_t Pin, uint32_t AF , GPIO_MODE_t mode );
void HAL_InitGpioInPUP(PortName_t PORT, uint16_t Pin);
void HAL_InitGpioOut( PortName_t PORT, uint16_t Pin  );
void HAL_SetBit(  PortName_t  port, uint16_t pin );
void HAL_ResetBit(  PortName_t  port, uint16_t pin );
void HAL_InitGpioAIN(PortName_t PORT, uint16_t Pin );
BitState_t HAL_GetBit( PortName_t  port, uint16_t pin);
BitState_t xHAL_GetOutBit( PortName_t  port, uint16_t pin);

#endif /* HAL_HAL_GPIO_H_ */
