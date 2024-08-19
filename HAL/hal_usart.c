/*
 * hal_usart.c
 *
 *  Created on: Aug 14, 2024
 *      Author: smoke
 */


#include "hal_usart.h"


#if MCU == CH32V2 || MCU == CH32V3

#include "hal_irq.h"

void USART1_IRQHandler( void )  __attribute__((interrupt()));
void USART2_IRQHandler( void )  __attribute__((interrupt()));
USART_CallBack_t CallBackFunction[2];

#include "ch32v30x_usart.h"
#include "ch32v30x.h"

#define CTLR1_UE_Set              ((uint16_t)0x2000) /* USART Enable Mask */

USART_TypeDef * USART[]={USART1,USART2};

void HALUSARTInit(HAL_USART_t usart, uint32_t USART_BaudRate, HAL_USART_STOP_BIT_t USART_StopBits ,HAL_USART_PARITY_t USART_Parity ,HAL_USART_WL_t USART_wordlength   )
{
    switch (usart)
    {
        case HAL_USART1:
            HAL_InitAPB2(RCC_APB2Periph_USART1);
            break;
        case HAL_USART2:
            HAL_InitAPB1(RCC_APB1Periph_USART2);
            break;
    }
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = USART_BaudRate;
    USART_InitStructure.USART_WordLength = USART_wordlength ;
    USART_InitStructure.USART_StopBits = USART_StopBits;
    USART_InitStructure.USART_Parity = USART_Parity;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;
    USART_Init(USART[usart], &USART_InitStructure);

}

void HALUSARTEnable(HAL_USART_t usart)
{
    USART[usart]->CTLR1 |= CTLR1_UE_Set;
}

void HALUSARTInitIT(HAL_USART_t usart, void (* rx_it_callback) ( void ), void (* tx_it_callback) ( void ), uint8_t prior, uint8_t subprior)
{
    switch (usart)
    {
        case HAL_USART1:
#if MCU == CH32V2
    PFIC_IRQ_ENABLE_PG1(USART1_IRQn,prior,subprior);
#endif
#if MCU == CH32V3
    PFIC_IRQ_ENABLE_PG2(USART1_IRQn,prior,subprior);
#endif
                break;
            case HAL_USART2:
#if MCU == CH32V2
    PFIC_IRQ_ENABLE_PG1(USART2_IRQn,prior,subprior);
#endif
#if MCU == CH32V3
    PFIC_IRQ_ENABLE_PG2(USART2_IRQn,prior,subprior);
#endif
                break;

    }
    CallBackFunction[usart].rx = rx_it_callback;
    CallBackFunction[usart].tx = tx_it_callback;
}

void HAL_SendByte_IT(HAL_USART_t usart, u8 data )
{
    USART_SendData(USART[usart], data);
    USART_ITConfig(USART[usart],USART_IT_TC,ENABLE);
}

void USART1_IRQHandler( void )  __attribute__((interrupt()))
{

}
void USART2_IRQHandler( void )  __attribute__((interrupt()))
{
   if ( USART_GetITStatus(USART2,USART_IT_TC  ) == SET )
   {
       USART_GetITStatus(USART2,USART_IT_TC);
       USART_ITConfig(USART2,USART_IT_TC,DISABLE);
       CallBackFunction[1].tx();
   }
   if ( USART_GetITStatus(USART2,USART_IT_RXNE ) == SET )
   {
       CallBackFunction[1].rx();
  }

}

#endif
