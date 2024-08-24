/*
 * system_init.c
 *
 *  Created on: Jul 8, 2024
 *      Author: i.dymov
 */
#include "system_init.h"


static void vDefaultTask( void  * argument );
static void WCHNET_task(void *pvParameters);


static StaticTask_t xIdleTaskTCB;
static StaticTask_t xTimerTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];


static TaskHandle_t WCHNETTask_Handler;
static StackType_t WCHNETTaskBuffer[WCHNET_STK_SIZE];
static StaticTask_t WCHNETTaskControlBlock;


static TaskHandle_t KeyboarTask_Handler;
static StackType_t  KeyboarTaskBuffer[KEYBAORD_STK_SIZE];
static StaticTask_t KeyboardTaskControlBlock;


static StackType_t  I2CTaskBuffer[I2C_STK_SIZE];
static StaticTask_t I2CTaskControlBlock;

static StackType_t  SERIALTaskBuffer[SERIAL_STK_SIZE];
static StaticTask_t SERIALTaskControlBlock;

static StackType_t LCDTaskBuffer[MBTCP_STK_SIZE];
static StaticTask_t LCDTaskControlBlock;


static StackType_t USERTaskBuffer[USER_STK_SIZE];
static StaticTask_t USERTaskControlBlock;

static TaskHandle_t MPTCPTask_Handler;
static StackType_t MBTCPTaskBuffer[MBTCP_STK_SIZE];
static StaticTask_t MBTCPTaskControlBlock;

static StackType_t defaultTaskBuffer[DEFAULT_TASK_STACK_SIZE];
static StaticTask_t defaultTaskControlBlock;
static TaskHandle_t DefautTask_Handler;

static StackType_t ADCTaskBuffer[ADC_STK_SIZE];
static StaticTask_t ADCTaskControlBlock;

uint8_t ucQueueStorageArea[  16U * sizeof( KeyEvent ) ];
static StaticQueue_t xStaticQueue;

static StaticEventGroup_t xOSStateEventGroup;
static StaticEventGroup_t xSerialStateEventGroup;

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
   *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
   *ppxIdleTaskStackBuffer = uxIdleTaskStack;
   *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
   *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
   *ppxTimerTaskStackBuffer = uxTimerTaskStack;
   *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}


void TaskSuspend()
{
    vTaskSuspend( WCHNETTask_Handler );
    vTaskSuspend( MPTCPTask_Handler  );
    vTaskSuspend(* getSerialTask());
    //vTaskSuspend(*  getADCTaskHandle());
}

void vSYStaskInit ( void )
{

    (* getI2CTaskHandle())

                    =    xTaskCreateStatic( I2C_task, "I2C", I2C_STK_SIZE , ( void * ) 1, I2C_TASK_PRIO  ,
                                      (StackType_t * const )I2CTaskBuffer, &I2CTaskControlBlock );

   (* getSerialTask())
           =    xTaskCreateStatic( StartUARTTask, "Serial", SERIAL_STK_SIZE , ( void * ) 1, SERIAL_TASK_PRIO  ,
                   (StackType_t * const )SERIALTaskBuffer, &SERIALTaskControlBlock );

  (* getLCDTaskHandle())
           =  xTaskCreateStatic( LCD_task, "LCD", LCD_STK_SIZE , ( void * ) 1, LCD_TASK_PRIOR  ,
                   (StackType_t * const )LCDTaskBuffer, &LCDTaskControlBlock );

  (*  getADCTaskHandle())
             = xTaskCreateStatic( ADC_task, "ADC", ADC_STK_SIZE , ( void * ) 1, ADC_TASK_PRIO  ,
                                    (StackType_t * const )ADCTaskBuffer, &ADCTaskControlBlock );
  MPTCPTask_Handler
  = xTaskCreateStatic( MBRTU_task, "MPTCP", MBTCP_STK_SIZE , ( void * ) 1, MBTCP_TASK_PRIO ,
                     (StackType_t * const )MBTCPTaskBuffer, &MBTCPTaskControlBlock );

  (*getUserProcessTaskHandle())
        = xTaskCreateStatic( user_process_task, "user", USER_STK_SIZE , ( void * ) 1, USER_TASK_PRIO,
                (StackType_t * const )USERTaskBuffer, &USERTaskControlBlock );


   WCHNETTask_Handler
  = xTaskCreateStatic( WCHNET_task, "MPTCP", WCHNET_STK_SIZE , ( void * ) 1,WCHNET_TASK_PRIO ,
                     (StackType_t * const )WCHNETTaskBuffer, &WCHNETTaskControlBlock );

   KeyboarTask_Handler
   = xTaskCreateStatic( vKeyboardTask, "KEYBOARD", KEYBAORD_STK_SIZE , ( void * ) 1,KEYBAORD_TASK_PRIO ,
                      (StackType_t * const )KeyboarTaskBuffer, &KeyboardTaskControlBlock);

  DefautTask_Handler = xTaskCreateStatic( vDefaultTask, "DefTask", DEFAULT_TASK_STACK_SIZE , ( void * ) 1, DEFAULT_TASK_PRIOR, (StackType_t * const )defaultTaskBuffer, &defaultTaskControlBlock );
  TaskSuspend();
  return;
}

void vSYSeventInit ( void )
{
  *(xGetOSEvent() ) = xEventGroupCreateStatic(&xOSStateEventGroup );
  *(getSerialEvenGroup()) = xEventGroupCreateStatic(&xSerialStateEventGroup );
}


void vSYSqueueInit ( void )
{
     *( xKeyboardQueue()) = xQueueCreateStatic( 16U, sizeof( KeyEvent ),ucQueueStorageArea, &xStaticQueue );
}





void vDefaultTask( void  * argument )
{
    u8 control_type;
    uint32_t ulNotifiedValue;
    HAL_RTC_INIT_t RTC_INIT_TYPE;
    TaskFSM_t main_task_fsm = STATE_INIT;
    vMenuInit();
    while(1)
    {

       switch (main_task_fsm)
        {
            case STATE_INIT:
                vDrawBitmap();
                xTaskNotifyIndexed(*(getLCDTaskHandle()), 0, 0x01, eSetValueWithOverwrite);
                if (DataModel_Init()!=NORMAL_INIT)
                {
                    RTC_INIT_TYPE = HAL_RTC_NEW_INIT;
                }
                else
                {
                    RTC_INIT_TYPE = HAL_RTC_NORMAL_INIT;
                }
                vRTC_TASK_Init(RTC_INIT_TYPE);
                main_task_fsm =  STATE_WHAIT_TO_RAEDY;
                vTaskDelay(3000);
                break;
            case STATE_WHAIT_TO_RAEDY:

                control_type  = getReg8(CONTROL_TYPE );
                if (control_type == MB_DIN )  control_type  = getReg8(MB_PROTOCOL_TYPE);
                if (control_type == MB_RTU)
                {
                    vTaskResume(* getSerialTask());
                    vTaskResume( MPTCPTask_Handler  );
                }
                else
                {
                    vTaskResume( WCHNETTask_Handler );
                    vTaskResume( MPTCPTask_Handler  );
                }
                main_task_fsm  = STATE_RUN;
                break;
            case STATE_RUN:
                vMenuTask();
                xTaskNotifyIndexed(*(getLCDTaskHandle()), 0, 0x01, eSetValueWithOverwrite);
                vTaskDelay(100);
                break;
        }
  }


}




/*********************************************************************
 * @fn      task2_task
 *
 * @brief   task2 program.
 *
 * @param  *pvParameters - Parameters point of task2
 *
 * @return  none
 */
void WCHNET_task(void *pvParameters)
{
    while(1)
    {
        WCHNET_MainTask();
               /*Query the Ethernet global interrupt,
                * if there is an interrupt, call the global interrupt handler*/
        if(WCHNET_QueryGlobalInt())
        {
                 WCHNET_HandleGlobalInt();
        }
        vTaskDelay(1);
    }
}
