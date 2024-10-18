/*
 * rtc_task.c
 *
 *  Created on: Aug 14, 2024
 *      Author: i.dymov
 */


#include "rtc_task.h"
#include "init.h"
#include "hw_lib_din.h"
#include "data_model.h"


static u32 hour_counter;
static u8 second_counter;
/*
 * §¬§Ñ§Ý§Ò§Ö§Ü §æ§å§ß§Ü§è§Ú§ñ §Õ§Ý§ñ §á§â§Ö§â§í§Ó§Ñ§ß§Ú§ñ RTC
 */
static void vRTCTaskITCallback( void )
{
  if (eGetDOUT(OUT_1))   //§¦§ã§Ý§Ú §á§â§à§è§Ö§ã§ã §å§á§â§Ó§Ý§Ö§ß§Ú§ñ §Ó §â§Ñ§Ò§à§é§Ö§Þ §â§Ö§Ø§Ú§Þ§Ö, §ã§é§Ú§ä§Ñ§Ö§Þ §Þ§à§ä§à§é§Ñ§ã§í
  {
      if (++second_counter >=60)
      {
          hour_counter++;
          second_counter = 0;
          setReg32(MOTO_HOURS,hour_counter);
      }
  }
  return;
}

void ResetMotorHour()
{
    hour_counter = 0;

}

u16 vRTC_TASK_GetMinute( void )
{
    return (hour_counter % 60);
}
u32 vRTC_TASK_GetHoure( void )
{
    return (hour_counter / 60);
}
void vRTC_Init()
{
    HAL_RTC_IT_Init( &vRTCTaskITCallback, RTC_IT_PRIOR,RTC_IT_SUBPRIOR);
}

void vRTC_TASK_Init()
{
    hour_counter = getReg32(MOTO_HOURS);

    return;
}
