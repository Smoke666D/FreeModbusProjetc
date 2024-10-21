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
#include "menu.h"
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


EventGroupHandle_t * xGetADCEvent()
{
    return (&xADCEventGroupHandle);
}


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */


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
	vInit_DeviceConfig();
	USART_Printf_Init(115200);
	vRTC_Init();
	vSYSqueueInit ( );
    vSYSeventInit ( );
    vSYStaskInit ( );
    vNetInit();
    printf("System start\n");
    vTaskStartScheduler();
	while(1)
	{

	}
}
