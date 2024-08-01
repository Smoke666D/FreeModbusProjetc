/*
 * EEPROM_25C.c
 *
 *  Created on: Jul 12, 2024
 *      Author: i.dymov
 */


#include "EEPROM_25C.h"

static EEPROM_25C_t SPI_EEPROM;

void WriteCallBack()
{
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
   switch (SPI_EEPROM.state)
   {
       case READ_STATUS_STATE:
           HAL_DMA_Disable(SPI_EEPROM.dma_stream_write);
           HAL_DMA_Start(SPI_EEPROM.dma_stream_read, 1 ,(uint32_t) &SPI_EEPROM.pdata );
           break;
       case READ_ADDR_WRITE_STATE:
           SPI_EEPROM.state = READ_DATA_READ_STATE;
           HAL_DMA_Disable(SPI_EEPROM.dma_stream_write);
           HAL_DMA_Start(SPI_EEPROM.dma_stream_read, SPI_EEPROM.data_size ,(uint32_t) &SPI_EEPROM.pdata );
           break;
       case WRITE_ADDR_WRITE_STATE:
           SPI_EEPROM.state = WRITE_DATA_WRITE_STATE;
           HAL_DMA_Disable(SPI_EEPROM.dma_stream_write);
           HAL_DMA_Start(SPI_EEPROM.dma_stream_write, SPI_EEPROM.data_size ,(uint32_t) &SPI_EEPROM.pdata );
           break;
       case WRITE_DATA_WRITE_STATE:
       case WRITE_STATUS_STATE:
           SPI_EEPROM.state = EEPROM_IDLE_STATE;
           HAL_DMA_Disable(SPI_EEPROM.dma_stream_write);
           xTaskNotifyIndexedFromISR(SPI_EEPROM.NotifyTaskHeandle, SPI_EEPROM.ucTaskNatificationIndex, 0x01, eSetValueWithOverwrite, &xHigherPriorityTaskWoken  );
           portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
           break;


   }
}
void ReadCallBack()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    switch (SPI_EEPROM.state)
    {
        case READ_DATA_READ_STATE:
            SPI_EEPROM.state = EEPROM_IDLE_STATE;
            HAL_DMA_Disable(SPI_EEPROM.dma_stream_read);
            xTaskNotifyIndexedFromISR(SPI_EEPROM.NotifyTaskHeandle, SPI_EEPROM.ucTaskNatificationIndex, 0x01, eSetValueWithOverwrite, &xHigherPriorityTaskWoken  );
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
            break;
    }
}
void InitEEPROM( HAL_SPI_t spi, DMA_Stram_t stream_read, DMA_Stram_t stream_write)
{
    HAL_SPI_InitTypeDef Init;
    Init.SPI_Direction = HAL_SPI_FULLDuplex;
    Init.SPI_DataSize = SPI_8bit;
    Init.SPI_CPOL = SPI_CPOL_High;              //§±§à§Ý§ñ§â§à§ß§ã§ä§î §Ü§Ý§à§Ü§Ñ
    Init.SPI_CPHA = SPI_CPHA_2Edge;               //§¶§â§à§ß§ä §Ü§Ý§à§Ü§Ñ
    Init.SPI_NSS= SPI_HARD_NSS;
    Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; //§¥§Ö§Ý§Ú§ä§Ö§Ý§î §ã§Ü§à§â§à§ã§ä§Ú
    Init.SPI_FirstBit = SPI_FirstBit_MSB;
    Init.SPI_CRCPolynomial = 0;
    HAL_SPI_MsterBaseInit(spi,&Init);
    DMA_INIT_t init;
    init.stream = stream_write;
    init.direction = MTOP;
    init.mode  = DMA_Normal;
    init.paddr = (u32)&(SPI2->DATAR);
    init.memadr = 0;
    init.dma_size = DMA_BYTE;
    init.bufsize = 0;
    init.prioroty = dma_Medium;
    HAL_DMAInitIT(init,5,0,&WriteCallBack);
    init.stream = stream_read;
    init.direction = PTOM;
    HAL_DMAInitIT(init,5,0,&ReadCallBack);
    SPI_EEPROM.spi =  spi;
    SPI_EEPROM.dma_stream_read =  stream_read;
    SPI_EEPROM.dma_stream_write =  stream_write;
}


EERPOM_ERROR_CODE_t ReadEEPROMData( u8 addres, u16 data_adress, u8 * data, u16 data_size, u32 timeout, u8 TASK_NOTIFICATION_INDEX)
{
    SPI_EEPROM.state =  READ_ADDR_WRITE_STATE;
    SPI_EEPROM.data_addres =  data_adress;
    SPI_EEPROM.pdata = data;
    SPI_EEPROM.data_size = data_size;
    SPI_EEPROM.ucTaskNatificationIndex = TASK_NOTIFICATION_INDEX;
    SPI_EEPROM.NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
    SPI_EEPROM.dma_command[0]= READ_COMMAND;
    SPI_EEPROM.dma_command[1] =(u8)(data_adress>8);
    SPI_EEPROM.dma_command[2] =(u8)(data_adress & 0xFF);
    uint32_t exit_code;
   // HAL_SPI_GetBusy(HAL_SPI2)
    while (HAL_SPI_GetBusy(SPI_EEPROM.spi) == HAL_SET);
    HAL_DMA_Start(SPI_EEPROM.dma_stream_write, 3 , (uint32_t)SPI_EEPROM.dma_command );
    exit_code = ulTaskNotifyTakeIndexed( SPI_EEPROM.ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
    return ( (exit_code == 0x01  )? (EEPROM_OK) : (EEPROM_WRITE_ERROR)) ;
}



EERPOM_ERROR_CODE_t WriteEEPROMData( u8 addres, u16 data_adress, u8 * data, u16 data_size, u32 timeout, u8 TASK_NOTIFICATION_INDEX)
{
    SPI_EEPROM.state =  WRITE_ADDR_WRITE_STATE;
    SPI_EEPROM.data_addres =  data_adress;
    SPI_EEPROM.pdata = data;
    SPI_EEPROM.data_size = data_size;
    SPI_EEPROM.ucTaskNatificationIndex = TASK_NOTIFICATION_INDEX;
    SPI_EEPROM.NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
    SPI_EEPROM.dma_command[0]= WRITE_COMMAND;
    SPI_EEPROM.dma_command[1] =(u8)(data_adress>8);
    SPI_EEPROM.dma_command[2] =(u8)(data_adress & 0xFF);
    uint32_t exit_code;
    while (HAL_SPI_GetBusy(SPI_EEPROM.spi) == HAL_SET);
    HAL_DMA_Start(SPI_EEPROM.dma_stream_write, 3 , (uint32_t)SPI_EEPROM.dma_command );
    exit_code = ulTaskNotifyTakeIndexed( SPI_EEPROM.ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
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

