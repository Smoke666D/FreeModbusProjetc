/*
 * hal_config.h
 *
 *  Created on: 8 灞戦偑瑜� 2024 璋�.
 *      Author: i.dymov
 */

#ifndef HAL_HAL_CONFIG_H_
#define HAL_HAL_CONFIG_H_


/*
 * MCU Define
 */

#define APM32       0
#define CH32V2      1
#define CH32V3      2

#include "board_define.h"
#if MCU == APM32
#include "apm32f4xx_gpio.h"
#endif
#if MCU == CH32V2
#include "ch32v20x_gpio.h"
#endif
#if MCU == CH32V3

#include "ch32v30x_gpio.h"
#endif

#define  MAX_GPIO_PORT 7

#if MCU == APM32

#define   PORT_A  GPIOA
#define   PORT_B  GPIOB
#define   PORT_C  GPIOC
#define   PORT_D  GPIOD
#define   PORT_E  GPIOE
#define   PORT_F  GPIOF
#define   PORT_G  GPIOG
#define   PORT_H  GPIOH
#define   PortName_t   GPIO_T*
#endif

#if MCU == CH32V2 || MCU==CH32V3
#define   PORT_A  GPIOA
#define   PORT_B  GPIOB
#define   PORT_C  GPIOC
#define   PORT_D  GPIOD
#define   PORT_E  GPIOE
#define   PORT_F  GPIOF
#define   PORT_G  GPIOG
#define   PortName_t   GPIO_TypeDef *
#endif

typedef enum
{
#if MCU == APM32
    GPIO_0 = GPIO_PIN_0,
	GPIO_1 = GPIO_PIN_1,
	GPIO_2 = GPIO_PIN_2,
	GPIO_3 = GPIO_PIN_3,
	GPIO_4 = GPIO_PIN_4,
	GPIO_5 = GPIO_PIN_5,
	GPIO_6 = GPIO_PIN_6,
	GPIO_7 = GPIO_PIN_7,
	GPIO_8 = GPIO_PIN_8,
	GPIO_9 = GPIO_PIN_9,
	GPIO_10 = GPIO_PIN_10,
	GPIO_11 = GPIO_PIN_11,
	GPIO_12 = GPIO_PIN_12,
	GPIO_13 = GPIO_PIN_13,
	GPIO_14 = GPIO_PIN_14,
	GPIO_15 = GPIO_PIN_15,
	GPIO_ALL = GPIO_PIN_ALL
#endif
#if MCU == CH32V2 || MCU==CH32V3
	 GPIO_0 = GPIO_Pin_0,
	 GPIO_1 = GPIO_Pin_1,
	 GPIO_2 = GPIO_Pin_2,
	 GPIO_3 = GPIO_Pin_3,
	 GPIO_4 = GPIO_Pin_4,
	 GPIO_5 = GPIO_Pin_5,
	 GPIO_6 = GPIO_Pin_6,
	 GPIO_7 = GPIO_Pin_7,
	 GPIO_8 = GPIO_Pin_8,
	 GPIO_9 = GPIO_Pin_9,
	 GPIO_10 = GPIO_Pin_10,
	 GPIO_11 = GPIO_Pin_11,
	 GPIO_12 = GPIO_Pin_12,
	 GPIO_13 = GPIO_Pin_13,
	 GPIO_14 = GPIO_Pin_14,
	 GPIO_15 = GPIO_Pin_15,
	 GPIO_ALL = GPIO_Pin_All

#endif
} PinName_t;

#define HAL_SET   1
#define HAL_RESET 0

#define DMA1_CH1_ENABLE 1
#define DMA1_CH2_ENABLE 1
#define DMA1_CH3_ENABLE 0
#define DMA1_CH4_ENABLE 0
#define DMA1_CH5_ENABLE 0
#define DMA1_CH6_ENABLE 0
#define DMA1_CH7_ENABLE 0
#define DMA2_CH1_ENABLE 0
#define DMA2_CH2_ENABLE 0
#define DMA2_CH3_ENABLE 0
#define DMA2_CH4_ENABLE 0
#define DMA2_CH5_ENABLE 0
#define DMA2_CH6_ENABLE 0
#define DMA2_CH7_ENABLE 0


#define ADC_1_ENABLE 1
#define ADC_2_ENABLE 1
#define ADC_1_IT_ENABLE 0
#define ADC_2_IT_ENABLE 1

#endif /* HAL_HAL_CONFIG_H_ */
