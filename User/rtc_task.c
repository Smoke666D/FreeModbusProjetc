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
 * ���ѧݧҧ֧� ���ߧܧ�ڧ� �էݧ� ���֧��ӧѧߧڧ� RTC
 */
static void vRTCTaskITCallback( void )
{
  if (eGetDOUT(OUT_1))   //����ݧ� �����֧�� ����ӧݧ֧ߧڧ� �� ��ѧҧ��֧� ��֧اڧާ�, ���ڧ�ѧ֧� �ާ����ѧ��
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
