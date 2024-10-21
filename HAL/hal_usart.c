/*
 * hal_usart.c
 *
 *  Created on: Aug 14, 2024
 *      Author: smoke
 */


#include "hal_usart.h"


#if MCU == CH32V2 || MCU == CH32V3

#include "hal_irq.h"

#define USART_IT_RXNE_MASK  0x20
#define USART_IT_TC_MASK    0x40

void USART1_IRQHandler( void )  __attribute__((interrupt()));
void USART2_IRQHandler( void )  __attribute__((interrupt()));
void USART3_IRQHandler( void )  __attribute__((interrupt()));
void UART4_IRQHandler( void )   __attribute__((interrupt()));
USART_CallBack_t CallBackFunction[2];

#include "ch32v30x_usart.h"
#include "ch32v30x.h"

#define CTLR1_UE_Set              ((uint16_t)0x2000) /* USART Enable Mask */

USART_TypeDef * USART[]={USART1,USART2,USART3,UART4};

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
        case HAL_USART3:
            HAL_InitAPB1(RCC_APB1Periph_USART3);
            break;
        case HAL_USART4:
            HAL_InitAPB1(RCC_APB1Periph_UART4);
            break;

    }
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = USART_BaudRate;
    USART_InitStructure.USART_WordLength = USART_wordlength ;
    USART_InitStructure.USART_StopBits = USART_StopBits;
    USART_InitStructure.USART_Parity = USART_Parity;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART[usart], &USART_InitStructure);

}

void HALUSARTEnable(HAL_USART_t usart)
{
    USART[usart]->CTLR1 |= CTLR1_UE_Set;
}

void HALUSARTInitIT(HAL_USART_t usart, void (* rx_it_callback) ( void ), void (* tx_it_callback) ( void ), uint8_t prior, uint8_t subprior)
{
#if MCU == CH32V2

    switch (usart)
    {
        case HAL_USART1:
            PFIC_IRQ_ENABLE_PG1(USART1_IRQn,prior,subprior);
            break;
#endif
#if MCU == CH32V3
    switch (usart)
    {
        case HAL_USART1:
                PFIC_IRQ_ENABLE_PG2(USART1_IRQn,prior,subprior);
                break;
            case HAL_USART2:
                PFIC_IRQ_ENABLE_PG2(USART2_IRQn,prior,subprior);
                break;
            case HAL_USART4:
                PFIC_IRQ_ENABLE_PG2(UART4_IRQn,prior,subprior);
                break;
    }
#endif
    CallBackFunction[usart].rx = rx_it_callback;
    CallBackFunction[usart].tx = tx_it_callback;
}

void HAL_SendByte_IT(HAL_USART_t usart, u8 data )
{
   USART[usart]->DATAR = (data & (uint16_t)0x01FF);
   USART[usart]->CTLR1 |=  USART_IT_TC_MASK;

}

static u8 * recive_byte;

void HAL_RecieveByte_IT(HAL_USART_t usart , u8 * rb)
{
   recive_byte = rb;
   USART[usart]->CTLR1 |= USART_IT_RXNE_MASK;
}


void HAL_RecieveITDisable(HAL_USART_t usart)
{
    USART[usart]->CTLR1 &= ~USART_IT_RXNE_MASK;
}


void HAL_UASRT_IRQ( HAL_USART_t usart)
{

    if ( USART_GetITStatus(USART[usart],USART_IT_TC  ) == SET )
    {
         USART_GetITStatus(USART[usart],USART_IT_TC);
         USART[usart]->CTLR1 &=  ~USART_IT_TC_MASK;
         CallBackFunction[usart].tx();
   }
    if ( USART_GetITStatus(USART[usart],USART_IT_RXNE ) == SET )
    {
         *recive_byte = (uint16_t)(USART[usart]->DATAR & (uint16_t)0x01FF);
         CallBackFunction[usart].rx();
    }
}


void USART1_IRQHandler( void )
{
    HAL_UASRT_IRQ(HAL_USART1);
}
void USART2_IRQHandler( void )
{
    HAL_UASRT_IRQ(HAL_USART2);

}
void USART3_IRQHandler( void )
{
    HAL_UASRT_IRQ(HAL_USART3);
}
void UART4_IRQHandler( void )
{
    HAL_UASRT_IRQ(HAL_USART4);
}


#endif
