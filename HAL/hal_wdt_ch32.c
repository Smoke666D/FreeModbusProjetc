/*
 * hal_wdt_ch32.c
 *
 *  Created on: 24 июл. 2024 г.
 *      Author: i.dymov
 */

#include "hal_wdt.h"

#if MCU == CH32V2 || MCU == CH32V3

#if MCU == CH32V2
    #include "ch32v20x_iwdg.h"
#endif
#if MCU == CH32V3
   #include "ch32v30x_iwdg.h"
#endif



#define CTLR_KEY_Reload    ((uint16_t)0xAAAA)
#define CTLR_KEY_Enable    ((uint16_t)0xCCCC)


void HAL_WDTInit()
{
#ifdef WDT
    IWDG->CTLR = IWDG_WriteAccess_Enable;
    IWDG->PSCR = IWDG_Prescaler_64;
    IWDG->RLDR = 4000;
    IWDG->CTLR = CTLR_KEY_Reload;
    IWDG->CTLR = CTLR_KEY_Enable;
#endif
}

void HAL_WDTReset()
{
#ifdef WDT
    IWDG->CTLR = CTLR_KEY_Reload;
#endif
}
/*
 *
 */
void HAL_WDT_Init1s()
{
#ifdef WDT

    IWDG->CTLR = IWDG_WriteAccess_Enable;
    IWDG->PSCR = IWDG_Prescaler_64;
    IWDG->RLDR =  625;
    IWDG->CTLR = CTLR_KEY_Reload;
    IWDG->CTLR = CTLR_KEY_Enable;

#endif
}
#endif
