/*
 * data_model.c
 *
 *  Created on: Jul 14, 2024
 *      Author: i.dymov
 */
#include "data_model.h"
#include "EEPROM_25C.h"
#include "stdlib.h"
#include "math.h"

u8 DATA_MODEL_REGISTER[DATA_MODEL_REGISTERS];


DATA_MODEL_INIT_t DataModel_Init()
{
    memset(DATA_MODEL_REGISTER,0,EEPROM_REGISTER_COUNT);
    if (   ReadEEPROMData(0x00 ,DATA_MODEL_REGISTER , EEPROM_REGISTER_COUNT, 100 ,2) == EEPROM_OK)
    {
           if (DATA_MODEL_REGISTER[VALID_CODE_ADDRES]!=VALID_CODE )
           {
               memset(DATA_MODEL_REGISTER,0,EEPROM_REGISTER_COUNT);
               DATA_MODEL_REGISTER[VALID_CODE_ADDRES] = VALID_CODE;
               DATA_MODEL_REGISTER[SOFT_V1 ]          =  SW_V ;
               DATA_MODEL_REGISTER[SOFT_V2 ]          =  SW_V2 ;
               DATA_MODEL_REGISTER[SOFT_V3 ]          =  SW_V3;
               DATA_MODEL_REGISTER[CONTROL_TYPE ]     = MKV_MB_RTU;
               DATA_MODEL_REGISTER[MB_RTU_ADDR ]      = 1;
               DATA_MODEL_REGISTER[MB_PROTOCOL_TYPE]  = MKV_MB_RTU;
               DATA_MODEL_REGISTER[LOW_VOLTAGE_ON]    = 187;
               DATA_MODEL_REGISTER[LOW_VOLTAGE_OFF]   = 197;
               DATA_MODEL_REGISTER[HIGH_VOLTAGE_ON ]  = 250;
               DATA_MODEL_REGISTER[HIGH_VOLTAGE_OFF]  = 240;
               DATA_MODEL_REGISTER[IP_1]              = 192;
               DATA_MODEL_REGISTER[IP_2]              = 168;
               DATA_MODEL_REGISTER[IP_3]              = 1;
               DATA_MODEL_REGISTER[IP_4]              = 10;
               DATA_MODEL_REGISTER[GATE_1]              = 192;
               DATA_MODEL_REGISTER[GATE_2]              = 168;
               DATA_MODEL_REGISTER[GATE_3]              = 1;
               DATA_MODEL_REGISTER[GATE_4]              = 1;
               DATA_MODEL_REGISTER[MASK_1]              = 255;
               DATA_MODEL_REGISTER[MASK_2]              = 255;
               DATA_MODEL_REGISTER[MASK_3]              = 255;
               DATA_MODEL_REGISTER[MASK_4]              = 0;
               DATA_MODEL_REGISTER[CONTRAST]            = 50;
               DATA_MODEL_REGISTER[MOD_BUS_TIMEOUT ]    = 5;
               DATA_MODEL_REGISTER[FAN_START_TIMEOUT ] =  20;
               DATA_MODEL_REGISTER[CLEAN_TIMER]        =  20;
               DATA_MODEL_REGISTER[CDV_BP_CH_COUNT]    =  1;
               DATA_MODEL_REGISTER[SENSOR_COUNT]       =  TIME_5_0;
               setRegFloat(KOOFKPS , 36.0);
               setRegFloat(COOF_I,10.0);
               setRegFloat(COOF_P,5.0);
               setRegFloat(COOF_I1,15.0);
               setRegFloat(COOF_P1,5.0);
               setRegFloat(F_CHANNEL,0.0314);
               setReg16(SETTING1, 900);
               setReg16(SETTING2, 600);
               setReg16(IP_PORT,502);
               setReg16( FILTER_LOW, 150);
               setReg16( FILTER_HIGH, 300);
               setReg16(SETTING_MIN , 150);
               setReg16(SETTING_MID , 300);
               setReg16(SETTING_MAX , 600);
               setReg16(CH1_SETTING  , 700);
               setReg16(CH2_SETTING  , 800);
               setReg16(MIN_SET, 10);
               setReg16(MAX_SET,11);
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


void vDataModelResetJournal()
{
    saveReg16(RECORD_COUNT, 0);
}

void saveReg16( u16 reg_adress, u16 data)
{
    switch (reg_adress)
    {
        case SETTING1:
        case SETTING2:
            USER_SetSettingChange();
            break;
    }
    setReg16(reg_adress,data);
    printf("set_reg\r\n");
    WriteEEPROM(reg_adress, &DATA_MODEL_REGISTER[ reg_adress], 2, 10, 2);
}


u8 VerifyAndSetReg8(u16 reg_adress, u16 data )
{
    u8 temp_data = data;
    switch (reg_adress)
    {
        case INPUT_SENSOR_MODE:

        case MEASERING_UNIT:
        case AFTER_ZONE_SETTING:
        case PRIOR_SENSOR:
            if (data>3) return 0;

            break;
        case SENSOR_COUNT:
             if (data >6) return 0;

             break;
        case CDV_BP_CH_COUNT:
            if (data > 2) temp_data = 2;

             break;
        case LIGTH:
        case MODE:
              if (data > 1) temp_data = 1;
              break;
        case CONTRAST:
              if (data>100)  temp_data = 100;

              break;
        case MB_RTU_ADDR:
             if ((data >100) && (data==0)) return (0);

             break;
        case CDV_CONTOROL :
            printf(" new_state = %i\r\n", data);
            if  ( data > 4 )
                return 0;
            break;
        case CONTROL_TYPE:
             if (data >3 ) return 0;
             if ( data > 0)
             {
                 DATA_MODEL_REGISTER[ MB_PROTOCOL_TYPE] = temp_data ;
                 WriteEEPROM(MB_PROTOCOL_TYPE, &DATA_MODEL_REGISTER[ MB_PROTOCOL_TYPE], 1,10, 2);
             }
             break;
        case MB_PROTOCOL_TYPE:
            if ((getReg8(CONTROL_TYPE)!=MKV_MB_DIN)) return 0;
            break;
        case AIN1_TYPE:
        case AIN2_TYPE:
        case AIN3_TYPE:
            if (temp_data>2) return 0;
            break;
        default:
            break;
    }
    DATA_MODEL_REGISTER[ reg_adress] = temp_data ;

    return 1;
}


void setReg8( u16 reg_adress, u8 data)
{
    DATA_MODEL_REGISTER[ reg_adress] = (u8)( data );
}
void SaveReg8( u16 reg_adress, u16 data)
{
    if (VerifyAndSetReg8( reg_adress,  data))
    {
        WriteEEPROM(reg_adress, &DATA_MODEL_REGISTER[ reg_adress], 1,10, 2);
    }
}

u16 getReg16(u16 reg_adress )
{
    uint16_t  data =  (u16)DATA_MODEL_REGISTER[reg_adress] | (u16)(DATA_MODEL_REGISTER[reg_adress+1])<<8;
    return  data;
}


int16_t getRegi16(u16 reg_adress )
{
    int16_t  data = (int16_t) (DATA_MODEL_REGISTER[reg_adress] | (DATA_MODEL_REGISTER[reg_adress+1])<<8);
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

void setRegFloat( u16 reg_adress, float data)
{
    float temp = data;
    u8 * pdata = (u8 *)&temp;
    DATA_MODEL_REGISTER[ reg_adress]     =  pdata[0];
    DATA_MODEL_REGISTER[ reg_adress + 1] =  pdata[1];
    DATA_MODEL_REGISTER[ reg_adress + 2] =  pdata[2];
    DATA_MODEL_REGISTER[ reg_adress + 3] =  pdata[3];
}
void saveRegFloat(u16 reg_adress, float data )
{

    setRegFloat(reg_adress,data);
    WriteEEPROM(reg_adress, &DATA_MODEL_REGISTER[ reg_adress], 4,10, 2);
    if ((reg_adress ==COOF_I ) || (reg_adress ==COOF_P))
            UPDATE_COOF();
}





float convert_int_to_float( u16 * data)
{

    u32 temp_int =( data[0]<<0 | data[1]<<16);
    float * ftemp = (float *) &temp_int;
    return (*ftemp);

}

void convert_float_to_int(float fdata, u16 * data)
{
    float temp = fdata;
    u32 * temp_int =(u32*) &temp;
    data[0]= ((*temp_int) >>00 ) & 0xFFFF;
    data[1]= ((*temp_int) >>16) & 0xFFFF ;
}

float getRegFloat(u16 reg_adress )
{
    float * ptemp;
    u32 data = (u32)DATA_MODEL_REGISTER[reg_adress] | (u32)(DATA_MODEL_REGISTER[reg_adress+1])<<8 |
            (u32)DATA_MODEL_REGISTER[reg_adress+2]<<16 | (u32)(DATA_MODEL_REGISTER[reg_adress+3])<<24;
    ptemp = (float *)&data;
    return  (*ptemp);
}


u16 DataModel_SetLToPressere(float L)
{
   float K = getRegFloat(KOOFKPS);
  if ( ( L!=0 ) && ( K!=0 ) )
   return pow(L/K,2);
  else
    return 0;

}

u16 DataModel_SetVToPressere(float V)
{
  float F = getRegFloat(F_CHANNEL);
  if ( V!=0 )
   return  DataModel_SetLToPressere (V* F *3600.0);
  else
    return 0;

}


float  DataModel_GetPressureToL(u16 pressure)
{
    return (float) sqrt((float)pressure)*getRegFloat(KOOFKPS) ;
}

float  DataModel_GetPressureToV(u16 pressure)
{
    float L = DataModel_GetPressureToL(pressure);
    float F = getRegFloat(F_CHANNEL);
    if (F == 0) return (0);
            else
    return  L/F/3600.0 ;
}

float DataModelGetCDVSettings( u16 pressure)
{
    float res = (float)pressure;
    switch ( getReg8(MEASERING_UNIT) )
                      {
                           case 0:
                               res = DataModel_GetPressureToL( res);
                               break;
                           case 1:
                               res = DataModel_GetPressureToV( res);
                               break;
                           default:

                               break;
                      }
  return (res);
}
/*
 *   §±§à§Ý§å§é§Ú§ä§î §Ù§Ñ§á§Ú§ã§î §Ú§Ù §Ø§å§â§ß§Ñ§Ý§Ñ
 */
void vGetRecord( uint16_t addr,uint8_t * flag, HAL_TimeConfig_T * time, HAL_DateConfig_T * date)
{
  uint16_t total     =  getReg16( RECORD_COUNT );
  uint16_t cur_index =  getReg16( RECORD_INDEX );
  uint8_t pData[RECORD_DATA_SIZE];
  uint16_t index = 0;
  if ( addr  <= total )
  {
      if ( (cur_index - total) >= 0  )
      {
          index = addr;
      }
      else
      {
          if ((cur_index + index) < total)
          {
              index = cur_index + index;
          }
          else
          {
              index = addr - (total - cur_index + 1);
          }
      }
      ReadEEPROMData(EEPROM_REGISTER_COUNT+  index*RECORD_SIZE  ,pData , RECORD_SIZE ,10,2);
      u8 temp = pData[0];
      for (index=0;index<5;index++)
      {
          if (temp & 0x01) break;
          temp =temp>>1;
      }
      *flag =         index;
      date->date    = pData[1];
      date->month   = pData[2];
      date->year    = pData[3];
      time->hours   = pData[4];
      time->minutes = pData[5];
      time->seconds = pData[6];
  }
  else
  {
      *flag = 0;
  }
}
/*
 * §¥§à§Ò§Ñ§Ó§Ú§ä§î §Ù§Ñ§á§Ú§ã§î §Ó §Ø§å§â§ß§Ñ§Ý
 */
void vADDRecord( uint8_t flag)
{
    HAL_TimeConfig_T time;
    HAL_DateConfig_T date;

    uint16_t  index = getReg16(RECORD_INDEX);
    uint8_t DataBuffer[RECORD_SIZE];
    DataBuffer[0] =  flag;
    HAL_RTC_ReadTime( &time);
    HAL_RTC_ReadDate(&date);

    DataBuffer[1] =  date.date;
    DataBuffer[2] =  date.month;
    DataBuffer[3] =  date.year;
    DataBuffer[4] =  time.hours;
    DataBuffer[5] =  time.minutes;
    DataBuffer[6] =  time.seconds;
    WriteEEPROM(EEPROM_REGISTER_COUNT +  index* RECORD_SIZE,  DataBuffer, RECORD_SIZE ,10, 2);

    if ( ++index >= RECORD_DATA_SIZE ) index = 0;
    setReg16(RECORD_INDEX, index);
    if (getReg16(RECORD_COUNT) < ( RECORD_DATA_SIZE)  )
    {
        setReg16(RECORD_COUNT,  getReg16(RECORD_COUNT) +1 );
    }
    WriteEEPROM(RECORD_COUNT , &DATA_MODEL_REGISTER[RECORD_COUNT], 4 ,10, 2);
}

/*
 * §°§é§Ú§ã§ä§Ú§ä§î §Ø§å§â§ß§Ñ§Ý
 */
void JournalClear()
{
    setReg16(RECORD_INDEX, 0);
    setReg16(RECORD_COUNT, 0);
    WriteEEPROM(RECORD_COUNT , &DATA_MODEL_REGISTER[RECORD_COUNT], 4,10, 2 );
}

void SaveBeforePowerOff()
{

    WriteEEPROM(MOTO_HOURS , &DATA_MODEL_REGISTER[MOTO_HOURS], 4,10, 2 );
    WriteEEPROM(RESURSE  ,   &DATA_MODEL_REGISTER[RESURSE ],   1,10, 2 );
    WriteEEPROM(SENSOR1_ZERO, &DATA_MODEL_REGISTER[SENSOR1_ZERO],   2,10, 2 );
    WriteEEPROM(SENSOR2_ZERO, &DATA_MODEL_REGISTER[SENSOR2_ZERO],   2,10, 2 );
}
