/*
 * system_init.c
 *
 *  Created on: Jul 8, 2024
 *      Author: i.dymov
 */
#include "system_init.h"
#include "hal_wdt.h"
#include "user_process.h"

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
    vTaskSuspend( MPTCPTask_Handler  );
    vTaskSuspend(* getSerialTask());
    vTaskSuspend(* getUserProcessTaskHandle());
    vTaskSuspend(*getI2CTaskHandle());

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

static DEVICE_TYPE_t device;

DEVICE_TYPE_t SystemInitGetDevType()
{
    return (device);
}


static u8 TCP_STOP = 0;

#define EXIT_KEY_PRESS 0x01
#define LEFT_KEY_PRESS 0x02

void vDefaultTask( void  * argument )
{
    u8 init_menu_state = 0;
    QueueHandle_t     pKeyboard        = *( xKeyboardQueue());
    static KeyEvent          TempEvent        = { 0U };
    char temp_str[50];
    u8 buffer_draw_counter = 0;
    TaskFSM_t main_task_fsm = STATE_INIT;
    u8 contrast = 0;
    vMenuInit();
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
    while(1)
    {
       if ( contrast != getReg8(CONTRAST))
       {
           contrast = getReg8(CONTRAST);
           u16 cc = (u16)((4095.0)*((100-contrast)/100.0));
           DAC_SetChannel1Data(DAC_Align_12b_R, cc);
       }
       switch (main_task_fsm)
        {
            case STATE_INIT:
                vDrawBitmap();
                xTaskNotifyIndexed(*(getLCDTaskHandle()), 0, 0x01, eSetValueWithOverwrite);
                device = getReg8(DEVICE_TYPE);
                for (uint16_t k=0; k< 3000;k++)
                {
                    if ( uxQueueMessagesWaiting(pKeyboard) != 0)
                    {
                        if ( xQueueReceive(pKeyboard, &TempEvent, 0U ) == pdPASS )
                        {


                            if (TempEvent.KeyCode ==EXIT_KEY)
                            {
                                if ( TempEvent.Status == MAKECODE ) init_menu_state |=EXIT_KEY_PRESS ;
                                else
                                    init_menu_state &= ~EXIT_KEY_PRESS ;

                            }
                            if (TempEvent.KeyCode ==LEFT_KEY)
                            {
                                if ( TempEvent.Status == MAKECODE ) init_menu_state |=LEFT_KEY_PRESS ;
                                else
                                    init_menu_state &= ~LEFT_KEY_PRESS ;
                            }
                            if (init_menu_state == (LEFT_KEY_PRESS | EXIT_KEY_PRESS )  )
                            {
                                  MenuSetDeviceMenu();
                                  main_task_fsm  = STATE_RUN;
                                  break;
                            }
                        }
                    }
                    HAL_WDTReset();
                    vTaskDelay(1);
                    if (k==2000)
                    {
                        MENU_ClearScreen();
                        strcpy(temp_str,getModeString(device));
                        u8 len = u8g2_GetUTF8Width(&u8g2,temp_str);
                        MENU_DrawString((128-len)/2, 20,temp_str);
                        MenuSetDevice();
                        sprintf(temp_str,"Версия ПО %02i.%02i.%02i",getReg8(SOFT_V1 ),getReg8(SOFT_V2 ),getReg8(SOFT_V3 ));
                        len = u8g2_GetUTF8Width(&u8g2,temp_str);
                        MENU_DrawString((128-len)/2, 40, temp_str);
                        xTaskNotifyIndexed(*(getLCDTaskHandle()), 0, 0x01, eSetValueWithOverwrite);
                        vTaskResume(*getUserProcessTaskHandle());
                        vTaskResume(*getI2CTaskHandle());
                    }
                }
                if  (main_task_fsm  != STATE_RUN)
                main_task_fsm =  STATE_WHAIT_TO_RAEDY;
                printf("menu_run\r\n");
                break;
            case STATE_WHAIT_TO_RAEDY:
                if (getReg8(MB_PROTOCOL_TYPE) == MKV_MB_RTU)
                {
                    vTaskResume(* getSerialTask());
                    TCP_STOP = 1;
                }
                setWorkMode();
                vTaskResume( MPTCPTask_Handler  );
                main_task_fsm  = STATE_RUN;
                break;
            case STATE_RUN:
                HAL_WDTReset();
                vMenuTask();
                if (++buffer_draw_counter>2)
                {
                    buffer_draw_counter = 0;
                    if (LED_BufferCompare())
                    {
                        xTaskNotifyIndexed(*(getLCDTaskHandle()), 0, 0x01, eSetValueWithOverwrite);
                    }
                }
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
    TCP_STOP  = 0;
    DataModel_Init();
    vRTC_TASK_Init();
    xTaskNotifyGive( DefautTask_Handler );
    while(1)
    {
        if (TCP_STOP==0)
        {
            WCHNET_MainTask();
              /*Query the Ethernet global interrupt,
                * if there is an interrupt, call the global interrupt handler*/
            if(WCHNET_QueryGlobalInt())
            {
                WCHNET_HandleGlobalInt();
            }
        }
      //  HAL_WDTReset();
        vTaskDelay(1);
    }
}
