/*
 * hal_timers.c
 *
 *  Created on: 9 апр. 2024 г.
 *      Author: i.dymov
 */


#include "hal_timers.h"
#include "hal_irq.h"


static void vTimerInitRCC(TimerName_t TimerName);
static TimerConfif_t config[TIMERS_COUNT];

#if CORE == APM32
static uint32_t getTimerFreq( TimerName_t TimerName );
static TMR_T * timers[TIMERS_COUNT] = { TMR1,TMR2,TMR3,TMR4,TMR5,TMR6,TMR7,TMR8,TMR9,TMR10,TMR11,TMR12,TMR13,TMR14};
#endif

#if  CORE == WCH32V3
static TIM_TypeDef * timers[TIMERS_COUNT] = { TIM1,TIM2,TIM3,TIM4,TIM5,TIM6,TIM7,TIM8,TIM9};
#endif
#if CORE == WCH32V2
static TIM_TypeDef * timers[TIMERS_COUNT] = { TIM1,TIM2,TIM3,TIM4};
#endif

static uint32_t getTimerFreq(  )
{
                return  ( SystemCoreClock );
}


#if CORE == WCH32V2 || CORE == WCH32V3


void HAL_TiemrEneblae( TimerName_t TimerName )
{
    timers[TimerName]->CNT = 0;
    timers[TimerName]->CTLR1 |= TIM_CEN;

}
/*
 *  Функция иницализаиурет тактирование таймера и возвращает указатель
 *  на handle нужного таймера
 */

static void vTimerInitRCC(TimerName_t TimerName)
{
    u32 timers_mask;
    if ((TimerName == TIMER1) || (TimerName > TIMER7))
    {
#if CORE == WCH32V3
        switch (TimerName)
        {
            case TIMER1:
#endif
                timers_mask = RCC_APB2Periph_TIM1;
#if CORE == WCH32V3
                break;
            case TIMER8:
                timers_mask = RCC_APB2Periph_TIM8;
                break;
            case TIMER9:
                timers_mask = RCC_APB2Periph_TIM9;
                break;
            default:
                timers_mask = RCC_APB2Periph_TIM10;
                break;
        }
#endif
        RCC->APB2PRSTR |= timers_mask;
        RCC->APB2PRSTR &= ~timers_mask;
        RCC->APB2PCENR |= timers_mask;

    }
    else
    {
        switch (TimerName)
        {
            case TIMER2:
                timers_mask = RCC_APB1Periph_TIM2;
                break;
            case TIMER3:
                timers_mask = RCC_APB1Periph_TIM3;
                break;
            case TIMER4:
                timers_mask = RCC_APB1Periph_TIM4;
                break;
#if CORE == WCH32V3
            case TIMER5:
                timers_mask = RCC_APB1Periph_TIM5;
                break;
            case TIMER6:
                timers_mask = RCC_APB1Periph_TIM6;
                break;
            default:
                timers_mask = RCC_APB1Periph_TIM7;
                break;
#endif
        }
        RCC->APB1PRSTR |= timers_mask;
        RCC->APB1PRSTR &= ~timers_mask;
        RCC->APB1PCENR |= timers_mask;
    }
}

#endif

#if CORE == WCH32V3

void  HW_TIMER_TimerInit(TimerName_t TimerName, uint32_t freq_in_hz, uint32_t Period )
{
    vTimerInitRCC(TimerName) ;
    config[TimerName].Period = Period;
    config[TimerName].Div = ( getTimerFreq() /freq_in_hz);
    HW_TIMER_BaseTimerInit(TimerName);

}


void HW_TIMER_SelectOutTrigger( TimerName_t TimerName, u32 trigger_source)
{
    TIM_SelectOutputTrigger(timers[TimerName], trigger_source);
}

#endif



#if CORE == WCH32V2 || CORE == WCH32V3

#ifdef TIM1_UP_ENABLE
void TIM1_UP_IRQHandler(void) __attribute__((interrupt()));
#endif
#ifdef TIM2_UP_ENABLE
void TIM2_IRQHandler(void) __attribute__((interrupt()));
#endif
#ifdef TIM3_UP_ENABLE
void TIM3_IRQHandler(void) __attribute__((interrupt()));
#endif
#ifdef TIM4_UP_ENABLE
void TIM4_IRQHandler(void) __attribute__((interrupt()));
#endif
#ifdef TIM8_UP_ENABLE
void TIM8_UP_IRQHandler(void) __attribute__((interrupt()));
#endif
#ifdef TIM5_UP_ENABLE
void TIM5_IRQHandler(void) __attribute__((interrupt()));
#endif
#ifdef TIM6_UP_ENABLE
void TIM6_IRQHandler(void) __attribute__((interrupt()));
#endif

void HAL_TIMER_InitIt( TimerName_t TimerName, uint32_t freq_in_hz, uint32_t Period, void (*f)() ,uint8_t prior, uint8_t subprior )
{
    vTimerInitRCC(TimerName) ;
    config[TimerName].Period = Period;
    config[TimerName].Div = ( getTimerFreq() /freq_in_hz);
    config[TimerName].callback_function = f;
    config[TimerName].ClockDiv = 0;
    HW_TIMER_BaseTimerInit(TimerName);
    timers[TimerName]->INTFR = (uint16_t)~TIM_IT_Update;
    timers[TimerName]->DMAINTENR |=  TIM_IT_Update;
    uint8_t irq;
    switch (TimerName )
    {
#ifdef TIM1_UP_ENABLE
        case TIMER1:
            irq = TIM1_UP_IRQn;
            break;
#endif
#ifdef TIM2_UP_ENABLE
        case TIMER2:
            irq = TIM2_IRQn;
            break;
#endif
#ifdef TIM3_UP_ENABLE
        case TIMER3:
            irq = TIM3_IRQn;
            break;
#endif
#ifdef TIM4_UP_ENABLE
        case TIMER4:
            irq = TIM4_IRQn;
            break;
#endif
#ifdef TIM6_UP_ENABLE
        case TIMER6:
            irq = TIM6_IRQn;
            break;
#endif
#ifdef TIM8_UP_ENABLE
        default:
            irq = TIM8_UP_IRQn;
            break;
#endif
#ifdef TIM5_UP_ENABLE
        case TIMER5:
            irq = TIM5_IRQn;
            break;
#endif
    }
#if CORE == WCH32V2
    PFIC_IRQ_ENABLE_PG1(irq,prior,subprior);
#else
    PFIC_IRQ_ENABLE_PG2(irq,prior,subprior);

#endif
}


void static TimerUPIrq( TimerName_t TimerName )
{
    if   (SET == TIM_GetITStatus(timers[TimerName], TIM_IT_Update) )
    {
        config[TimerName].callback_function();
        timers[TimerName]->INTFR = (uint16_t)~TIM_IT_Update;
    }
}

#ifdef TIM1_UP_ENABLE
void  TIM1_UP_IRQHandler(void)
{
    TimerUPIrq(TIMER1);
}
#endif
#ifdef TIM2_UP_ENABLE
void  TIM2_IRQHandler(void)
{
    TimerUPIrq(TIMER2);
}
#endif
#ifdef TIM3_UP_ENABLE
void  TIM3_IRQHandler(void)
{
    TimerUPIrq(TIMER3);
}
#endif
#ifdef TIM4_UP_ENABLE
void  TIM4_IRQHandler(void)
{
    TimerUPIrq(TIMER4);
}
#endif
#ifdef TIM8_UP_ENABLE
void TIM8_UP_IRQHandler()
{
    TimerUPIrq(TIMER8);
}
#endif
#ifdef TIM5_UP_ENABLE
void TIM5_IRQHandler()
{
    TimerUPIrq(TIMER5);
}
#endif
#ifdef TIM6_UP_ENABLE
void TIM6_IRQHandler()
{
    TimerUPIrq(TIMER6);
}
#endif

void HAL_TimerReset( TimerName_t TimerName)
{
    timers[TimerName]->CNT = 0;
}

uint32_t HAL_GetTimerCnt(TimerName_t TimerName )
{

    return (timers[TimerName]->CNT);
}

void HAL_TiemrDisable( TimerName_t TimerName )
{
    timers[TimerName]->CTLR1 &= (uint16_t)(~((uint16_t)TIM_CEN));
}

void  HW_TIMER_BaseTimerInit(TimerName_t TimerName  )
{
    timers[TimerName]->SMCFGR &= (uint16_t)(~((uint16_t)TIM_SMS)); //Тактирование от внутренней шины
    uint16_t tmpcr1 = 0;
    tmpcr1 = timers[TimerName]->CTLR1;
    tmpcr1 &= (uint16_t)(~((uint16_t)(TIM_DIR | TIM_CMS)));
    tmpcr1 |= (uint32_t)TIM_CounterMode_Up;
    tmpcr1 &= (uint16_t)(~((uint16_t)TIM_CTLR1_CKD));
    tmpcr1 |= (uint32_t)config[TimerName].ClockDiv;
    timers[TimerName]->CTLR1   = tmpcr1;
    timers[TimerName]->ATRLR   = config[TimerName].Period;
    timers[TimerName]->PSC     = config[TimerName].Div;
    if((TimerName == TIMER1 ))  timers[TimerName]->RPTCR = 0x0000;
    timers[TimerName]->SWEVGR = TIM_PSCReloadMode_Immediate;
}

u32 HAL_GetTimerCounterRegAdres(TimerName_t TimerName , uint8_t ch )
{
    switch (ch)
    {
        case TIM_CHANNEL_1:
            return (timers[TimerName]->CH1CVR);
        case TIM_CHANNEL_2:
            return (timers[TimerName]->CH2CVR);
        case TIM_CHANNEL_3:
            return (timers[TimerName]->CH3CVR);
        default:
            return (timers[TimerName]->CH4CVR);
    }
    return 0;
}


#if CORE == WCH32V3
void HAL_TIMER_PWMTimersInit(TimerName_t TimerName , uint32_t freq_in_hz, uint32_t Period, uint8_t channel)
{
	 TIM_OCInitTypeDef TIM_OCInitStructure={0};
	 vTimerInitRCC(TimerName) ;
	 config[TimerName].Period = Period;
	 config[TimerName].Div = (getTimerFreq() /(freq_in_hz*Period));
	 config[TimerName].ClockDiv = 0;
	 HW_TIMER_BaseTimerInit(TimerName);
	 TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	 TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	 TIM_OCInitStructure.TIM_Pulse = Period;
	 TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	 if (channel &  TIM_CHANNEL_1)
	 {
		 TIM_OC1Init( timers[TimerName], &TIM_OCInitStructure );
		 TIM_OC1PreloadConfig( timers[TimerName], TIM_OCPreload_Disable );
	 }
	 if (channel &  TIM_CHANNEL_2)
	 {
	     TIM_OC2Init( timers[TimerName], &TIM_OCInitStructure );
		 TIM_OC2PreloadConfig( timers[TimerName], TIM_OCPreload_Disable );
	 }
	 if (channel &  TIM_CHANNEL_3)
	 {
		TIM_OC3Init( timers[TimerName], &TIM_OCInitStructure );
	    TIM_OC3PreloadConfig( timers[TimerName], TIM_OCPreload_Disable );
	 }
	 if (channel &  TIM_CHANNEL_4)
	 {
		 TIM_OC4Init( timers[TimerName], &TIM_OCInitStructure );
		 TIM_OC4PreloadConfig( timers[TimerName], TIM_OCPreload_Disable );
	 }
	 timers[TimerName]->CTLR1 |= TIM_ARPE;
	 timers[TimerName]->BDTR |= TIM_MOE;
}

#endif

#if CORE == WCH32V2
void HAL_TIMER_PWMTimersInit(TimerName_t TimerName , uint32_t freq_in_hz, uint32_t Period, uint8_t channel)
{
     TIM_OCInitTypeDef TIM_OCInitStructure={0};
     vTimerInitRCC(TimerName) ;
     config[TimerName].Period = Period;
     config[TimerName].Div = (72000000U /freq_in_hz);
     config[TimerName].ClockDiv = 0;
     HW_TIMER_BaseTimerInit(TimerName);
     TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
     TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
     TIM_OCInitStructure.TIM_Pulse = Period;
     TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
     if (channel &  TIM_CHANNEL_1)
     {
         TIM_OC1Init( timers[TimerName], &TIM_OCInitStructure );
         TIM_OC1PreloadConfig( timers[TimerName], TIM_OCPreload_Disable );
     }
     if (channel &  TIM_CHANNEL_2)
     {
         TIM_OC2Init( timers[TimerName], &TIM_OCInitStructure );
         TIM_OC2PreloadConfig( timers[TimerName], TIM_OCPreload_Disable );
     }
     if (channel &  TIM_CHANNEL_3)
     {
        TIM_OC3Init( timers[TimerName], &TIM_OCInitStructure );
        TIM_OC3PreloadConfig( timers[TimerName], TIM_OCPreload_Disable );
     }
     if (channel &  TIM_CHANNEL_4)
     {
         TIM_OC4Init( timers[TimerName], &TIM_OCInitStructure );
         TIM_OC4PreloadConfig( timers[TimerName], TIM_OCPreload_Disable );
     }
     timers[TimerName]->CTLR1 |= TIM_ARPE;
     timers[TimerName]->BDTR |= TIM_MOE;
}
#endif

static void TI2_Config(TimerName_t TimerName, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
    uint16_t tmpccmr1 = 0, tmpccer = 0, tmp = 0;

    timers[TimerName]->CCER &= (uint16_t) ~((uint16_t)TIM_CC2E);
    tmpccmr1 = timers[TimerName]->CHCTLR1;
    tmpccer = timers[TimerName]->CCER;
    tmp = (uint16_t)(TIM_ICPolarity << 4);
    tmpccmr1 &= (uint16_t)(((uint16_t) ~((uint16_t)TIM_CC2S)) & ((uint16_t) ~((uint16_t)TIM_IC2F)));
    tmpccmr1 |= (uint16_t)(TIM_ICFilter << 12);
    tmpccmr1 |= (uint16_t)(TIM_ICSelection << 8);
    tmpccer &= (uint16_t) ~((uint16_t)(TIM_CC2P));
    tmpccer |= (uint16_t)(tmp | (uint16_t)TIM_CC2E);
    timers[TimerName]->CHCTLR1 = tmpccmr1;
    timers[TimerName]->CCER = tmpccer;
}

static void TI4_Config(TimerName_t TimerName, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
    uint16_t tmpccmr2 = 0, tmpccer = 0, tmp = 0;
    timers[TimerName]->CCER &= (uint16_t) ~((uint16_t)TIM_CC4E);
    tmpccmr2 = timers[TimerName]->CHCTLR2;
    tmpccer = timers[TimerName]->CCER;
    tmp = (uint16_t)(TIM_ICPolarity << 12);
    tmpccmr2 &= (uint16_t)((uint16_t)(~(uint16_t)TIM_CC4S) & ((uint16_t) ~((uint16_t)TIM_IC4F)));
    tmpccmr2 |= (uint16_t)(TIM_ICSelection << 8);
    tmpccmr2 |= (uint16_t)(TIM_ICFilter << 12);
    tmpccer &= (uint16_t) ~((uint16_t)(TIM_CC4P));
    tmpccer |= (uint16_t)(tmp | (uint16_t)TIM_CC4E);
    timers[TimerName]->CHCTLR2 = tmpccmr2;
    timers[TimerName]->CCER = tmpccer;
}

void HAL_TimeInitCaptureDMA( TimerName_t TimerName , uint32_t freq_in_hz, uint32_t Period, uint8_t channel )
{
     vTimerInitRCC(TimerName) ;
     config[TimerName].Period = Period;
     config[TimerName].Div = (getTimerFreq() /freq_in_hz);
     config[TimerName].ClockDiv = 0;
     HW_TIMER_BaseTimerInit(TimerName);
     if ( channel == TIM_CHANNEL_2 )
     {
         TI2_Config(TimerName, TIM_ICPolarity_Rising, TIM_ICSelection_DirectTI, 4);
         timers[TimerName]->CHCTLR1 &= (uint16_t) ~((uint16_t)TIM_IC2PSC);   // Устанавилваем предделитель
         timers[TimerName]->CHCTLR1 |= (uint16_t)(TIM_ICPSC_DIV1 << 8);
         timers[TimerName]->DMAINTENR |= ( TIM_IT_CC2 | TIM_DMA_CC2 ) ;      //Рзрешаем прерывание и работу по DMA
     }
     if ( channel == TIM_CHANNEL_4 )
     {
          TI4_Config(TimerName, TIM_ICPolarity_Rising,  TIM_ICSelection_DirectTI,  4);
          timers[TimerName]->CHCTLR2 &= (uint16_t) ~((uint16_t)TIM_IC4PSC); // Устанавилваем предделитель
          timers[TimerName]->CHCTLR2 |= (uint16_t)(TIM_ICPSC_DIV1 << 8);
          timers[TimerName]->DMAINTENR |= ( TIM_IT_CC4 | TIM_DMA_CC4 ) ;    //Рзрешаем прерывание и работу по DMA
     }
}

#if CORE == WCH32V3

void HAL_TIMER_SetPWMPulse( TimerName_t TimerName , uint8_t channel, uint32_t pulse )
{
     timers[TimerName]->BDTR &= (uint16_t)(~((uint16_t)TIM_MOE));
     TIM_OCInitTypeDef TIM_OCInitStructure={0};
     timers[TimerName]->BDTR &= (uint16_t)(~((uint16_t)TIM_MOE)); //Выключаем выхода ШИМ
     TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
     TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
     TIM_OCInitStructure.TIM_Pulse = (uint16_t) pulse ;
     TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
     if (channel &  TIM_CHANNEL_1) TIM_OC1Init( timers[TimerName], &TIM_OCInitStructure );
     if (channel &  TIM_CHANNEL_2) TIM_OC2Init( timers[TimerName], &TIM_OCInitStructure );
     if (channel &  TIM_CHANNEL_3) TIM_OC3Init( timers[TimerName], &TIM_OCInitStructure );
     if (channel &  TIM_CHANNEL_4) TIM_OC4Init( timers[TimerName], &TIM_OCInitStructure );
     timers[TimerName]->BDTR |= TIM_MOE;   //Включем выхода ШИМ
}
#endif

#if CORE == WCH32V2

void HAL_TIMER_SetPWMPulse( TimerName_t TimerName , uint8_t channel, uint32_t pulse )
{
     timers[TimerName]->BDTR &= (uint16_t)(~((uint16_t)TIM_MOE));
     TIM_OCInitTypeDef TIM_OCInitStructure={0};
     timers[TimerName]->BDTR &= (uint16_t)(~((uint16_t)TIM_MOE)); //Выключаем выхода ШИМ
     TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
     TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
     TIM_OCInitStructure.TIM_Pulse = (uint16_t) pulse ;
     TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
     if (channel &  TIM_CHANNEL_1) TIM_OC1Init( timers[TimerName], &TIM_OCInitStructure );
     if (channel &  TIM_CHANNEL_2) TIM_OC2Init( timers[TimerName], &TIM_OCInitStructure );
     if (channel &  TIM_CHANNEL_3) TIM_OC3Init( timers[TimerName], &TIM_OCInitStructure );
     if (channel &  TIM_CHANNEL_4) TIM_OC4Init( timers[TimerName], &TIM_OCInitStructure );
     timers[TimerName]->BDTR |= TIM_MOE;   //Включем выхода ШИМ
}


#endif


u16 HAL_TIMER_GetPuse(TimerName_t TimerName , uint8_t channel)
{
    return (timers[TimerName]->CH1CVR);
}

void HAL_TIMER_EnablePWMCH(TimerName_t TimerName  )
{
    timers[TimerName]->BDTR |= TIM_MOE;

}
#endif



