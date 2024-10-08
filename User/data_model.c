/*
 * data_model.c
 *
 *  Created on: Jul 14, 2024
 *      Author: i.dymov
 */
#include "data_model.h"
#include "EEPROM_25C.h"


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
               DATA_MODEL_REGISTER[LOW_VOLTAGE_ON]    = 190;
               DATA_MODEL_REGISTER[LOW_VOLTAGE_OFF]   = 199;
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
               DATA_MODEL_REGISTER[MOD_BUS_TIMEOUT ]    = 25;
               DATA_MODEL_REGISTER[FAN_START_TIMEOUT ] =  20;
               DATA_MODEL_REGISTER[CLEAN_TIMER]       =   20;
               setRegFloat(KOOFKPS , 36.0);
               setRegFloat(COOF_I,15.0);
               setRegFloat(COOF_P,5.0);
               setRegFloat(COOF_I1,0.5);
               setRegFloat(COOF_P1,5);
               setRegFloat(COOF_I2,0.5);
               setRegFloat(COOF_P2,5);
               setRegFloat(COOF_I3,0.5);
               setRegFloat(COOF_P3,5);
               setReg16(SETTING1, 900);
               setReg16(SETTING2, 600);
               setReg16(IP_PORT,502);
               setReg16( FILTER_LOW, 150);
               setReg16( FILTER_HIGH, 300);
               DATA_MODEL_REGISTER[SENSOR_COUNT]        = TIME_1_0;
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
    WriteEEPROM(reg_adress, &DATA_MODEL_REGISTER[ reg_adress], 2, 10, 2);
}


u8 VerifyAndSetReg8(u16 reg_adress, u16 data )
{
    u8 temp_data = data;
    switch (reg_adress)
    {
        case KK_SENSOR_TYPE:
        case CO2_SENSOR_TYPE:
        case H_SENSOR_TYPE:
        case MEASERING_UNIT:
        case AFTER_ZONE_SETTING:
        case PRIOR_SENSOR:
            if (data>3) return 0;

            break;
        case SENSOR_COUNT:
             if (data >6) return 0;

             break;
        case CDV_BP_CH_COUNT:
            if (data > 1) temp_data = 1;

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
        case CONTROL_TYPE:
             if (data >3 ) return 0;
             if ( data > 0)
             {
                 DATA_MODEL_REGISTER[ MB_PROTOCOL_TYPE] = temp_data ;
                 WriteEEPROM(MB_PROTOCOL_TYPE, &DATA_MODEL_REGISTER[ MB_PROTOCOL_TYPE], 1,10, 2);
             }
             break;
        case MB_PROTOCOL_TYPE:
            if ( (data!=2) || (data!=3) || (getReg8(CONTROL_TYPE)!=0)) return 0;

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

    u32 temp_int = (data[0]<<16 | data[1]);
    float * ftemp = &temp_int;
    return (*ftemp);

}

void convert_float_to_int(float fdata, u16 * data)
{
    float temp = fdata;
    u32 * temp_int = &temp;
    data[0]= ((*temp_int) >>16 ) & 0xFFFF;
    data[1]= ((*temp_int) >> 0) & 0xFFFF ;
}

float getRegFloat(u16 reg_adress )
{
    float * ptemp;
    u32 data = (u32)DATA_MODEL_REGISTER[reg_adress] | (u32)(DATA_MODEL_REGISTER[reg_adress+1])<<8 |
            (u32)DATA_MODEL_REGISTER[reg_adress+2]<<16 | (u32)(DATA_MODEL_REGISTER[reg_adress+3])<<24;
    ptemp = (float *)&data;
    return  (*ptemp);
}



/*
 *   ����ݧ��ڧ�� �٧ѧ�ڧ�� �ڧ� �ا��ߧѧݧ�
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
      for (index=0;index<4;index++)
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
 * ����ҧѧӧڧ�� �٧ѧ�ڧ�� �� �ا��ߧѧ�
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
 * ����ڧ��ڧ�� �ا��ߧѧ�
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
    WriteEEPROM(RESURSE  ,   &DATA_MODEL_REGISTER[RESURSE ],   4,10, 2 );
}
