/*
 * hal_rtc.h
 *
 *  Created on: 26 апр. 2024 г.
 *      Author: i.dymov
 */

#ifndef HAL_HAL_RTC_H_
#define HAL_HAL_RTC_H_

#include "main.h"


typedef enum
{
    MONTH_JANUARY   = 0x01, /*!< January */
    MONTH_FEBRUARY  = 0x02, /*!< February */
    MONTH_MARCH     = 0x03, /*!< March */
    MONTH_APRIL     = 0x04, /*!< April */
    MONTH_MAY       = 0x05, /*!< May */
    MONTH_JUNE      = 0x06, /*!< June */
    MONTH_JULY      = 0x07, /*!< July */
    MONTH_AUGUST    = 0x08, /*!< August */
    MONTH_SEPTEMBER = 0x09, /*!< September */
    MONTH_OCTOBER   = 0x10, /*!< October */
    MONTH_NOVEMBER  = 0x11, /*!< November */
    MONTH_DECEMBER  = 0x12  /*!< December */
} HAL_MONTH_T;

/**
 * @brief RTC WEEKDAY
 */
typedef enum
{
    WEEKDAY_MONDAY    = 0x01,   /*!< Set Monday as the weekend */
    WEEKDAY_TUESDAY   = 0x02,   /*!< Set Tuesday as the weekend */
    WEEKDAY_WEDNESDAY = 0x03,   /*!< Set Wednesday as the weekend */
    WEEKDAY_THURSDAY  = 0x04,   /*!< Set Thursday as the weekend */
    WEEKDAY_FRIDAY    = 0x05,   /*!< Set Friday as the weekend */
    WEEKDAY_SATURDAY  = 0x06,   /*!< Set Saturday as the weekend */
    WEEKDAY_SUNDAY    = 0x07    /*!< Set sunday as the weekend */
} HAL_WEEKDAY_T;

typedef enum
{
    HAL_H12_AM, /*!< Set RTC time to AM */
    HAL_H12_PM  /*!< Set RTC time to PM */
} HAL_H12_T;

typedef struct
{
    uint8_t   hours;    /*!< Set hours of RTC time */
    uint8_t   minutes;  /*!< Set minutes of RTC time */
    uint8_t   seconds;  /*!< Set seconds of RTC time */
    HAL_H12_T h12;      /*!< Set RTC time to AM or PM */
} HAL_TimeConfig_T;

/**
  * @brief  RTC Date structure definition
  */

typedef struct
{
    HAL_WEEKDAY_T weekday;  /*!< Set weekday of RTC date */
    HAL_MONTH_T   month;    /*!< Set month of RTC date */
    uint8_t       date;     /*!< Set date of RTC date */
    uint8_t       year;     /*!< Set year of RTC date */
} HAL_DateConfig_T;

typedef enum
{
  HAL_RTC_NEW_INIT    = 0,
  HAL_RTC_NORMAL_INIT = 1,
} HAL_RTC_INIT_t;

#if MCU == APM32

void vRTCInit();
#endif

uint8_t HAL_RTC_ConfigTime( HAL_TimeConfig_T* timeConfig);
uint8_t HAL_RTC_ConfigDate( HAL_DateConfig_T* dateConfig);
void HAL_RTC_ReadTime( HAL_TimeConfig_T* time);
void HAL_RTC_ReadDate(HAL_DateConfig_T* date);

#if CORE == WCH32V2 || CORE == WCH32V3
#ifdef RTC_IRQ_ENABLE
void RTC_IRQHandler ( void );
#endif
void HAL_RTC_IT_Init(  void (* rtc_it_callback) ( void ), uint8_t prior, uint8_t subprior );
#endif

#endif /* HAL_HAL_RTC_H_ */
