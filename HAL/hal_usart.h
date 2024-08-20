/*
 * hal_usart.h
 *
 *  Created on: Aug 14, 2024
 *      Author: smoke
 */

#ifndef HAL_HAL_USART_H_
#define HAL_HAL_USART_H_

#include "main.h"
#include "hal_config.h"

typedef enum
{
    HAL_USART1 = 0,
    HAL_USART2 = 1,
    HAL_USART3 = 2,
    HAL_USART4 = 3,
} HAL_USART_t;

typedef struct
{
    void (* rx)( void );
    void (* tx)( void );

} USART_CallBack_t;

#if MCU == CH32V2 || MCU == CH32V3

typedef enum
{
   HAL_StopBits_1      = USART_StopBits_1,
   HAL_StopBits_0_5    = USART_StopBits_0_5 ,
   HAL_StopBits_2      = USART_StopBits_2,
   HAL_StopBits_1_5    = USART_StopBits_1_5,
} HAL_USART_STOP_BIT_t;

typedef enum
{
    HAL_Parity_No  = USART_Parity_No,
    HAL_Parity_Even  = USART_Parity_Even,
    HAL_Parity_Odd  = USART_Parity_Odd,
} HAL_USART_PARITY_t;

typedef enum
{
    UART_WORDLENGTH_8B = USART_WordLength_8b,
    UART_WORDLENGTH_9B  =USART_WordLength_9b,
} HAL_USART_WL_t;


#endif

void HALUSARTInit(HAL_USART_t usart, uint32_t USART_BaudRate, HAL_USART_STOP_BIT_t USART_StopBits ,HAL_USART_PARITY_t USART_Parity ,HAL_USART_WL_t USART_wordlength   );
void HALUSARTEnable(HAL_USART_t usart);
void HALUSARTInitIT(HAL_USART_t usart, void (* rx_it_callback) ( void ), void (* tx_it_callback) ( void ), uint8_t prior, uint8_t subprior);
void HAL_SendByte_IT(HAL_USART_t usart, u8 data );
void HAL_RecieveByte_IT(HAL_USART_t usart , u8 * rb);
void HAL_RecieveITDisable(HAL_USART_t usart);
#endif /* HAL_HAL_USART_H_ */
