/*
 * rtc_task.c
 *
 *  Created on: Aug 14, 2024
 *      Author: i.dymov
 */


#include "rtc_task.h"
#include "init.h"

/*
 * §¬§Ñ§Ý§Ò§Ö§Ü §æ§å§ß§Ü§è§Ú§ñ §Õ§Ý§ñ §á§â§Ö§â§í§Ó§Ñ§ß§Ú§ñ RTC
 */
static void vRTCTaskITCallback()
{

}

void vRTCTaskInit(HAL_RTC_INIT_t init)
{

    HAL_RTC_IT_Init(init, &vRTCTaskITCallback, RTC_IT_PRIOR,RTC_IT_SUBPRIOR);

}
