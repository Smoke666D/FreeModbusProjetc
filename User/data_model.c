/*
 * data_model.c
 *
 *  Created on: Jul 14, 2024
 *      Author: i.dymov
 */
#include "data_model.h"
#include "EEPROM_25C.h"

u8 REGISTER[DATA_MODEL_REGISTERS];
static const u8 default_data[]= { VALID_CODE, 2, 13, 13, 0x20 ,1};

void DataModel_Init()
{

    memset(REGISTER,0,DATA_MODEL_REGISTERS);
    if (  ReadEEPROMData(0x00 ,REGISTER , DATA_MODEL_REGISTERS, 10 ,2) == EEPROM_OK)
    {
            if (REGISTER[VALID_CODE_ADDRES]!=VALID_CODE )
            {
                memcpy(REGISTER,default_data,6);
            }
    }
}

