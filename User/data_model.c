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
               DATA_MODEL_REGISTER[CONTROL_TYPE ]     = MB_RTU;
               DATA_MODEL_REGISTER[MB_RTU_ADDR ]      = 2;
               DATA_MODEL_REGISTER[MB_PROTOCOL_TYPE] =  MB_RTU;
               DATA_MODEL_REGISTER[LOW_VOLTAGE_ON]   =190;
               DATA_MODEL_REGISTER[LOW_VOLTAGE_OFF]  = 199;
               DATA_MODEL_REGISTER[HIGH_VOLTAGE_ON ] =  250;
               DATA_MODEL_REGISTER[HIGH_VOLTAGE_OFF]  = 240;
               DATA_MODEL_REGISTER[IP_1]  = 192;
               DATA_MODEL_REGISTER[IP_2]  = 168;
               DATA_MODEL_REGISTER[IP_3]  = 1;
               DATA_MODEL_REGISTER[IP_4]  = 10;
               setReg16(SENSOR1_ZERO, 0);
               setReg16(SENSOR2_ZERO, 0);
               setReg32(RESURSE, 0);
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


void setReg16( u16 reg_adress, u16 data)
{
    DATA_MODEL_REGISTER[ reg_adress] = (u8)( data & 0xFF);
    DATA_MODEL_REGISTER[ reg_adress + 1] =(u8)( data>>8 & 0xFF);
}

void setReg8( u16 reg_adress, u8 data)
{
    DATA_MODEL_REGISTER[ reg_adress] = (u8)( data );
}
void SaveReg8( u16 reg_adress, u8 data)
{
    DATA_MODEL_REGISTER[ reg_adress] = (u8)( data );
    WriteEEPROM(reg_adress, &DATA_MODEL_REGISTER[ reg_adress], 1,10, 2);
}

u16 getReg16(u16 reg_adress )
{
    uint16_t  data =  (u16)DATA_MODEL_REGISTER[reg_adress] | (u16)(DATA_MODEL_REGISTER[reg_adress+1])<<8;
    return  data;
}

u32 getReg32(u16 reg_adress )
{
    u32 data = (u32)DATA_MODEL_REGISTER[reg_adress] | (u32)(DATA_MODEL_REGISTER[reg_adress+1])<<8 |
            (u32)DATA_MODEL_REGISTER[reg_adress+2]<<16 | (u32)(DATA_MODEL_REGISTER[reg_adress+3])<<24;
    return  (data);
}

u8 getReg8( u16 reg_adress)
{
    return DATA_MODEL_REGISTER[reg_adress];
}

void setReg32( u16 reg_adress, u32 data)
{
    DATA_MODEL_REGISTER[ reg_adress]     = (u8)( data & 0xFF);
    DATA_MODEL_REGISTER[ reg_adress + 1] = (u8)( data>>8 & 0xFF);
    DATA_MODEL_REGISTER[ reg_adress + 2] = (u8)( data>>16 & 0xFF);
    DATA_MODEL_REGISTER[ reg_adress + 3] = (u8)( data>>24 & 0xFF);
}



