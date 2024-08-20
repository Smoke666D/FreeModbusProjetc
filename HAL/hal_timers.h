/*
 * hal_timers.h
 *
 *  Created on: 9 閭攲瑜�. 2024 璋�.
 *      Author: i.dymov
 */

#ifndef HAL_HAL_TIMERS_H_
#define HAL_HAL_TIMERS_H_

#include "main.h"
#include "hal_config.h"
#if MCU == APM32
	#include "apm32f4xx_tmr.h"
	#include "apm32f4xx_rcm.h"
#endif
#define EDGE 3

#define TIM_CHANNEL_1  0x01
#define TIM_CHANNEL_2  0x02
#define TIM_CHANNEL_3  0x04
#define TIM_CHANNEL_4  0x08

#define CC_PERIOD      60000

#define TIM_UPDATE_FLAG TMR_INT_UPDATE
#define TIM_CC1_FLAG   TMR_INT_CC1
#define TIM_CC2_FLAG   TMR_INT_CC2
#define TIM_CC3_FLAG   TMR_INT_CC3
#define TIM_CC4_FLAG   TMR_INT_CC4

#define TIM10_INT  TMR1_UP_TMR10_IRQn
#define TIM9_INT   TMR1_BRK_TMR9_IRQn

typedef enum {
	FREE_RUN_TIMER,
	PWM_TIMER,
} TimerType_t;

typedef enum {
	TIMER1 = 0,
	TIMER2 = 1,
	TIMER3 = 2,
	TIMER4 = 3,
	TIMER5 = 4,
	TIMER6 = 5,
	TIMER7 = 6,
	TIMER8 = 7,
	TIMER9 = 8,
	TIMER10 =9,
	TIMER11 =10,
	TIMER12 = 11,
	TIMER13 = 12,
	TIMER14  =13,
} TimerName_t;


#if MCU == APM32
#define TimerRef_t TMR_T*
#define TIMERS_COUNT  14
#endif
#if MCU == CH32V2 || MCU==CH32V3
#define TimerRef_t TIM_TypeDef *

#endif
#if MCU == CH32V2
#define TIMERS_COUNT  4
#endif
#if MCU == APM32
#define TIMERS_COUNT  6
#endif
#if MCU == CH32V3
#define TIMERS_COUNT  9
#endif


typedef struct
{
	uint16_t Period;
	uint16_t Div;
	uint16_t ClockDiv;
	TimerRef_t timers;
	void (* callback_function)();
	void (* event_callback_function)(u8, u16);
} TimerConfif_t;
#if MCU == CH32V2
u32 HAL_GetTimerCounterRegAdres(TimerName_t TimerName , uint8_t ch );
void HAL_TIMER_EnablePWMCH(TimerName_t TimerName  );
void HAL_TimeInitCaptureDMA( TimerName_t TimerName , uint32_t freq_in_hz, uint32_t Period, uint8_t channel );

#endif


void vHW_L_LIB_FreeRunEneblae( TimerName_t TimerName );
void HAL_TimerReset( TimerName_t TimerName);
void  HW_TIMER_TimerInit(TimerName_t TimerName, uint32_t freq_in_hz, uint32_t Period );
void HAL_TiemrEneblae( TimerName_t TimerName );
void HAL_TiemrDisable( TimerName_t TimerName );

void HAL_TIMER_EnablePWMCH(TimerName_t TimerName  );
void HAL_TIMER_InitIt( TimerName_t TimerName, uint32_t freq_in_hz, uint32_t Period, void (*f)() ,uint8_t prior, uint8_t subprior );

void HAL_TIMER_PWMTimersInit(TimerName_t TimerName , uint32_t freq_in_hz, uint32_t Period, uint8_t channel);
void HAL_TIMER_SetPWMPulse( TimerName_t TimerName , uint8_t channel, uint32_t pulse );
#if MCU== CH32V3
void HW_TIMER_SelectOutTrigger( TimerName_t TimerName, u32 trigger_source);
#endif

void HW_TIMER_BaseTimerInit(TimerName_t TimerName  );

void HAL_TimeInitCaptureIT( TimerName_t TimerName , uint32_t freq_in_hz, uint32_t Period, uint8_t channel,uint8_t prior, uint8_t subprior, void (*f)( u8 ,u16) );

#if MCU == APM32
void vHW_L_LIB_FreeRunInit( TimerName_t TimerName, uint32_t freq_in_hz  );
void vHAL_SetTimerFreq( TimerName_t TimerName, uint32_t freq_in_hz  );
void vHAL_EnableTimerIRQ(TimerName_t TimerName,uint8_t priority, uint16_t interupt, uint8_t INT_FLAG );
void HAL_InitCaptureIRQTimer( TimerName_t TimerName , uint32_t freq_in_hz, uint32_t Period, uint8_t channel );
uint16_t vHAL_CaptureTimerInteruptCallback(TimerName_t TimerName , uint16_t TimInterupt  );
void HAL_TIMER_DisablePWMCH(TimerName_t TimerName , uint8_t channel );
void HAL_TIMER_EnablePWMCH(TimerName_t TimerName , uint8_t channel );
uint32_t uGetFreeRuningTimer(TimerName_t TimerName );
#endif
#endif /* HAL_HAL_TIMERS_H_ */



