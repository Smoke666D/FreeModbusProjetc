/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

#include "mb.h"
#include "mbport.h"
#include "port.h"
#include "hal_gpio.h"
#include "init.h"
#include "hal_usart.h"
#include "init.h"
//#include "mainFSM.h"

static void HAL_UART_TxCpltCallback(void);
static void HAL_UART_RxCpltCallback(void);

/* ----------------------- Static variables ---------------------------------*/

static EventGroupHandle_t xSerialEventGroupHandle;
static uint8_t rx_data_buf;
static uint8_t rx_data;


//ALIGN(RT_ALIGN_SIZE)
/* software simulation serial transmit IRQ handler thread stack */
//static rt_uint8_t serial_soft_trans_irq_stack[512];
/* software simulation serial transmit IRQ handler thread */
//static struct rt_thread thread_serial_soft_trans_irq;
/* serial event */
//static struct rt_event event_serial;
/* modbus slave serial device */
//static rt_serial_t *serial;

/* ----------------------- Defines ------------------------------------------*/
/* serial transmit event */
#define EVENT_SERIAL_TRANS_START    (1<<0)

/* ----------------------- static functions ---------------------------------*/
//static void prvvUARTTxReadyISR(void);
//static void prvvUARTRxISR(void);
//static rt_err_t serial_rx_ind(rt_device_t dev, rt_size_t size);
//static void serial_soft_trans_irq(void* parameter);

static void vReviceEnable()
{
    HAL_ResetBit(RS485_EN_Port ,RS485_EN_Pin);

}
static void vTransmitEnable()
{
    HAL_SetBit(RS485_EN_Port ,RS485_EN_Pin);

}

static HAL_USART_t MBPORT;

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
    HAL_USART_PARITY_t USART_Parity;
    HAL_USART_WL_t USART_wordlength;
    switch(eParity)
    {
    	case MB_PAR_NONE:
    	    USART_wordlength= UART_WORDLENGTH_8B;
    		USART_Parity = HAL_Parity_No;
    		break;
    	case MB_PAR_ODD:
    	    USART_wordlength= UART_WORDLENGTH_9B;
    		USART_Parity = HAL_Parity_Odd;
    	  	break;
    	case MB_PAR_EVEN:
    	    USART_wordlength= UART_WORDLENGTH_9B;
    		USART_Parity = HAL_Parity_Even;
    		break;
    }
    MBPORT = (ucPORT == 1)? HAL_USART1 : HAL_USART2;
    HALUSARTInit(MBPORT, ulBaudRate, HAL_StopBits_1 ,USART_Parity , USART_wordlength  );
    HALUSARTInitIT(MBPORT,&HAL_UART_RxCpltCallback , &HAL_UART_TxCpltCallback, UART2_IT_PRIOR,  UART2_IT_SUBPRIOR);
    HALUSARTEnable(MBPORT);
    return TRUE;
}

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
//    rt_uint32_t recved_event;
    if (xRxEnable)
    {

    	HAL_UART_Receive_IT(&huart1,&rx_data_buf,1);
    	//HAL_HalfDuplex_EnableReceiver(&huart1);
        /* enable RX interrupt */
  //      serial->ops->control(serial, RT_DEVICE_CTRL_SET_INT, (void *)RT_DEVICE_FLAG_INT_RX);
        /* switch 485 to receive mode */
    	vReviceEnable();

    }
    else
    {
        /* switch 485 to transmit mode */
    	vTransmitEnable();
        /* disable RX interrupt */
    	HAL_UART_AbortReceive_IT(&huart1);
     //   serial->ops->control(serial, RT_DEVICE_CTRL_CLR_INT, (void *)RT_DEVICE_FLAG_INT_RX);
    }
    if (xTxEnable)
    {
    	//HAL_HalfDuplex_EnableTransmitter(&huart1);

    	xEventGroupSetBits(xSerialEventGroupHandle,EVENT_SERIAL_TRANS_START);
        /* start serial transmit */
       // rt_event_send(&event_serial, EVENT_SERIAL_TRANS_START);
    }
    else
    {
        /* stop serial transmit */
    	xEventGroupWaitBits( xSerialEventGroupHandle,  EVENT_SERIAL_TRANS_START,  pdTRUE, pdFALSE, 0 );
        //rt_event_recv(&event_serial, EVENT_SERIAL_TRANS_START,
        //        RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, 0,
         //       &recved_event);
    }
}

void vMBPortClose(void)
{
    //serial->parent.close(&(serial->parent));
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
    HAL_SetBit(RS485_EN_Port ,RS485_EN_Pin);
    HAL_SendByte_IT(MBPORT, ucByte );
    return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{
	* pucByte = rx_data;
    //serial->parent.read(&(serial->parent), 0, pucByte, 1);
    return TRUE;
}



static void HAL_UART_TxCpltCallback()
{

	BaseType_t xHigherPriorityTaskWoken, xResult;
	/* xHigherPriorityTaskWoken must be initialised to pdFALSE. */
	xHigherPriorityTaskWoken = pdFALSE;

	 /* Set bit 0 and bit 4 in xEventGroup. */
	 xResult = xEventGroupSetBitsFromISR(
			 	 	 	 	 	   xSerialEventGroupHandle,   /* The event group being updated. */
								   EVENT_SERIAL_TRANS_START, /* The bits being set. */
	                              &xHigherPriorityTaskWoken );

	  /* Was the message posted successfully? */
	  if( xResult != pdFAIL )
	  {
	      /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
	      switch should be requested.  The macro used is port specific and will
	      be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
	      the documentation page for the port being used. */
	      portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	  }
}



void StartUARTTask(void *argument)
{
	  EventBits_t uxBits;
	  /* Attempt to create the event group. */
	  xSerialEventGroupHandle = xGetUARTEvent();
	  while(1)
	  {
		  uxBits = xEventGroupWaitBits( xSerialEventGroupHandle,  EVENT_SERIAL_TRANS_START,  pdTRUE, pdFALSE, portMAX_DELAY );
		  switch (uxBits)
		  {
		  	  	  case EVENT_SERIAL_TRANS_START:
		  	  		  pxMBFrameCBTransmitterEmpty();
		  	  		  break;
		  	  	  default:
		  	  		  break;
		  }
	  }


}


static void HAL_UART_RxCpltCallback()
{
	rx_data = rx_data_buf;

	HAL_UART_Receive_IT(&huart1,&rx_data_buf,1);
	pxMBFrameCBByteReceived();
}



