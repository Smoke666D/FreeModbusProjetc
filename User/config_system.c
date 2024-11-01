/*
 * config_system.c
 *
 *  Created on: Nov 1, 2024
 *      Author: i.dymov
 */


#include "config_system.h"
#include "data_model.h"
#include "hal_usart.h"
#include "ch32v30x_crc.h"

static TaskHandle_t  processTaskHandle;
static QueueHandle_t pRXQueue;

QueueHandle_t * xConfigSystemRxQueue( void )
{
  return  (&pRXQueue);
}

TaskHandle_t * getConfigTaskHandle()
{
    return (&processTaskHandle);
}


static uint8_t rx_data_buf;
static uint8_t rx_data;

CONFIG_SYSTEM_ERROR_t SetConfigData( ConfigDataPacket_t data)
{

   u32 temp_int;
   float * ftemp;
   if (data.reg_addres <= EEPROM_REGISTER_COUNT)
   {
       switch (data.type)
       {
           case Byte_Config_Data:
               setReg8(data.reg_addres,data.data[0]);
               break;
           case Short_Config_Data:
               setReg16(data.reg_addres,(data.data[0]<<8) ||  (data.data[1]));
               break;
           case Float_Config_Data:
               temp_int =( data.data[0]<<24 | data.data[1]<<16 |  data.data[2]<<8 | data.data[0]);
               ftemp = (float *) &temp_int;
               saveRegFloat(data.reg_addres,*ftemp);
               break;
       }
       return (CONFIG_OK );
  }
   else
   {
       return (ILLEGAL_CONFIG_ADDRES);
   }

}


static void HAL_UART_RxCpltCallback()
{
    BaseType_t xHigherPriorityTaskWoken;
    rx_data = rx_data_buf;
    xQueueSendFromISR( pRXQueue, &rx_data, &xHigherPriorityTaskWoken );
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void HAL_UART_TxCpltCallback()
{
    BaseType_t xHigherPriorityTaskWoken;
    vTaskNotifyGiveFromISR( processTaskHandle, &xHigherPriorityTaskWoken );
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


void SendAnswer(CONFIG_SYSTEM_RX_STATUS_t status)
{
    HAL_SendByte_IT(HAL_USART2, (status == RECIVE_STATUS_OK) ? 0x44 : 0x55 );
    ulTaskNotifyTake(  pdTRUE, portMAX_DELAY );
}

void config_process_task(void *pvParameters)
{
     u8 recive_index = 0;
     BaseType_t RES;
     static CONFIG_RECIVE_FMS_t CONFIG_FSM = CONFIG_RECIVE_START;
     RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
     HALUSARTInit  (HAL_USART2, 115200, HAL_StopBits_1 ,MB_PAR_EVEN , UART_WORDLENGTH_9B );
     HALUSARTInitIT(HAL_USART2, &HAL_UART_RxCpltCallback , &HAL_UART_TxCpltCallback, UART2_IT_PRIOR,  UART2_IT_SUBPRIOR);
     HALUSARTEnable(HAL_USART2);
     u32 sdata[3];
     u8 * data = (u8 *)&sdata;
     while(1)
     {
         RES = xQueueReceive(pRXQueue, &data[recive_index], 10);
         if (RES== pdFALSE)
         {
             CONFIG_FSM = CONFIG_RECIVE_START;
             recive_index = 0;
         }
         switch (CONFIG_FSM)
         {
             case CONFIG_RECIVE_START:

                 if (data[recive_index] == CONFIG_START_BYTE)
                 {
                     CONFIG_FSM = CONFIG_RECIVE_TYPE;
                     recive_index =  1;
                 }
                 else
                 {
                     SendAnswer(RECIVE_STATUS_FAIL);
                     recive_index  = 0;
                 }
                 break;
             case CONFIG_RECIVE_TYPE:
                 if (data[recive_index]>  Float_Config_Data)
                 {
                     SendAnswer(RECIVE_STATUS_FAIL);
                     CONFIG_FSM = CONFIG_RECIVE_START;
                     recive_index  = 0;
                 }
                 else
                 {
                     CONFIG_FSM = CONFIG_RECIVE_ADDR;
                     recive_index = 2 ;
                 }
                 break;
             case CONFIG_RECIVE_ADDR:
                 recive_index++;
                 if (recive_index > 4 )
                 {
                     CONFIG_FSM = CONFIG_RECIVE_DATA;
                 }
                 break;
             case CONFIG_RECIVE_DATA:
                 recive_index++;
                 if (recive_index > 8 )
                  {
                    CONFIG_FSM = CONIFG_RECIVE_CRC;
                 }
                 break;
             case CONIFG_RECIVE_CRC:
                 recive_index++;
                 if (recive_index > 12 )
                 {
                     CRC_ResetDR();
                     CRC_CalcBlockCRC(sdata, 2);
                     if (CRC_GetCRC() == data[2])
                      {
                         ConfigDataPacket_t pcket;
                         pcket.type = data[1];
                         pcket.reg_addres = data[2]<<8 | data[3];
                         pcket.data[0]= data[4];
                         pcket.data[1]= data[5];
                         pcket.data[2]= data[6];
                         pcket.data[3]= data[7];
                         if (SetConfigData(pcket) == CONFIG_OK )
                             SendAnswer(RECIVE_STATUS_OK);
                         else
                             SendAnswer(RECIVE_STATUS_FAIL);
                      }
                     else
                         SendAnswer(RECIVE_STATUS_FAIL);
                     CONFIG_FSM = CONFIG_RECIVE_START;
                     recive_index = 0;
                 }
                 break;
         }
     }
}
