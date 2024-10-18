/*
 * rtc_task.h
 *
 *  Created on: Aug 14, 2024
 *      Author: i.dymov
 */

#ifndef USER_RTC_TASK_H_
#define USER_RTC_TASK_H_

#include "main.h"
#include "hal_rtc.h"


void vRTC_Init();
void vRTC_TASK_Init();
u16 vRTC_TASK_GetMinute( void );
u32 vRTC_TASK_GetHoure( void );
void ResetMotorHour();
#endif /* USER_RTC_TASK_H_ */
