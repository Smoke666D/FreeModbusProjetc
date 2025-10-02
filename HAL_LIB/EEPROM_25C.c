/*
 * EEPROM_25C.c
 *
 *  Created on: Jul 12, 2024
 *      Author: i.dymov
 */


#include "EEPROM_25C.h"
#include "hal_spi.h"
#include "init.h"
#include "hal_gpio.h"

static EEPROM_25C_t SPI_EEPROM;


static u8 sector_buffer[SECTOR_SIZE];


void WriteCallBack()
{

}
void ReadCallBack()
{
    u8 return_flag = 0;

    u8 read_data =  HAL_SPI_GetData(SPI_EEPROM.spi);
    switch (SPI_EEPROM.state)
    {
        case READ_ADDR_WRITE_STATE:
            SPI_EEPROM.state = READ_ADDR_WRITE_STATE1;
            HAL_SPI_Send(SPI_EEPROM.spi, (u8)(SPI_EEPROM.data_addres>>8));
             break;
        case READ_ADDR_WRITE_STATE1:
            HAL_SPI_Send(SPI_EEPROM.spi, (u8)(SPI_EEPROM.data_addres & 0xFF));
            SPI_EEPROM.state = READ_DATA_READ_STATE1;
            break;
        case READ_DATA_READ_STATE1:
            HAL_SPI_Send(SPI_EEPROM.spi, 0);
            SPI_EEPROM.state = READ_DATA_READ_STATE;
            break;
        case WRITE_COMMAND_STATE:
            return_flag = 1;
            break;
        case  READ_STATUS_STATE:
            if ( read_data & 0x01 )
                HAL_SPI_Send(SPI_EEPROM.spi, RDSR_COMMAND );
            else
            {
                return_flag = 1;
            }
            break;
        case READ_DATA_READ_STATE:
               SPI_EEPROM.pdata[SPI_EEPROM.data_index]=read_data;
               if (++SPI_EEPROM.data_index < SPI_EEPROM.data_size)
               {
                   HAL_SPI_Send(SPI_EEPROM.spi, 0x00);
               }
               else
               {
                   return_flag = 1;
               }
               break;
        case WRITE_ADDR_WRITE_STATE:
               SPI_EEPROM.state = WRITE_ADDR_WRITE_STATE1;
               HAL_SPI_Send(SPI_EEPROM.spi, (u8)(SPI_EEPROM.data_addres>>8));
               break;
        case WRITE_ADDR_WRITE_STATE1:
               HAL_SPI_Send(SPI_EEPROM.spi, (u8)(SPI_EEPROM.data_addres & 0xFF));
               SPI_EEPROM.state = WRITE_DATA_WRITE_STATE;
               break;
       case WRITE_DATA_WRITE_STATE:
               if (SPI_EEPROM.data_index < SPI_EEPROM.data_size)
               {
                  HAL_SPI_Send(SPI_EEPROM.spi, (u8)SPI_EEPROM.pdata[SPI_EEPROM.data_index++]);
               }
               else
               {
                   return_flag = 1;
               }
               break;
        default:
            break;
    }
    if (return_flag)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyIndexedFromISR(SPI_EEPROM.NotifyTaskHeandle, SPI_EEPROM.ucTaskNatificationIndex, 0x01, eSetValueWithOverwrite, &xHigherPriorityTaskWoken  );
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
}
INIT_FUNC_LOC void InitEEPROM( HAL_SPI_t spi, TimerName_t TimerName)
{
    HAL_SPI_InitTypeDef Init;
    Init.SPI_Direction = HAL_SPI_FULLDuplex;
    Init.SPI_DataSize = SPI_8bit;
    Init.SPI_CPOL = SPI_CPOL_High;              //§±§à§Ý§ñ§â§à§ß§ã§ä§î §Ü§Ý§à§Ü§Ñ
    Init.SPI_CPHA = SPI_CPHA_2Edge;               //§¶§â§à§ß§ä §Ü§Ý§à§Ü§Ñ
    Init.SPI_NSS= SPI_SOFT_NSS;
    Init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; //§¥§Ö§Ý§Ú§ä§Ö§Ý§î §ã§Ü§à§â§à§ã§ä§Ú
    Init.SPI_FirstBit = SPI_FirstBit_MSB;
    Init.SPI_CRCPolynomial = 0;
    SPI_EEPROM.spi = spi;
    HAL_SPI_MsterBaseInit(spi,&Init);
    HAL_SPI_ConfgiIT(spi, &ReadCallBack,&WriteCallBack,SPI2_PRIOR,SPI2_SUBPRIOR);
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE,  ENABLE );
    HAL_SetBit(SPI2_Port,SPI2_NSS_Pin);
    HW_TIMER_TimerInit( TimerName, 10000,10);
    SPI_EEPROM.TimerName  = TimerName;
}

EERPOM_ERROR_CODE_t SetEEPROMUnprotect( u32 timeout,u8 TASK_NOTIFICATION_INDEX)
{
    SPI_EEPROM.state =  WRITE_COMMAND_STATE;
    uint32_t exit_code;
    SPI_EEPROM.ucTaskNatificationIndex = TASK_NOTIFICATION_INDEX;
    SPI_EEPROM.NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
    HAL_ResetBit(SPI2_Port,SPI2_NSS_Pin);
    HAL_SPI_Send(SPI_EEPROM.spi, WREN_COMMAND );
    exit_code = ulTaskNotifyTakeIndexed( SPI_EEPROM.ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
    vTaskDelay(1);
    HAL_SetBit(SPI2_Port,SPI2_NSS_Pin);
    return ( (exit_code == 0x01  )? (EEPROM_OK) : (EEPROM_WRITE_ERROR)) ;
}

EERPOM_ERROR_CODE_t GetEEPROMStatus( u32 timeout,u8 TASK_NOTIFICATION_INDEX)
{
    SPI_EEPROM.state =  READ_STATUS_STATE;
    uint32_t exit_code;
    SPI_EEPROM.ucTaskNatificationIndex = TASK_NOTIFICATION_INDEX;
    SPI_EEPROM.NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
    HAL_ResetBit(SPI2_Port,SPI2_NSS_Pin);
    HAL_SPI_Send(SPI_EEPROM.spi,0x05);
    exit_code = ulTaskNotifyTakeIndexed( SPI_EEPROM.ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
    HAL_SetBit(SPI2_Port,SPI2_NSS_Pin);
    return ( (exit_code == 0x01  )? (EEPROM_OK) : (EEPROM_WRITE_ERROR)) ;
}


EERPOM_ERROR_CODE_t ReadEEPROMData( u16 data_adress, u8 * data, u16 data_size, u32 timeout, u8 TASK_NOTIFICATION_INDEX)
{
       SPI_EEPROM.data_addres =  data_adress;
       SPI_EEPROM.pdata = data;
       SPI_EEPROM.data_size = data_size;
       SPI_EEPROM.data_index = 0;
       SPI_EEPROM.ucTaskNatificationIndex = TASK_NOTIFICATION_INDEX;
       SPI_EEPROM.NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
       uint32_t exit_code = GetEEPROMStatus( timeout,TASK_NOTIFICATION_INDEX );
       if (exit_code == EEPROM_OK)
       {
           HAL_ResetBit(SPI2_Port,SPI2_NSS_Pin);
           SPI_EEPROM.state =  READ_ADDR_WRITE_STATE;
           HAL_SPI_Send(SPI_EEPROM.spi,READ_COMMAND);
           exit_code = ulTaskNotifyTakeIndexed( SPI_EEPROM.ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
           HAL_SetBit(SPI2_Port,SPI2_NSS_Pin);
       }
       return ( (exit_code == 0x01  )? (EEPROM_OK) : (EEPROM_WRITE_ERROR)) ;
}


u8 SPI1_ReadWriteByte(u8 TxData, u8 * RxData, uint32_t timeout)
{
    HAL_TimerReset( SPI_EEPROM.TimerName);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
    {
        if  (HAL_GetTimerCnt(SPI_EEPROM.TimerName)> timeout )
           return (EEPROM_BUSY);
    }
    SPI_I2S_SendData(SPI2, TxData);
    HAL_TimerReset( SPI_EEPROM.TimerName);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
    {
        if  (HAL_GetTimerCnt(SPI_EEPROM.TimerName)> timeout )
                   return (EEPROM_BUSY);
    }

    * RxData = SPI_I2S_ReceiveData(SPI2);
    return EEPROM_OK;
}

EERPOM_ERROR_CODE_t GetEEPROMStatusPolling( u32 timeout)
{
    EERPOM_ERROR_CODE_t res = EEPROM_OK;
    uint8_t rx_data;
    HAL_ResetBit(SPI2_Port,SPI2_NSS_Pin);
    while(1)
    {
        res = SPI1_ReadWriteByte(0x05,&rx_data, timeout);
        if (((rx_data & 0x01) == 0 ) || (res!= EEPROM_OK )) break;
    }
    HAL_SetBit(SPI2_Port,SPI2_NSS_Pin);
    return ( res) ;
}

EERPOM_ERROR_CODE_t ReadEEPROMDataPolling( u16 data_adress, u8 * data, u16 data_size, u32 timeout)
{
    EERPOM_ERROR_CODE_t res = EEPROM_OK;
    uint8_t rx_data;
    HAL_TiemrEneblae(SPI_EEPROM.TimerName);
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE,  DISABLE );
    res =  GetEEPROMStatusPolling(timeout);
    HAL_ResetBit(SPI2_Port,SPI2_NSS_Pin);
    res = SPI1_ReadWriteByte(READ_COMMAND,&rx_data, timeout);
    res = SPI1_ReadWriteByte((u8)(data_adress>>8),&rx_data, timeout);
    res = SPI1_ReadWriteByte( (u8)(data_adress & 0xFF),&rx_data, timeout);
    for (u16 i = 0; i < data_size;i++)
    {
        res = SPI1_ReadWriteByte(0,&data[i],timeout );
        if (res !=EEPROM_OK) break;
    }
    HAL_SetBit(SPI2_Port,SPI2_NSS_Pin);
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE,  ENABLE );
    HAL_TiemrDisable(SPI_EEPROM.TimerName);
    return  (res);
}

EERPOM_ERROR_CODE_t SetEEPROMUnprotectPolling( u32 timeout)
{
    uint8_t rx_data;
    EERPOM_ERROR_CODE_t res = EEPROM_OK;
    HAL_ResetBit(SPI2_Port,SPI2_NSS_Pin);
    res = SPI1_ReadWriteByte( WREN_COMMAND, &rx_data, 1);
    Delay_Ms(1);
    HAL_SetBit(SPI2_Port,SPI2_NSS_Pin);
    return (res);
}

EERPOM_ERROR_CODE_t WriteEEPROMDataPolling( u16 data_adress, u8 * data, u16 data_size, u32 timeout)
{
    EERPOM_ERROR_CODE_t res = EEPROM_OK;
        uint8_t rx_data;
        HAL_ResetBit(SPI2_Port,SPI2_NSS_Pin);
        res = SPI1_ReadWriteByte(WRITE_COMMAND,&rx_data, timeout );
        res = SPI1_ReadWriteByte((u8)(data_adress>>8),&rx_data, timeout);
        res = SPI1_ReadWriteByte( (u8)(data_adress & 0xFF),&rx_data, timeout);
        for (u16 i = 0; i < data_size;i++)
        {
              res = SPI1_ReadWriteByte( data[i],&rx_data, timeout );
              if (res!=EEPROM_OK) break;
       }
        HAL_SetBit(SPI2_Port,SPI2_NSS_Pin);


    return (res)  ;
}


EERPOM_ERROR_CODE_t WriteEEPROMPolling(  u16 data_adress, u8 * data, u16 data_size, u32 timeout)
{
         SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE,  DISABLE );
         EERPOM_ERROR_CODE_t res = EEPROM_WRITE_ERROR;
        if ((  data_adress+ data_size  <= EEPROM_SIZE ) && (data_size!=0))
        {
          u16 cur_len;
          u16 byte_to_send = data_size;
          u16 offset = 0;
          u16 cur_addr = data_adress;
          while  (byte_to_send > 0)
          {
              GetEEPROMStatusPolling(timeout);
              cur_len = SECTOR_SIZE - ( cur_addr % SECTOR_SIZE );
              if ( cur_len > byte_to_send )  cur_len = byte_to_send;
              ( void )memcpy( &sector_buffer, &data[offset], cur_len );
              res = SetEEPROMUnprotectPolling(timeout);
              if (res!=EEPROM_OK) break;
              res = WriteEEPROMDataPolling( cur_addr, sector_buffer ,cur_len, timeout);
              if (res!=EEPROM_OK) break;
              offset = offset  + cur_len;
              byte_to_send = byte_to_send - cur_len;
              cur_addr = cur_addr  + cur_len;

            }
        }
        SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE,  ENABLE );
        return (res);


}




EERPOM_ERROR_CODE_t WriteEEPROMData( u16 data_adress, u8 * data, u16 data_size, u32 timeout, u8 TASK_NOTIFICATION_INDEX)
{

    SPI_EEPROM.data_addres =  data_adress;
    SPI_EEPROM.pdata = data;
    SPI_EEPROM.data_size = data_size;
    SPI_EEPROM.data_index = 0;
    SPI_EEPROM.ucTaskNatificationIndex = TASK_NOTIFICATION_INDEX;
    SPI_EEPROM.NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
    uint32_t exit_code =  GetEEPROMStatus( timeout,TASK_NOTIFICATION_INDEX );
    if (exit_code == EEPROM_OK )
    {
        HAL_ResetBit(SPI2_Port,SPI2_NSS_Pin);
        SPI_EEPROM.state =  WRITE_ADDR_WRITE_STATE;
        HAL_SPI_Send(SPI_EEPROM.spi,WRITE_COMMAND);
        exit_code = ulTaskNotifyTakeIndexed( SPI_EEPROM.ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
        HAL_SetBit(SPI2_Port,SPI2_NSS_Pin);
    }

    return ( (exit_code == 0x01  )? (EEPROM_OK) : (EEPROM_WRITE_ERROR)) ;
}



EERPOM_ERROR_CODE_t WriteEEPROM(  u16 data_adress, u8 * data, u16 data_size, u32 timeout, u8 TASK_NOTIFICATION_INDEX)
{
    EERPOM_ERROR_CODE_t res = EEPROM_WRITE_ERROR;
    if ((  data_adress+ data_size  <= EEPROM_SIZE ) && (data_size!=0))
    {
      u16 cur_len;
      u16 byte_to_send = data_size;
      u16 offset = 0;
      u16 cur_addr = data_adress;
      while  (byte_to_send > 0)
      {
          cur_len = SECTOR_SIZE - ( cur_addr % SECTOR_SIZE );
          if ( cur_len > byte_to_send )  cur_len = byte_to_send;
          ( void )memcpy( &sector_buffer, &data[offset], cur_len );
          res = GetEEPROMStatus( timeout,TASK_NOTIFICATION_INDEX);
          if (res!=EEPROM_OK) break;
          res = SetEEPROMUnprotect(timeout,2);
          if (res!=EEPROM_OK) break;
          res = WriteEEPROMData( cur_addr, sector_buffer ,cur_len, timeout, 2);
          if (res!=EEPROM_OK) break;
          offset = offset  + cur_len;
          byte_to_send = byte_to_send - cur_len;
          cur_addr = cur_addr  + cur_len;
        }
    }
    return (res);
}


