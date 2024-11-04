/*
 * main.h
 *
 *  Created on: Feb 5, 2024
 *      Author: i.dymov
 */

#ifndef USER_MAIN_H_
#define USER_MAIN_H_

#include "string.h"
#include "eth_driver.h"
#include "debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "wchnet.h"
#include "port.h"
#include "mb.h"
#include "event_groups.h"
#include "ADC.H"
#include "u8g2.h"
EventGroupHandle_t * xGetADCEvent();
EventGroupHandle_t * xGetOSEvent();

#define DATA_READY  0x01
#define DATA_SEND   0x02
#define KEYBOARD_START 0x04


/****************************************************************/



#define WCH32V3   2
#define WCH32V2   1
/*
 *
 * §¬§à§æ§Ú§ß§å§Ñ§è§Ú§ñ §ñ§Õ§â§Ñ
 *
 */
#define CORE WCH32V3


#define __SECTION(x)

/****************************DMA*******************************/
#define DMA1_CH1_ENABLE
#define DMA1_CH2_ENABLE
//#define DMA1_CH3_ENABLE
#define DMA1_CH4_ENABLE
#define DMA1_CH5_ENABLE
//#define DMA1_CH6_ENABLE
//#define DMA1_CH7_ENABLE
//#define DMA2_CH1_ENABLE
//#define DMA2_CH2_ENABLE
//#define DMA2_CH3_ENABLE
//#define DMA2_CH4_ENABLE
//#define DMA2_CH5_ENABLE
//#define DMA2_CH6_ENABLE
//#define DMA2_CH7_ENABLE
/****************************ADC**********************************/
#define ADC_1_ENABLE
#define ADC_2_ENABLE
#define ADC_1_IT_ENABLE
#define ADC_2_IT_ENABLE
/***************************watchdog***************************/
//§¡§Ü§ä§Ú§Ó§Ú§â§à§Ó§Ñ§ä§î §Ó§Ñ§ä§é§Õ§à§Ô
#define WDT_ENABLE
/****************************RTC**************************/
#define RTC_IRQ_ENABLE
/***************************SPI*****************************/
//#define SPI1_IT_ENABLE
#define SPI2_IT_ENABLE
//#define SPI3_IT_ENABLE
/************************USART**********************************/
//#define USART1_IT_ENABLE
#define USART2_IT_ENABLE
//#define USART3_IT_ENABLE
#define USART4_IT_ENABLE
/************************I2C************************************/
#define I2C1_IT_ENABLE
#define I2C2_IT_ENABLE
/***********************TIME**********************************/
//#define TIM1_UP_ENABLE 0
//#define TIM2_UP_ENABLE 0
//#define TIM3_UP_ENABLE 0
#define TIM4_UP_ENABLE
#define TIM5_UP_ENABLE
//#define TIM6_UP_ENABLE 0
//#define TIM7_UP_ENABLE 0
#define TIM8_UP_ENABLE
//#define TIM9_UP_ENABLE 0


/*********************************************/
#define AIN_NUMBER  5
#define DAC_NUMBER  4
#define ADC1_CHANNELS  1
#define ADC2_CHANNELS  3
#define ADC3_CHANNELS  0
#define RR 10
/*********************************************/



#endif /* USER_MAIN_H_ */
