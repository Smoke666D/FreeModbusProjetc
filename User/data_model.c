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
               DATA_MODEL_REGISTER[MB_PROTOCOL_TYPE]  = MB_RTU;
               DATA_MODEL_REGISTER[MB_RTU_ADDR]       = 2;
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

void int8SetRegisterBit(uint16_t addres, uint8_t bits, uint8_t data)
{
    if ( addres < TOTAL_REGISTER_COUNT  )
    {
        if (data != 0)
            DATA_MODEL_REGISTER[addres] |= 0x1 << bits;
        else
            DATA_MODEL_REGISTER[addres] &= ~(0x1 << bits);
    }

}

uint8_t int8GetRegister(uint16_t addres)
{
    return  (( addres < TOTAL_REGISTER_COUNT   ) ? DATA_MODEL_REGISTER[addres] : 0);

}
