/*
 * EEPROM_25C.c
 *
 *  Created on: Jul 12, 2024
 *      Author: i.dymov
 */


#include "EEPROM_25C.h"
#include "hal_spi.h"
static EEPROM_25C_t SPI_EEPROM;

void WriteCallBack()
{
    u8 data;
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
   switch (SPI_EEPROM.state)
   {
       case WRITE_ADDR_WRITE_STATE:
           SPI_EEPROM.state = WRITE_ADDR_WRITE_STATE1;
           SPI_I2S_SendData(SPI2, (u8)(SPI_EEPROM.data_addres>>8));
           break;
       case WRITE_ADDR_WRITE_STATE1:
           SPI_I2S_SendData(SPI2, (u8)(SPI_EEPROM.data_addres & 0xFF));
           SPI_EEPROM.state = WRITE_DATA_WRITE_STATE;
           break;
       case WRITE_DATA_WRITE_STATE:
           if (SPI_EEPROM.data_index < SPI_EEPROM.data_size)
           {
               SPI_I2S_SendData(SPI2, (u8)(SPI_EEPROM.pdata[SPI_EEPROM.data_index++]));
           }
           else
           {
               SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_TXE,DISABLE );
               SPI_EEPROM.state = EEPROM_IDLE_STATE;
               xTaskNotifyIndexedFromISR(SPI_EEPROM.NotifyTaskHeandle, SPI_EEPROM.ucTaskNatificationIndex, 0x01, eSetValueWithOverwrite, &xHigherPriorityTaskWoken  );
               portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
           }
           break;
       case READ_ADDR_WRITE_STATE:
                  SPI_EEPROM.state = WRITE_ADDR_WRITE_STATE1;
                  SPI_I2S_SendData(SPI2, (u8)(SPI_EEPROM.data_addres>>8));
                  break;
       case READ_ADDR_WRITE_STATE1:
                  SPI_I2S_SendData(SPI2, (u8)(SPI_EEPROM.data_addres & 0xFF));
                  SPI_EEPROM.state = READ_DATA_READ_STATE;
                  break;
       case READ_DATA_READ_STATE:
                 HAL_SPI_RXOveleyClear(HAL_SPI2);
                 SPI_I2S_SendData(SPI2, 0xFF);
                 break;
       case WRITE_COMMAND_STATE:
           SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_TXE,DISABLE );
           SPI_EEPROM.state = EEPROM_IDLE_STATE;
           xTaskNotifyIndexedFromISR(SPI_EEPROM.NotifyTaskHeandle, SPI_EEPROM.ucTaskNatificationIndex, 0x01, eSetValueWithOverwrite, &xHigherPriorityTaskWoken  );
           portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
           break;
       case WRITE_STATUS_STATE:
                   data =  SPI_I2S_ReceiveData(SPI2);
                   if (( data & 0X01 )) SPI_I2S_SendData(SPI2, 0x05 );
                   else
                   {
                       xTaskNotifyIndexedFromISR(SPI_EEPROM.NotifyTaskHeandle, SPI_EEPROM.ucTaskNatificationIndex, 0x01, eSetValueWithOverwrite, &xHigherPriorityTaskWoken  );
                       portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
                       SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
                   }
                   break;

               case READ_DATA_READ_STATE1:
                   SPI_EEPROM.pdata[SPI_EEPROM.data_index]=SPI_I2S_ReceiveData(SPI2);
                   if (++SPI_EEPROM.data_index < SPI_EEPROM.data_size)
                   {
                       SPI_I2S_SendData(SPI2, 0xFF);
                   }
                   else
                   {
                       xTaskNotifyIndexedFromISR(SPI_EEPROM.NotifyTaskHeandle, SPI_EEPROM.ucTaskNatificationIndex, 0x01, eSetValueWithOverwrite, &xHigherPriorityTaskWoken  );
                       portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
                       SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
                   }
                   break;



   }
}
void ReadCallBack()
{
    u8 data;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    switch (SPI_EEPROM.state)
    {

    }
}
void InitEEPROM( HAL_SPI_t spi)
{
    HAL_SPI_InitTypeDef Init;
    Init.SPI_Direction = HAL_SPI_FULLDuplex;
    Init.SPI_DataSize = SPI_8bit;
    Init.SPI_CPOL = SPI_CPOL_Low;              //§±§à§Ý§ñ§â§à§ß§ã§ä§î §Ü§Ý§à§Ü§Ñ
    Init.SPI_CPHA = SPI_CPHA_1Edge;               //§¶§â§à§ß§ä §Ü§Ý§à§Ü§Ñ
    Init.SPI_NSS= SPI_SOFT_NSS;
    Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128; //§¥§Ö§Ý§Ú§ä§Ö§Ý§î §ã§Ü§à§â§à§ã§ä§Ú
    Init.SPI_FirstBit = SPI_FirstBit_MSB;
    Init.SPI_CRCPolynomial = 0;
    HAL_SPI_MsterBaseInit(spi,&Init);
    HAL_SPI_ConfgiIT(spi, &ReadCallBack,&WriteCallBack,1,3);
}

EERPOM_ERROR_CODE_t SetEEPROMUnprotect(HAL_SPI_t spi, u32 timeout,u8 TASK_NOTIFICATION_INDEX)
{
    SPI_EEPROM.state =  WRITE_COMMAND_STATE;
    uint32_t exit_code;
    SPI_EEPROM.ucTaskNatificationIndex = TASK_NOTIFICATION_INDEX;
    SPI_EEPROM.NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
    SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_TXE,ENABLE );
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
    vTaskDelay(1);
    SPI_I2S_SendData(SPI2, 0x06 );
    exit_code = ulTaskNotifyTakeIndexed( SPI_EEPROM.ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
    HAL_SPI_RXOveleyClear(spi);
    return ( (exit_code == 0x01  )? (EEPROM_OK) : (EEPROM_WRITE_ERROR)) ;
}



EERPOM_ERROR_CODE_t ReadEEPROMData(HAL_SPI_t spi,  u8 addres, u16 data_adress, u8 * data, u16 data_size, u32 timeout, u8 TASK_NOTIFICATION_INDEX)
{
       SPI_EEPROM.state =  WRITE_STATUS_STATE;
       SPI_EEPROM.data_addres =  data_adress;
       SPI_EEPROM.pdata = data;
       SPI_EEPROM.data_size = data_size;
       SPI_EEPROM.ucTaskNatificationIndex = TASK_NOTIFICATION_INDEX;
       SPI_EEPROM.NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
       uint32_t exit_code;

       SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE );
       GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
       vTaskDelay(1);
       SPI_I2S_SendData(SPI2, 0x05 );
       exit_code = ulTaskNotifyTakeIndexed( SPI_EEPROM.ucTaskNatificationIndex, 0xFFFFFFFF, timeout );

       vTaskDelay(1);
       GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
       vTaskDelay(1);
       GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
       SPI_EEPROM.state =  READ_ADDR_WRITE_STATE;
       SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE );
       SPI_I2S_SendData(SPI2, 0x03);
       exit_code = ulTaskNotifyTakeIndexed( SPI_EEPROM.ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
       SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_RXNE,DISABLE );
       vTaskDelay(1);
       GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
       return ( (exit_code == 0x01  )? (EEPROM_OK) : (EEPROM_WRITE_ERROR)) ;
}



EERPOM_ERROR_CODE_t WriteEEPROMData( HAL_SPI_t spi, u8 addres, u16 data_adress, u8 * data, u16 data_size, u32 timeout, u8 TASK_NOTIFICATION_INDEX)
{
    SPI_EEPROM.state =  WRITE_STATUS_STATE;
    SPI_EEPROM.data_addres =  data_adress;
    SPI_EEPROM.pdata = data;
    SPI_EEPROM.data_size = data_size;
    SPI_EEPROM.ucTaskNatificationIndex = TASK_NOTIFICATION_INDEX;
    SPI_EEPROM.NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
    uint32_t exit_code;
    HAL_SPI_RXOveleyClear(spi);

    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
    vTaskDelay(1);
    SPI_I2S_SendData(SPI2, 0x05 );
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE );
    exit_code = ulTaskNotifyTakeIndexed( SPI_EEPROM.ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
    vTaskDelay(1);
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
    vTaskDelay(1);
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
    SPI_EEPROM.state =  WRITE_ADDR_WRITE_STATE;
    SPI_I2S_SendData(SPI2, 0x02);
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE );
    exit_code = ulTaskNotifyTakeIndexed( SPI_EEPROM.ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
    vTaskDelay(1);
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
    HAL_SPI_RXOveleyClear(spi);
    return ( (exit_code == 0x01  )? (EEPROM_OK) : (EEPROM_WRITE_ERROR)) ;
}

EERPOM_ERROR_CODE_t WriteEEPROMStatus( u8 status,  u32 timeout, u8 TASK_NOTIFICATION_INDEX)
{
    SPI_EEPROM.state =  WRITE_STATUS_STATE;
    SPI_EEPROM.ucTaskNatificationIndex = TASK_NOTIFICATION_INDEX;
    SPI_EEPROM.NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
    SPI_EEPROM.dma_command[0]= WRSR_COMMNAD;
    SPI_EEPROM.dma_command[1] =(u8)(status);
    uint32_t exit_code;
    while (HAL_SPI_GetBusy(SPI_EEPROM.spi) == HAL_SET);
    HAL_DMA_Start(SPI_EEPROM.dma_stream_write, 2 , (uint32_t)SPI_EEPROM.dma_command );
    exit_code = ulTaskNotifyTakeIndexed( SPI_EEPROM.ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
    return ( (exit_code == 0x01  )? (EEPROM_OK) : (EEPROM_WRITE_ERROR)) ;
}
EERPOM_ERROR_CODE_t ReadEEPROMStatus( u8 * status,  u32 timeout, u8 TASK_NOTIFICATION_INDEX)
{
    SPI_EEPROM.state =  READ_STATUS_STATE;
    SPI_EEPROM.ucTaskNatificationIndex = TASK_NOTIFICATION_INDEX;
    SPI_EEPROM.NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
    SPI_EEPROM.dma_command[0]= RDSR_COMMAND ;
    SPI_EEPROM.pdata = status;
    uint32_t exit_code;
    while (HAL_SPI_GetBusy(SPI_EEPROM.spi) == HAL_SET);
    HAL_DMA_Start(SPI_EEPROM.dma_stream_write, 1 , (uint32_t)SPI_EEPROM.dma_command );
    exit_code = ulTaskNotifyTakeIndexed( SPI_EEPROM.ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
    return ( (exit_code == 0x01  )? (EEPROM_OK) : (EEPROM_WRITE_ERROR)) ;
}

EERPOM_ERROR_CODE_t WriteEEPROMProtect( u8 command,  u32 timeout, u8 TASK_NOTIFICATION_INDEX)
{
    SPI_EEPROM.state =  WRITE_STATUS_STATE;
    SPI_EEPROM.ucTaskNatificationIndex = TASK_NOTIFICATION_INDEX;
    SPI_EEPROM.NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
    SPI_EEPROM.dma_command[0]= command;
    uint32_t exit_code;
    while (HAL_SPI_GetBusy(SPI_EEPROM.spi) == HAL_SET);
    HAL_DMA_Start(SPI_EEPROM.dma_stream_write, 1 , (uint32_t)SPI_EEPROM.dma_command );
    exit_code = ulTaskNotifyTakeIndexed( SPI_EEPROM.ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
    return ( (exit_code == 0x01  )? (EEPROM_OK) : (EEPROM_WRITE_ERROR)) ;
}

