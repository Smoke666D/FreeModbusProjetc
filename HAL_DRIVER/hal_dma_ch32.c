/*
 * hal_dma.c
 *
 *  Created on: 25 апр. 2024 г.
 *      Author: i.dymov
 */

#include "hal_dma.h"
#include "hal_irq.h"

#if CORE == APM32
    #include "apm32f4xx_dma.h"
    #include "apm32f4xx_rcm.h"
    #include "apm32f4xx_adc.h"
#endif


#if CORE == WCH32V2
    #include "ch32v20x_dma.h"
    #include "ch32v20x_adc.h"
    #include "ch32v20x_rcc.h"
/* DMA registers Masks */
    DMA_CFG_t DMA_CALLback[7]   __SECTION(RAM_SECTION_CCMRAM);
    DMA_Channel_TypeDef * DMACH[]={DMA1_Channel1,DMA1_Channel2,DMA1_Channel3,DMA1_Channel4,DMA1_Channel5,DMA1_Channel6,DMA1_Channel7};
#endif
#if CORE == WCH32V3
    #include "ch32v30x_dma.h"
    #include "ch32v30x_adc.h"
    #include "ch32v30x_rcc.h"
    DMA_CFG_t DMA_CALLback[14]   __SECTION(RAM_SECTION_CCMRAM);
    DMA_Channel_TypeDef * DMACH[]={DMA1_Channel1,DMA1_Channel2,DMA1_Channel3,DMA1_Channel4,DMA1_Channel5,DMA1_Channel6,DMA1_Channel7,
                               DMA2_Channel1,DMA2_Channel2,DMA2_Channel3,DMA2_Channel4,DMA2_Channel5,DMA2_Channel6,DMA2_Channel7,
};
#endif

#if CORE == WCH32V2 || CORE == WCH32V3
#ifdef DMA1_CH1_ENABLE
void     DMA1_Channel1_IRQHandler(void)   __attribute__((interrupt()));
#endif
#ifdef DMA1_CH2_ENABLE
void     DMA1_Channel2_IRQHandler(void)   __attribute__((interrupt()));
#endif
#ifdef DMA1_CH3_ENABLE
void     DMA1_Channel3_IRQHandler(void)   __attribute__((interrupt()));
#endif
#ifdef DMA1_CH4_ENABLE
void     DMA1_Channel4_IRQHandler(void)   __attribute__((interrupt()));
#endif
#ifdef DMA1_CH5_ENABLE
void     DMA1_Channel5_IRQHandler(void)   __attribute__((interrupt()));
#endif
#ifdef DMA1_CH6_ENABLE
void     DMA1_Channel6_IRQHandler(void)   __attribute__((interrupt()));
#endif
#ifdef DMA1_CH7_ENABLE
void     DMA1_Channel7_IRQHandler(void)   __attribute__((interrupt()));
#endif




void HAL_DMAInitIT(  DMA_INIT_t Init, uint8_t prior, uint8_t subprior, void (*f)(void))
{
#ifdef DMA2
    if (Init.stream < DMA2_CH1)
    {
#endif
        RCC->AHBPCENR |= RCC_AHBPeriph_DMA1;
        DMA1->INTFCR  |= 0xF<<Init.stream;
#ifdef DMA2_PRESENT
    }
    else
    {
        RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA2, ENABLE );
        DMA2->INTFCR |= 0xF<<Init.stream;
    }
#endif
    //HAL_DMA_Disable(Init.stream);
    u32 DMA_MemoryDataSize;
    u32 DMA_PeripheralDataSize;
    switch (Init.dma_size)
    {
       case DMA_BYTE:
           DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
           DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
           break;
      case DMA_HWORD:
           DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
           DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
           break;
      default:
           DMA_MemoryDataSize = DMA_MemoryDataSize_Word ;
           DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word ;
           break;
     }
    DMA_CALLback[Init.stream].CallBack = f;
    HAL_DMA_Disable(Init.stream);

    uint32_t tmpreg = 0;
    tmpreg = DMACH[Init.stream]->CFGR;
    tmpreg &= CFGR_CLEAR_Mask;
    tmpreg |= Init.direction |  Init.mode |
                DMA_PeripheralInc_Disable | DMA_MemoryInc_Enable |
                DMA_PeripheralDataSize | DMA_MemoryDataSize |
    Init.prioroty |  DMA_M2M_Disable;

    DMACH[Init.stream]->CFGR = tmpreg;
    DMACH[Init.stream]->CNTR = Init.bufsize;
    DMACH[Init.stream]->PADDR = Init.paddr;
    DMACH[Init.stream]->MADDR = Init.memadr;
    DMACH[Init.stream]->CFGR |= DMA_IT_TC;
#ifdef DMA2
    if (Init.stream < DMA2_CH1)
    {

        PFIC_IRQ_ENABLE_PG2(DMA1_Channel1_IRQn  + Init.stream ,prior,subprior);
    }
    else

    {

        PFIC_IRQ_ENABLE_PG2(DMA2_Channel1_IRQn  + (Init.stream-DMA2_CH1) ,prior,subprior);
    }
#else
    PFIC_IRQ_ENABLE_PG1(DMA1_Channel1_IRQn  + Init.stream ,prior,subprior);
#endif

}


#if CORE == WCH32V3

void HAL_DMA1InitMemToMemHwordIT(DMA_Stram_t stream ,
                    DMA_Prioroty_t dma_prior,
                    uint8_t prior,
                    uint8_t subprior,
                    void (*f)(void) )
{
    RCC->AHBPCENR |= RCC_AHBPeriph_DMA1;
    DMA1->INTFCR  |= 0xF<<stream;
    DMA_CALLback[stream].CallBack = f;
    HAL_DMA_Disable(stream);
    uint32_t tmpreg = 0;
    tmpreg = DMACH[stream]->CFGR;
    tmpreg &= CFGR_CLEAR_Mask;
    tmpreg |= PTOM  |  DMA_Mode_Normal |
                DMA_PeripheralInc_Disable | DMA_MemoryInc_Enable |
                DMA_PeripheralDataSize_HalfWord | DMA_MemoryDataSize_HalfWord|
                dma_prior |  DMA_M2M_Enable;
    DMACH[stream]->CFGR = tmpreg;
    DMACH[stream]->CNTR = 0;
    DMACH[stream]->CFGR |= DMA_IT_TC;
    PFIC_IRQ_ENABLE_PG2(DMA1_Channel1_IRQn  + stream , prior, subprior);
}

#endif
void HAL_DMAMem2MemStart(DMA_Stram_t stream, uint32_t data_size, u32 source, u32 dest )
{
    DMACH[stream]->CNTR = data_size;
    DMACH[stream]->PADDR = source;
    DMACH[stream]->MADDR = dest;
    DMACH[stream]->CFGR |= DMA_CFGR1_EN;
}

void  HAL_DMA_Start(DMA_Stram_t stream, uint32_t counter, u32 memadr )
{
    DMACH[stream]->MADDR = memadr;
    DMACH[stream]->CNTR  = counter;
    DMACH[stream]->CFGR |= DMA_CFGR1_EN;
}


void HAL_DMA_SetCouterAndEnable(DMA_Stram_t stream, uint32_t counter )
{
    DMACH[stream]->CNTR = counter;
    DMACH[stream]->CFGR |= DMA_CFGR1_EN;
}

void HAL_DMA_Disable(DMA_Stram_t stream  )
{
    DMACH[stream]->CFGR &= (uint16_t)(~DMA_CFGR1_EN);
}

void HAL_DMA_Enable(DMA_Stram_t stream  )
{
    DMACH[stream]->CFGR |= DMA_CFGR1_EN;
}

void HAL_DMA_ITENABLE( DMA_Stram_t stream, uint32_t it )
{
    DMACH[stream]->CFGR |=it;
}

void HAL_DMA_SetCounter( DMA_Stram_t stream, uint32_t counter )
{
    DMACH[stream]->CNTR = counter;
}

static void DMA_IRQ( u8 ch)
{
    if ( DMA_GetITStatus(DMA1_IT_TC1<<(4*ch))==SET )
    {
         DMA_CALLback[ch].CallBack();
         DMA_ClearITPendingBit(DMA1_IT_GL1<<(4*ch)); //Сбрасываем флаг
    }
    return;

}

#ifdef DMA1_CH1_ENABLE
void DMA1_Channel1_IRQHandler(void)
{
    DMA_IRQ(0);
    return;
}
#endif
#ifdef DMA1_CH2_ENABLE
void DMA1_Channel2_IRQHandler(void)
{
    DMA_IRQ(1);
    return;
}
#endif
#ifdef DMA1_CH3_ENABLE
void DMA1_Channel3_IRQHandler(void)
{
    DMA_IRQ(2);
    return;
}
#endif
#ifdef DMA1_CH4_ENABLE
void  DMA1_Channel4_IRQHandler(void)
{
    DMA_IRQ(3);
    return;
}
#endif
#ifdef DMA1_CH5_ENABLE
void  DMA1_Channel5_IRQHandler(void)
{
    DMA_IRQ(4);
    return;
}
#endif
#ifdef DMA1_CH6_ENABLE
void  DMA1_Channel6_IRQHandler(void)
{
    DMA_IRQ(5);
    return;
}
#endif
#ifdef DMA1_CH7_ENABLE
void DMA1_Channel7_IRQHandler(void)
{
    DMA_IRQ(6);
    return;
}
#endif


#endif



