/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 task1 and task2 alternate printing
*/
#include "main.h"
#include "system_init.h"
#include "init.h"
/* Global define */


#define KEEPALIVE_ENABLE                1               //Enable keep alive function

                                   //source port

//u8 SocketIdForListen;                                   //Socket for Listening
//u8 socket[WCHNET_MAX_SOCKET_NUM];                       //Save the currently connected socket
//u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];  //socket receive buffer
//u8 MyBuf[RECE_BUF_LEN];
/* Global Variable */


EventGroupHandle_t xOSEventGroupHandle;
EventGroupHandle_t xADCEventGroupHandle;

EventGroupHandle_t  * xGetOSEvent()
{
    return (&xOSEventGroupHandle);
}

EventGroupHandle_t * xGetADCEvent()
{
    return (&xADCEventGroupHandle);
}
/*********************************************************************
 * @fn      GPIO_Toggle_INIT
 *
 * @brief   Initializes GPIOA.0/1
 *
 * @return  none
 */
void GPIO_Toggle_INIT(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure={0};

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);



  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS)
        return;
    printf("Error: %02X\r\n", (u16) iError);
}






/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */



u8g2_t u8g2_ks0108;




/* Structure that will hold the TCB of the task being created. */
//StaticTask_t xTaskBuffer;

/* Buffer that the task being created will use as its stack.  Note this is
an array of StackType_t variables.  The size of StackType_t is dependent on
the RTOS port. */
//StackType_t xStack[  TASK2_STK_SIZE  ];

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);
		
	printf("SystemClk:%d\r\n",SystemCoreClock);
	//printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	//printf("FreeRTOS Kernel Version:%s\r\n",tskKERNEL_VERSION_NUMBER);

	//GPIO_Toggle_INIT();


	vSYSqueueInit ( );
    vSYSeventInit ( );
    vSYStaskInit ( );
    vNetInit();
    vInit_DeviceConfig();
    vTaskStartScheduler();

  //  u8g2_Setup_ks0108_128x64_f(&u8g2_ks0108, U8G2_R0, 0, 0);
	while(1)
	{
	    printf("shouldn't run at here!!\n");
	}
}
