/*
 * hal_typedef.h
 *
 *  Created on: Nov 4, 2024
 *      Author: smoke
 */

#ifndef HAL_TYPEDEF_H_
#define HAL_TYPEDEF_H_

#include "main.h"

#define HAL_SET   1
#define HAL_RESET 0


#if CORE == APM32
#include "apm32f4xx_gpio.h"
#endif
#if CORE == WCH32V2
#include "ch32v20x_gpio.h"
#endif
#if CORE == WCH32V3

#include "ch32v30x_gpio.h"
#endif

#define  MAX_GPIO_PORT 7

#if CORE == APM32

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

#if CORE == WCH32V2 || CORE==WCH32V3
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
#if CORE == APM32
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
#if CORE == WCH32V2 || CORE==WCH32V3
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


#endif /* HAL_TYPEDEF_H_ */
