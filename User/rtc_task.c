/*
 * rtc_task.c
 *
 *  Created on: Aug 14, 2024
 *      Author: i.dymov
 */


#include "rtc_task.h"
#include "init.h"

/*
 * ���ѧݧҧ֧� ���ߧܧ�ڧ� �էݧ� ���֧��ӧѧߧڧ� RTC
 */
static void vRTCTaskITCallback()
{

}

void vRTCTaskInit(HAL_RTC_INIT_t init)
{

    HAL_RTC_IT_Init(init, &vRTCTaskITCallback, RTC_IT_PRIOR,RTC_IT_SUBPRIOR);

}
