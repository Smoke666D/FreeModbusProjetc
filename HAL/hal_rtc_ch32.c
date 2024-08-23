/*
 * hal_rtc_ch32.c
 *
 *  Created on: 1 авг. 2024 г.
 *      Author: i.dymov
 */

#include "hal_rtc.h"

#if MCU == CH32V2 || MCU == CH32V3

#include "hal_irq.h"
/* RTC_Private_Defines */
#define RTC_LSB_MASK     ((uint32_t)0x0000FFFF) /* RTC LSB Mask */
#define PRLH_MSB_MASK    ((uint32_t)0x000F0000) /* RTC Prescaler MSB Mask */
void RTC_IRQHandler ( void )  __attribute__((interrupt()));
const u8 mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//Callbacl функция для вызова из секундрого прерывания RTC
static void (* func)( void);



void HAL_RTC_IT_Init( HAL_RTC_INIT_t init, void (* rtc_it_callback) ( void ), uint8_t prior, uint8_t subprior )
{

    uint8_t temp = 0;
    RCC->APB1PCENR |=(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP);   //Разрешаем тактирование
    PWR->CTLR |= (1 << 8);

    if ( init == HAL_RTC_NEW_INIT)
    {
        RCC->BDCTLR |= (1<<16);    //Сборс модуля Buckup
        RCC->BDCTLR &= ~(1<<16);
    }
        RCC_LSEConfig(RCC_LSE_ON);

        while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && temp < 250)
        {
              temp++;
              vTaskDelay(20);
        }
        if(temp >= 250) return;
        RCC->BDCTLR |=RCC_RTCCLKSource_LSE;
        RCC->BDCTLR |= (1<<15); //This function must be used only after the RTC clock was selected
                            // using the RCC_RTCCLKConfig function.
        RTC_WaitForLastTask();
        RTC_WaitForSynchro();
        RTC_WaitForLastTask();
        RTC_SetPrescaler(32767);
        RTC_WaitForLastTask();
        RTC_ExitConfigMode();

    RTC->CTLRH |= RTC_IT_SEC;   //Разрешаем прерывание
#if MCU == CH32V2
    PFIC_IRQ_ENABLE_PG1(RTC_IRQn,prior,subprior);
#endif
#if MCU == CH32V3
    PFIC_IRQ_ENABLE_PG2(RTC_IRQn,prior,subprior);
#endif
	  func = rtc_it_callback;
	  return;
}


void RTC_IRQHandler ( void )
{
    if ((RTC->CTLRH & RTC_FLAG_SEC) )
    {
        func();
        RTC->CTLRL &= (uint16_t)~RTC_FLAG_SEC;
    }
    RTC_WaitForLastTask();
    return;

}

void HAL_RTC_ReadTime( HAL_TimeConfig_T* time)
{
    printf("%i\r\n",RTC_GetCounter());
    u32 temp = RTC_GetCounter() % 86400;
    time->hours    = temp / 3600;
    time->minutes  = (temp % 3600) / 60;
    time->seconds  = (temp % 3600) % 60;
}

static u8 Is_Leap_Year(u16 year)
{
    if(year % 4 == 0)
    {
        if(year % 100 == 0)
        {
            if(year % 400 == 0)
                return 1;
            else
                return 0;
        }
        else
            return 1;
    }
    else
        return 0;
}

void HAL_RTC_ReadDate(HAL_DateConfig_T* date)
{

    u32        timecount = 0;
    u32        temp = 0;
    u16        temp1 = 0;
    u16           year;
    timecount = RTC_GetCounter();
    temp = timecount / 86400;

    temp1 = 1970;
    while(temp >= 365)
    {
        if(Is_Leap_Year(temp1))
        {
                if(temp >= 366)
                    temp -= 366;
                else
                {
                    temp1++;
                    break;
                }
            }
            else
                temp -= 365;
            temp1++;
        }
    year = temp1;

        temp1 = 0;
        while(temp >= 28)
        {
            if(Is_Leap_Year(year) && temp1 == 1)
            {
                if(temp >= 29)
                    temp -= 29;
                else
                    break;
            }
            else
            {
                if(temp >= mon_table[temp1])
                    temp -= mon_table[temp1];
                else
                    break;
            }
            temp1++;
        }
        date->year  = (year %100);
        date->month = temp1 + 1;
        date->date = temp + 1;

}

uint8_t HAL_RTC_ConfigTime( HAL_TimeConfig_T* timeConfig)
{
  return 1;
}
uint8_t HAL_RTC_ConfigDate( HAL_DateConfig_T* dateConfig)
{
    u32 time = RTC_GetCounter() % 86400;
    u16 t;
    u32 seccount = 0;
    if( dateConfig->year > 99)
        return 1;
    for(t = 1970; t < (dateConfig->year + 2000); t++)
    {
        if(Is_Leap_Year(t))
            seccount += 31622400;
        else
            seccount += 31536000;
    }
    dateConfig->month -= 1;
    for(t = 0; t < dateConfig->month; t++)
    {
        seccount += (u32)mon_table[t] * 86400;
        if(Is_Leap_Year(dateConfig->year + 2000) && t == 1)
            seccount += 86400;
    }
    seccount += (u32)(dateConfig->date - 1) * 86400;
    seccount += time;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_SetCounter(seccount);
    RTC_WaitForLastTask();
    return 0;
}

#endif
