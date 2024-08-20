/*
 * system_init.c
 *
 *  Created on: Jul 8, 2024
 *      Author: i.dymov
 */
#include "system_init.h"


#include "init.h"
#include "adc.h"
#include "data_model.h"
#include "led.h"
#include "din_dout_task.h"
#include "mb_task.h"
#include "menu.h"
#include "hal_spi.h"
#include "hal_rtc.h"
#include "rtc_task.h"
#include "EEPROM_25C.h"

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



static StackType_t  SERIALTaskBuffer[SERIAL_STK_SIZE];
static StaticTask_t SERIALTaskControlBlock;

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


void vSYStaskInit ( void )
{
   (* getSerialTask())
           =    xTaskCreateStatic( StartUARTTask, "Serial", SERIAL_STK_SIZE , ( void * ) 1, SERIAL_TASK_PRIO  ,
                   (StackType_t * const )SERIALTaskBuffer, &SERIALTaskControlBlock );

  (* getLCDTaskHandle())
           =  xTaskCreateStatic( LCD_task, "LCD", LCD_STK_SIZE , ( void * ) 1, LCD_TASK_PRIOR  ,
                   (StackType_t * const )LCDTaskBuffer, &LCDTaskControlBlock );

  /*(*  getADCTaskHandle())
             = xTaskCreateStatic( ADC_task, "ADC", ADC_STK_SIZE , ( void * ) 1, ADC_TASK_PRIO  ,
                                    (StackType_t * const )ADCTaskBuffer, &ADCTaskControlBlock );*/
 MPTCPTask_Handler
  = xTaskCreateStatic( MBRTU_task, "MPTCP", MBTCP_STK_SIZE , ( void * ) 1, MBTCP_TASK_PRIO ,
                     (StackType_t * const )MBTCPTaskBuffer, &MBTCPTaskControlBlock );



 /*  WCHNETTask_Handler
  = xTaskCreateStatic( WCHNET_task, "MPTCP", WCHNET_STK_SIZE , ( void * ) 1,WCHNET_TASK_PRIO ,
                     (StackType_t * const )WCHNETTaskBuffer, &WCHNETTaskControlBlock );
*/
   KeyboarTask_Handler
   = xTaskCreateStatic( vKeyboardTask, "KEYBOARD", KEYBAORD_STK_SIZE , ( void * ) 1,KEYBAORD_TASK_PRIO ,
                      (StackType_t * const )KeyboarTaskBuffer, &KeyboardTaskControlBlock);

  DefautTask_Handler = xTaskCreateStatic( vDefaultTask, "DefTask", DEFAULT_TASK_STACK_SIZE , ( void * ) 1, DEFAULT_TASK_PRIOR, (StackType_t * const )defaultTaskBuffer, &defaultTaskControlBlock );

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


u8 SPI1_ReadWriteByte(u8 TxData)
{
    u8 i = 0;
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

    SPI_I2S_SendData(SPI2, TxData);
    i = 0;

    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

    return SPI_I2S_ReceiveData(SPI2);
}
#define W25X_ReadStatusReg       0x05

u8 WaitWileBusy()
{
    u8 data= 0;
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
    vTaskDelay(1);
    while(1)
    {
        while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
        SPI_I2S_SendData(SPI2, 0x05);
        while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
        data =  SPI_I2S_ReceiveData(SPI2);
        if ((data & 0x01) == 0 ) break;
    }
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
    return (data);
}

void Unprotect()
{

    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
                  vTaskDelay(1);
                  SPI1_ReadWriteByte(0x06 );
                  vTaskDelay(1);
                  GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);

}

u8 data[100]= {1,2,3,4,5,6,7,8,9,10};
u8 data1[100]= {0};

void vDefaultTask( void  * argument )
{
    uint32_t ulNotifiedValue;
    HAL_RTC_INIT_t RTC_INIT_TYPE;
    TaskFSM_t main_task_fsm = STATE_INIT;


    vMenuInit();
    while(1)
    {

       switch (main_task_fsm)
        {
            case STATE_INIT:
                if (DataModel_Init()!=NORMAL_INIT)
                {
                    RTC_INIT_TYPE = HAL_RTC_NEW_INIT;
                }
                else
                {
                    RTC_INIT_TYPE = HAL_RTC_NORMAL_INIT;
                }
                vRTCTaskInit(RTC_INIT_TYPE);
                printf("System start\r\n");
                main_task_fsm =  STATE_RUN;
                break;
            case STATE_RUN:
                vMenuTask();

                xTaskNotifyIndexed(*(getLCDTaskHandle()), 0, 0x01, eSetValueWithOverwrite);
                vTaskDelay(100);
                break;
            case STATE_SAVE_DATA:

                ReadEEPROMData( 0, data1,100, 100, 2);
                for (u8 i=0;i<100;i++)
                {
                   printf("%i\r\n",data1[i]);
                }



                //WriteEEPROM(HAL_SPI2, 0, data,100, 100, 2);
                for (u8 i=0;i<100;i++)
                                {
                                  data[i]=i+50;
                                }
                /*byte = WaitWileBusy();
                if (byte & 0x02) Unprotect();
                vTaskDelay(1);
                WaitWileBusy();
                GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
                vTaskDelay(1);

                    SPI1_ReadWriteByte(0x02 );
                    SPI1_ReadWriteByte(0x00 );
                    SPI1_ReadWriteByte(0x01 );
                    SPI1_ReadWriteByte(0x55 );
                    SPI1_ReadWriteByte(0xAA );

                vTaskDelay(1);
                GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
                vTaskDelay(5);
                GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
                vTaskDelay(1);
             //   SPI1_ReadWriteByte(0x05);
             //   byte = SPI1_ReadWriteByte(0Xff);
              //  if ((byte & 0x01) == 0)
                {
                              SPI1_ReadWriteByte(0x03 );
                              SPI1_ReadWriteByte(0x00 );
                              SPI1_ReadWriteByte(0x01 );
                              data[0] = SPI1_ReadWriteByte(0xFF );
                              data[1] = SPI1_ReadWriteByte(0xFF );
                }

                vTaskDelay(1);
                GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);*/
                break;

               // if( SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_RXNE ) != RESET )
               // {
              //      u8 data = SPI_I2S_ReceiveData(SPI2);
              //      main_task_fsm = 1;
             //   }

               // DataModel_Init();
              //  ReadEEPROMData(0x00 ,10 , data, 10 ,2);
             //   main_task_fsm = STATE_WHAIT_TO_RAEDY;
            //    break;
          //  case STATE_WHAIT_TO_RAEDY:
            //    xTaskNotifyWait(0, 0, &ulNotifiedValue,portMAX_DELAY);
            //    if ( ulNotifiedValue == 2)
            //    {
             //                    ulTaskNotifyValueClearIndexed(0, 0, 0xFFFF);
                //                 vTaskResume( *xCanOpenProcessTaskHandle());
                //                 vTaskResume( *xCanOpenPeriodicTaskHandle ());
            //        main_task_fsm = STATE_RUN;
            //    }
            //    break;
           // case STATE_RUN:

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
