#include "hw_lib_eeprom_i2c.h"
#include "hal_wdt.h"


static EEPOROM Dev                                          __SECTION(RAM_SECTION_RAM);



void vInitEEPROM_I2C(I2C_NAME_t i2c, uint8_t prior, uint8_t subprior)
{

    Dev.dev = i2c;
    Dev.DevAdrres = Device_ADD;
    HAL_I2C_InitTypeDef  I2C_InitTSturcture = {0};

            I2C_InitTSturcture.I2C_ClockSpeed = 300000;
            I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
            I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_16_9;
            I2C_InitTSturcture.I2C_OwnAddress1 = 0;
            I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
            I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    InitI2CIT( i2c, &I2C_InitTSturcture, prior,  subprior,&I2C_FSM,I2C_FSM);

}


static EEPOROM Dev                                          __SECTION(RAM_SECTION_RAM);


 void I2C_FSM()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;


#if MCU == CH32V2
        uint16_t int_flags =    HAL_I2C_GET_STAR1(Dev.dev);
        switch(Dev.I2C_State)
        {
             case  I2C_MASTER_RECIVE_START:
                 if (int_flags &  STAR1_SB_FLAG)
                 {
                     HAL_I2C_SEND_ADDR_TRANS( Dev.dev,Dev.DevAdrres );
                     Dev.I2C_State = I2C_MASTER_RECIVE_WRITE_ADDR;
                 }
                 break;
              case I2C_MASTER_RECIVE_WRITE_ADDR:
                 if (int_flags & STAR1_ADDR_FLAG)
                 {
                       int_flags = HAL_I2C_GET_STAR2(Dev.dev);
                       HAL_I2C_SEND( Dev.dev,(Dev.data_address >>  8) & 0x1F );
                       Dev.I2C_State = I2C_MASTER_RECIVE_WRITE_ADDR2;
                 }
                 break;
              case  I2C_MASTER_RECIVE_WRITE_ADDR2:
                  if (int_flags & STAR1_TXE_FLAG)
                  {
                      HAL_I2C_SEND(Dev.dev, Dev.data_address & 0xFF );
                      Dev.I2C_State = I2C_MASTER_RECIVE_DA_WRITE;
                  }
                  break;
              case I2C_MASTER_RECIVE_DA_WRITE:
                  if (int_flags & STAR1_TXE_FLAG)
                  {
                      HAL_I2C_START(Dev.dev);
                      Dev.I2C_State = I2C_MASTER_RECIVE_ADDR;
                   }
                   break;
              case I2C_MASTER_RECIVE_ADDR:
                  if (int_flags & STAR1_SB_FLAG)
                  {
                      HAL_I2C_SEND_ADDR_RECIEVE(  Dev.dev,Dev.DevAdrres);
                      Dev.I2C_State = I2C_MASTER_RECIVE_MODE;
                  }
                  break;
             case I2C_MASTER_RECIVE_MODE:
                  if (int_flags & STAR1_ADDR_FLAG)
                  {
                      int_flags = HAL_I2C_GET_STAR2(Dev.dev);
                      Dev.I2C_State = I2C_MASTER_RECIEVE;
                   }
                   break;
            case I2C_MASTER_RECIEVE:
                   if (int_flags & 0x0040)
                   {
                       if (Dev.Index < (Dev.DataLength-1))
                       {
                            Dev.ReciveBuffer[Dev.Index++] = HAL_I2C_READ_DATA(Dev.dev);
                       }
                       else
                       {
                            HAL_I2C_ACK_DISABLE(Dev.dev);
                            Dev.ReciveBuffer[Dev.Index++] = HAL_I2C_READ_DATA(Dev.dev);
                            HAL_I2C_STOP(Dev.dev);
                            xTaskNotifyIndexedFromISR(Dev.NotifyTaskHeandle, Dev.ucTaskNatificationIndex, 0x01, eSetValueWithOverwrite, &xHigherPriorityTaskWoken  );
                            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
                            Dev.I2C_State = I2C_IDLE;
                        }
                     }
                     break;
                 case  I2C_MASTER_TRANSMIT_START:

                    if (int_flags & STAR1_SB_FLAG)
                    {
                        HAL_I2C_SEND_ADDR_TRANS( Dev.dev, Dev.DevAdrres );
                        Dev.I2C_State = I2C_MASTER_TRANSMIT_ADDR;
                    }
                     break;
                 case I2C_MASTER_TRANSMIT_ADDR:
                    if  ( int_flags & 0x0400)
                     {
                        HAL_I2C_ClearFlag( Dev.dev,I2C_FLAG_AF);
                        HAL_I2C_START(Dev.dev);
                         Dev.I2C_State = I2C_MASTER_TRANSMIT_START;
                      }
                     else

                    if (int_flags & STAR1_ADDR_FLAG)
                    {
                        int_flags = HAL_I2C_GET_STAR2(Dev.dev);
                        HAL_I2C_SEND( Dev.dev,(Dev.data_address >>  8) & 0x1F );
                        Dev.I2C_State = I2C_MASTER_TRANSMIT_ADDR2;
                    }
                     break;

                 case I2C_MASTER_TRANSMIT_ADDR2:
                     if (int_flags & STAR1_TXE_FLAG)
                     {
                         HAL_I2C_SEND(Dev.dev, Dev.data_address & 0xFF );
                         Dev.I2C_State = I2C_MASTER_TRANSMIT_NEXT;
                     }

                     break;
                 case I2C_MASTER_TRANSMIT_NEXT:
                     if (int_flags & STAR1_TXE_FLAG)
                     {
                        if (Dev.Index < Dev.DataLength )
                        {
                           HAL_I2C_SEND(Dev.dev, Dev.ReciveBuffer[Dev.Index] );
                           Dev.Index++;
                        }
                        else
                            Dev.I2C_State = I2C_MASTER_TRANSMIT_LAST;
                     }
                     break;
                 case  I2C_MASTER_TRANSMIT_LAST:
                     if (int_flags & STAR1_BTF_FLAG)
                     {
                         HAL_I2C_STOP(Dev.dev);
                         xTaskNotifyIndexedFromISR(Dev.NotifyTaskHeandle, Dev.ucTaskNatificationIndex, 0x01, eSetValueWithOverwrite, &xHigherPriorityTaskWoken  );
                         portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
                         Dev.I2C_State = I2C_IDLE;
                     }
                     break;
                 default:
                     break;
             }


#endif
}


static EERPOM_ERROR_CODE_t I2C_Master_TransmitFast(  u16 data_addres,  u8 * data, u16 data_size  )
{
   HAL_I2C_ENABLE(Dev.dev);
   while(  HAL_I2C_GET_STAR2(Dev.dev) & STAR2_BUSY_FLAG  );
   HAL_I2C_START(Dev.dev);
   while( !( HAL_I2C_GET_STAR1(Dev.dev) & STAR1_SB_FLAG )) ;

   HAL_I2C_SEND_ADDR_TRANS( Dev.dev, Dev.DevAdrres );
   while( !( HAL_I2C_GET_STAR1(Dev.dev) & STAR1_ADDR_FLAG ) );
   u16 int_flags = HAL_I2C_GET_STAR2(Dev.dev);
   HAL_I2C_SEND( Dev.dev, (u8)((data_addres >>  8) & 0x1F ) );
   while( !( HAL_I2C_GET_STAR1(Dev.dev) & STAR1_TXE_FLAG ) );
   HAL_I2C_SEND(Dev.dev,  data_addres & 0xFF );
   while( !( HAL_I2C_GET_STAR1(Dev.dev) & STAR1_TXE_FLAG ) );
   for (u8 i = 0;i<data_size;i++)
   {
       HAL_I2C_SEND(Dev.dev, data[i]);
       while( !( HAL_I2C_GET_STAR1(Dev.dev) & STAR1_TXE_FLAG ) );
   }
   HAL_I2C_STOP(Dev.dev);
   return (EEPROM_OK);
}

EERPOM_ERROR_CODE_t eEEPROMWrFast(  EEPROM_ADRESS_TYPE addr, uint8_t * data, EEPROM_ADRESS_TYPE len )
{
      EEPROM_ADRESS_TYPE byte_to_send = len;
      EEPROM_ADRESS_TYPE offset = 0;
      EEPROM_ADRESS_TYPE cur_addr = addr;
      EEPROM_ADRESS_TYPE cur_len;
      if ((  addr+ len  <= EEPROM_SIZE ) && (len!=0))
      {
            do
            {
                cur_len = SECTOR_SIZE - ( cur_addr % SECTOR_SIZE );
                if ( cur_len > byte_to_send )  cur_len = byte_to_send;


                I2C_Master_TransmitFast( cur_addr  , &data[offset],  cur_len  );
                vTaskDelay(5);
                HAL_WDTReset();
                offset         = offset  + cur_len;
                byte_to_send   = byte_to_send - cur_len;
                cur_addr       = cur_addr  + cur_len;
            }
            while (byte_to_send > 0);

          return (EEPROM_OK);
      }

      return ( EEPROM_NOT_VALIDE_ADRESS);
}



static EERPOM_ERROR_CODE_t I2C_Master_TransmitIT(  u16 data_addres,  u8 * data, u16 data_size, u32 timeout,uint8_t TNI  )
{
    EERPOM_ERROR_CODE_t res = EEPROM_ACCESS_ERROR;
    uint32_t exit_code;
    Dev.ucTaskNatificationIndex = TNI;
    Dev.Index          = 0;
    Dev.DataLength     = data_size;
    Dev.ReciveBuffer   = data;
    Dev.data_address   = data_addres;
    Dev.NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
    Dev.I2C_State = I2C_MASTER_TRANSMIT_START;
    HAL_I2C_ENABLE(Dev.dev);
    while(  HAL_I2C_GET_STAR2(Dev.dev) & STAR2_BUSY_FLAG  );
    I2C_IT_ENABLE( Dev.dev,I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR );
    HAL_I2C_START(Dev.dev);
    exit_code = ulTaskNotifyTakeIndexed( Dev.ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
    I2C_IT_DISABLE( Dev.dev,I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR);
    HAL_I2C_DISABLE(Dev.dev);
    res = (exit_code > 0  )? (EEPROM_OK) : (EEPROM_READ_ERROR);
    return (res) ;
}




EERPOM_ERROR_CODE_t eEEPROMWr(  EEPROM_ADRESS_TYPE addr, uint8_t * data, EEPROM_ADRESS_TYPE len , uint8_t NotifyIndex )
{
   EERPOM_ERROR_CODE_t res = EEPROM_NOT_VALIDE_ADRESS;
   EEPROM_ADRESS_TYPE byte_to_send = len;
   EEPROM_ADRESS_TYPE offset = 0;
   EEPROM_ADRESS_TYPE cur_addr = addr;
   EEPROM_ADRESS_TYPE cur_len;
   if ((  addr+ len  <= EEPROM_SIZE ) && (len!=0))
   {
         do
         {
             cur_len = SECTOR_SIZE - ( cur_addr % SECTOR_SIZE );
             if ( cur_len > byte_to_send )  cur_len = byte_to_send;
             for (int i =0; i<6;i++)
             {
                res = I2C_Master_TransmitIT(  cur_addr  , &data[offset],  cur_len  , 10, NotifyIndex );
                if  (res == EEPROM_OK) break;
                HAL_WDTReset();
             }
             offset         = offset  + cur_len;
             byte_to_send   = byte_to_send - cur_len;
             cur_addr       = cur_addr  + cur_len;
         }
         while (byte_to_send > 0);
   }
   return ( res );
}




EERPOM_ERROR_CODE_t eEEPROMRd(  EEPROM_ADRESS_TYPE addr, uint8_t * data,   EEPROM_ADRESS_TYPE len , uint8_t NotifyIndex )
{
    if ( (addr +len)  <= EEPROM_SIZE)
    {

           EERPOM_ERROR_CODE_t res = EEPROM_ACCESS_ERROR;
           Dev.ucTaskNatificationIndex = NotifyIndex;
           Dev.Index          = 0;
           Dev.DataLength     = len ;
           Dev.ReciveBuffer   = data;
           Dev.data_address   = addr;
           Dev.NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
           Dev.I2C_State = I2C_MASTER_RECIVE_START;
           HAL_I2C_ENABLE( Dev.dev);
           while(  HAL_I2C_GET_STAR2(Dev.dev) & STAR2_BUSY_FLAG  );
           HAL_I2C_ACK_ENABLE(Dev.dev);
           HAL_I2C_START(Dev.dev);
           I2C_IT_ENABLE( Dev.dev, I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR );
           uint32_t exit_code = ulTaskNotifyTakeIndexed( Dev.ucTaskNatificationIndex, 0xFFFFFFFF, EEPROM_TIME_OUT );
           I2C_IT_DISABLE ( Dev.dev,I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR);
           HAL_I2C_DISABLE( Dev.dev);

           res = (exit_code == 0x01  )? (EEPROM_OK) : (EEPROM_WRITE_ERROR) ;
           return (res);
    }
    return (  EEPROM_NOT_VALIDE_ADRESS );
}



uint8_t bReadEEPROM( EEPROM_ADRESS_TYPE addr, uint8_t NotifyIndex )
{
    u8 data;
    eEEPROMRd(addr, &data,1 ,NotifyIndex );
    return (data);
}
