/*
 * hal_wdt_ch32.c
 *
 *  Created on: 24 июл. 2024 г.
 *      Author: i.dymov
 */

#include "hal_wdt.h"
#include "main.h"

#if CORE == WCH32V2 || CORE == WCH32V3

#if CORE == WCH32V2
    #include "ch32v20x_iwdg.h"
#endif
#if CORE  == WCH32V3
   #include "ch32v30x_iwdg.h"
#endif



#define CTLR_KEY_Reload    ((uint16_t)0xAAAA)
#define CTLR_KEY_Enable    ((uint16_t)0xCCCC)
#define CTLR_KEY_Modify    ((uint16_t)0x5555)

void HAL_WDT_Init( uint32_t period_in_3_2ms)
{
#ifdef WDT_ENABLE
    IWDG->CTLR = IWDG_WriteAccess_Enable;
    while ((IWDG->STATR & IWDG_FLAG_PVU) !=0 ) ;
    IWDG->PSCR = IWDG_Prescaler_64;
    while ((IWDG->STATR & IWDG_FLAG_RVU) !=0 ) ;
    IWDG->RLDR = period_in_3_2ms;
    IWDG->CTLR = CTLR_KEY_Reload;
    IWDG->CTLR = CTLR_KEY_Enable;
#endif

}


void HAL_WDTInit()
{
#ifdef WDT_ENABLE
    IWDG->CTLR = IWDG_WriteAccess_Enable;
    IWDG->PSCR = IWDG_Prescaler_64;
    IWDG->RLDR = 4000;
    IWDG->CTLR = CTLR_KEY_Reload;
    IWDG->CTLR = CTLR_KEY_Enable;
#endif
}

void HAL_WDTReset()
{
#ifdef WDT_ENABLE
    IWDG->CTLR = CTLR_KEY_Reload;
#endif
}
/*
 *
 */
void HAL_WDT_Init1s()
{
#ifdef WDT_ENABLE

    IWDG->CTLR = IWDG_WriteAccess_Enable;
    IWDG->PSCR = IWDG_Prescaler_64;
    IWDG->RLDR =  625;
    IWDG->CTLR = CTLR_KEY_Reload;
    IWDG->CTLR = CTLR_KEY_Enable;

#endif
}
#endif
