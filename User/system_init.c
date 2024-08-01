/*
 * system_init.c
 *
 *  Created on: Jul 8, 2024
 *      Author: i.dymov
 */
#include "system_init.h"
#include "hw_lib_keyboard.h"
#include "hw_lib_din.h"
#include "init.h"
#include "adc.h"
#include "data_model.h"
#include "led.h"
static void vKeyboardTask(void  * argument );
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



static StackType_t LCDTaskBuffer[MBTCP_STK_SIZE];
static StaticTask_t LCDTaskControlBlock;


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
static StaticEventGroup_t xADCStateEventGroup;
static StaticEventGroup_t xOSStateEventGroup;

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


void vSYStaskInit ( void )
{
   (* getLCDTaskHandle())
           =  xTaskCreateStatic( LCD_task, "LCD", LCD_STK_SIZE , ( void * ) 1, LCD_TASK_PRIOR  ,
                   (StackType_t * const )LCDTaskBuffer, &LCDTaskControlBlock );

   (*  getADCTaskHandle())
             = xTaskCreateStatic( ADC_task, "ADC", ADC_STK_SIZE , ( void * ) 1, ADC_TASK_PRIO  ,
                                     (StackType_t * const )ADCTaskBuffer, &ADCTaskControlBlock );
   MPTCPTask_Handler
  = xTaskCreateStatic( MBTCP_task, "MPTCP", MBTCP_STK_SIZE , ( void * ) 1, MBTCP_TASK_PRIO ,
                     (StackType_t * const )MBTCPTaskBuffer, &MBTCPTaskControlBlock );

   WCHNETTask_Handler
  = xTaskCreateStatic( WCHNET_task, "MPTCP", WCHNET_STK_SIZE , ( void * ) 1,WCHNET_TASK_PRIO ,
                     (StackType_t * const )WCHNETTaskBuffer, &WCHNETTaskControlBlock );

   KeyboarTask_Handler
   = xTaskCreateStatic( vKeyboardTask, "KEYBOARD", KEYBAORD_STK_SIZE , ( void * ) 1,KEYBAORD_TASK_PRIO ,
                      (StackType_t * const )KeyboarTaskBuffer, &KeyboardTaskControlBlock);
  DefautTask_Handler = xTaskCreateStatic( vDefaultTask, "DefTask", DEFAULT_TASK_STACK_SIZE , ( void * ) 1, DEFAULT_TASK_PRIOR, (StackType_t * const )defaultTaskBuffer, &defaultTaskControlBlock );

  return;
}


void vSYSeventInit ( void )
{
  *(xGetOSEvent() ) = xEventGroupCreateStatic(&xOSStateEventGroup );
}


void vSYSqueueInit ( void )
{
     *( xKeyboardQueue()) = xQueueCreateStatic( 16U, sizeof( KeyEvent ),ucQueueStorageArea, &xStaticQueue );
}

BitState_t fPortState (uint8_t i)
{
    switch (i)
    {
        case 0:
            return HAL_GetBit( KL1_Port, KL1_Pin  );
        case 1:
            return HAL_GetBit( KL_Port, KL2_Pin  );
        case 2:
            return HAL_GetBit( KL_Port, KL3_Pin  );
        case 3:
            return HAL_GetBit( KL_Port, KL4_Pin  );
        case 4:
            return HAL_GetBit( KL_Port, KL5_Pin  );
        case 5:
            return HAL_GetBit( KL_Port, KL6_Pin  );

        default:
            return 0;
    }
}
#define KEY_COUNT 6
static uint8_t  STATUS[KEY_COUNT];
static uint8_t  COUNTERS[KEY_COUNT];


void vKeyboarInit()
{
    KeybaordStruct_t KeyboardInit;
    KeyboardInit.KEYBOARD_COUNT    = KEY_COUNT;
    KeyboardInit.COUNTERS          = COUNTERS;
    KeyboardInit.STATUS            = STATUS;
    KeyboardInit.REPEAT_TIME       = 2;
    KeyboardInit.KEYDOWN_HOLD_TIME = 3;
    KeyboardInit.KEYDOWN_DELAY     = 1;
    KeyboardInit.KEYBOARD_PERIOD   = 20;
    KeyboardInit.getPortCallback = &fPortState;
    eKeyboardInit(&KeyboardInit);
}

/*
 *
 */
uint8_t fDinStateCallback (uint8_t i)
{
    switch ( i)
    {
        case INPUT_1:
            return HAL_GetBit( DIN_1_5_Port  ,DIN_1_Pin );
        case INPUT_2:
            return HAL_GetBit( DIN_1_5_Port  ,DIN_2_Pin );
        case INPUT_3:
            return HAL_GetBit( DIN_1_5_Port  ,DIN_3_Pin );
        case INPUT_4:
            return HAL_GetBit( DIN_1_5_Port  ,DIN_4_Pin );
        case INPUT_5:
            return HAL_GetBit( DIN_1_5_Port  ,DIN_5_Pin );
        default:
            return (RESET);
    }
}
/*
 *
 */
void  vSetDoutState( OUT_NAME_TYPE ucCh, u8 BitVal )
{
    switch (ucCh )
    {
           case OUT_1:
               if ( BitVal == RESET)
                     HAL_SetBit(DOUT_Port,DOUT_1_Pin  );
                 else
                     HAL_ResetBit(DOUT_Port,DOUT_1_Pin );
                break;
           case OUT_2:
               if ( BitVal == RESET)
                   HAL_SetBit(DOUT_Port,DOUT_2_Pin  );
               else
                  HAL_ResetBit(DOUT_Port,DOUT_2_Pin );
                               break;
           case OUT_3:
               if ( BitVal == RESET)
                   HAL_SetBit(DOUT_Port,DOUT_3_Pin  );
               else
                   HAL_ResetBit(DOUT_Port,DOUT_3_Pin );
               break;
           default:
               break;
       }
}


void vDIN_DOUT_Init()
{
    DoutCinfig_t  DOUT_CONFIG;
    DinConfig_t DIN_CONFIG;
    DIN_CONFIG.eInputType = DIN_CONFIG_POSITIVE;
    DIN_CONFIG.ulHighCounter = DEF_H_FRONT;
    DIN_CONFIG.ulLowCounter  = DEF_L_FRONT;
    DIN_CONFIG.getPortCallback = &fDinStateCallback;
    eDinConfigWtihStruct(INPUT_1,&DIN_CONFIG);
    eDinConfigWtihStruct(INPUT_2,&DIN_CONFIG);
    eDinConfigWtihStruct(INPUT_3,&DIN_CONFIG);
    eDinConfigWtihStruct(INPUT_4,&DIN_CONFIG);
    eDinConfigWtihStruct(INPUT_5,&DIN_CONFIG);
    DIN_CONFIG.eInputType = DIN_CONFIG_POSITIVE;
    DOUT_CONFIG.setPortCallback =&vSetDoutState;
    eDOUTConfigWtihStruct( OUT_1, &DOUT_CONFIG);
    eDOUTConfigWtihStruct( OUT_2, &DOUT_CONFIG);
    eDOUTConfigWtihStruct( OUT_3, &DOUT_CONFIG);
}

void vKeyboardTask(void  * argument )
{
    vKeyboarInit();
    vDIN_DOUT_Init();
    for(;;)
    {
       vTaskDelay( HW_LIB_GetKeyboardPeriod() );
       HW_LIB_KeyboradFSM();
       vDinDoutProcess();
    }
}





void vDefaultTask( void  * argument )
{
    uint32_t ulNotifiedValue;
    TaskFSM_t main_task_fsm = STATE_INIT;
    while(1)
    {
        switch (main_task_fsm)
        {
            case STATE_INIT:
                DataModel_Init();
                main_task_fsm = STATE_WHAIT_TO_RAEDY;
                break;
            case STATE_WHAIT_TO_RAEDY:
                xTaskNotifyWait(0, 0, &ulNotifiedValue,portMAX_DELAY);
                if ( ulNotifiedValue == 2)
                {
                                 ulTaskNotifyValueClearIndexed(0, 0, 0xFFFF);
                //                 vTaskResume( *xCanOpenProcessTaskHandle());
                //                 vTaskResume( *xCanOpenPeriodicTaskHandle ());
                    main_task_fsm = STATE_RUN;
                }
                break;
            case STATE_RUN:
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
