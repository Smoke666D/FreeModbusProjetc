/*
 * data_model.c
 *
 *  Created on: Jul 14, 2024
 *      Author: i.dymov
 */
#include "data_model.h"
#include "EEPROM_25C.h"

u8 DATA_MODEL_REGISTER[EEPROM_REGISTER_COUNT];


DATA_MODEL_INIT_t DataModel_Init()
{

    memset(DATA_MODEL_REGISTER,0,EEPROM_REGISTER_COUNT);
    if (   ReadEEPROMData(0x00 ,DATA_MODEL_REGISTER , EEPROM_REGISTER_COUNT, 100 ,2) == EEPROM_OK)
    {
           if (DATA_MODEL_REGISTER[VALID_CODE_ADDRES]!=VALID_CODE )
           {
               DATA_MODEL_REGISTER[VALID_CODE_ADDRES] = VALID_CODE;
               DM_SetByteRegData(CONTROL_TYPE, MB_RTU );
               DM_SetByteRegMax( CONTROL_TYPE, MB_TCP );
               DM_SetByteRegMin( CONTROL_TYPE, MB_DIN );
               DM_SetByteRegData(MB_RTU_ADDR, 2 );
               DM_SetByteRegMax( MB_RTU_ADDR, 99 );
               DM_SetByteRegMin( MB_RTU_ADDR, 1 );
               DM_SetByteRegData(MB_PROTOCOL_TYPE, MB_RTU );
               DM_SetByteRegMax( MB_PROTOCOL_TYPE, MB_TCP );
               DM_SetByteRegMin( MB_PROTOCOL_TYPE, MB_RTU );
               if (WriteEEPROM(0x00 ,DATA_MODEL_REGISTER , EEPROM_REGISTER_COUNT, 1000 ,2) == EEPROM_OK) printf("EEPROMwtiye\r\n");
               ReadEEPROMData(0x00 ,DATA_MODEL_REGISTER , EEPROM_REGISTER_COUNT, 100 ,2);
               return (NEW_INIT);
           }
           else
           {
               return (NORMAL_INIT);
           }

   }
    else
        return (INIT_ERROR);
}


void DM_SetByteRegData( uint16_t addres, uint8_t data)
{
    DATA_MODEL_REGISTER[BYTE_BLOCK_START + addres*BYTE_BLOCK_SIZE] = data;
}
void DM_SetByteRegMax( uint16_t addres, uint8_t data)
{
    DATA_MODEL_REGISTER[BYTE_BLOCK_START + addres*BYTE_BLOCK_SIZE +1] = data;
}
void DM_SetByteRegMin( uint16_t addres, uint8_t data)
{
    DATA_MODEL_REGISTER[BYTE_BLOCK_START + addres*BYTE_BLOCK_SIZE +2 ] = data;
}
uint8_t DM_GetByteRegData( uint16_t addres)
{
    return (DATA_MODEL_REGISTER[BYTE_BLOCK_START + addres*BYTE_BLOCK_SIZE]);
}
uint8_t DM_GetByteRegMax( uint16_t addres)
{
    return (DATA_MODEL_REGISTER[BYTE_BLOCK_START + addres*BYTE_BLOCK_SIZE+1]);
}
uint8_t DM_GetByteRegMin( uint16_t addres)
{
    return (DATA_MODEL_REGISTER[BYTE_BLOCK_START + addres*BYTE_BLOCK_SIZE+2]);
}




