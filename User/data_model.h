/*
 * data_model.h
 *
 *  Created on: Jul 14, 2024
 *      Author: i.dymov
 */

#ifndef USER_DATA_MODEL_H_
#define USER_DATA_MODEL_H_

#include "system_init.h"
#include "hal_rtc.h"

#define VALID_CODE            0x98
#define VALID_CODE_ADDRES     0
/*§¢§Ý§à§Ü §Ò§Ñ§Û§ä§à§Ó§í§ç §å§ã§ä§Ó§à§Ü*/

#define CONTROL_TYPE          ( VALID_CODE_ADDRES+sizeof(uint8_t)   )
#define MB_RTU_ADDR           ( CONTROL_TYPE      +sizeof(uint8_t)  )
#define MB_PROTOCOL_TYPE      ( MB_RTU_ADDR       +sizeof(uint8_t)  )
#define LOW_VOLTAGE_ON        ( MB_PROTOCOL_TYPE  +sizeof(uint8_t)  )
#define LOW_VOLTAGE_OFF       ( LOW_VOLTAGE_ON    +sizeof(uint8_t)  )
#define HIGH_VOLTAGE_ON       ( LOW_VOLTAGE_OFF   +sizeof(uint8_t)  )
#define HIGH_VOLTAGE_OFF      ( HIGH_VOLTAGE_ON   +sizeof(uint8_t)  )
#define CONTRAST              ( HIGH_VOLTAGE_OFF  +sizeof(uint8_t)  )
#define FAN_START_TIMEOUT     ( CONTRAST          +sizeof(uint8_t) )
#define MOD_BUS_TIMEOUT       ( FAN_START_TIMEOUT +sizeof(uint8_t) )
#define IP_1                  ( MOD_BUS_TIMEOUT   +sizeof(uint8_t) )
#define IP_2                  ( IP_1   +sizeof(uint8_t) )
#define IP_3                  ( IP_2   +sizeof(uint8_t) )
#define IP_4                  ( IP_3   +sizeof(uint8_t) )
#define GATE_1                ( IP_4   +sizeof(uint8_t) )
#define GATE_2                ( GATE_1 +sizeof(uint8_t) )
#define GATE_3                ( GATE_2 +sizeof(uint8_t) )
#define GATE_4                ( GATE_3 +sizeof(uint8_t) )
#define MASK_1                ( GATE_4 +sizeof(uint8_t) )
#define MASK_2                ( MASK_1 +sizeof(uint8_t) )
#define MASK_3                ( MASK_2 +sizeof(uint8_t) )
#define MASK_4                ( MASK_3 +sizeof(uint8_t) )
#define SENSOR_COUNT          ( MASK_4 +sizeof(uint8_t) )
#define IP_PORT               ( SENSOR_COUNT +sizeof(uint8_t) )
#define SENSOR1_ZERO          ( IP_PORT +sizeof(uint16_t) )
#define SENSOR2_ZERO          ( SENSOR1_ZERO     + sizeof(uint16_t) )
#define RESURSE               ( SENSOR2_ZERO     + sizeof(uint16_t) )
#define COOF_I                ( RESURSE + sizeof(uint32_t)  )
#define COOF_P                ( COOF_I + sizeof(uint32_t) )
#define KOOFKPS               ( COOF_P + sizeof(uint32_t))
#define SETTING1              ( KOOFKPS + sizeof(uint32_t))
#define SETTING2              ( SETTING1 + sizeof(uint16_t) )
#define FILTER_LOW            ( SETTING2 + sizeof(uint16_t) )
#define FILTER_HIGH           ( FILTER_LOW + sizeof(uint16_t) )
#define MOTO_HOURS            ( FILTER_HIGH + sizeof(uint16_t) )
#define RECORD_COUNT          ( MOTO_HOURS  + sizeof(uint32_t)  )
#define RECORD_INDEX          ( RECORD_COUNT + sizeof(uint16_t) )
#define EEPROM_REGISTER_COUNT ( RECORD_INDEX  + sizeof(uint16_t) )
#define SYSTEM_START          ( RECORD_INDEX  + sizeof(uint16_t) )
#define LIGTH                 ( SYSTEM_START + sizeof(uint16_t) )
#define MODE                  ( LIGTH    + sizeof(uint16_t) )

#define RECORD_SIZE 7
#define RECORD_DATA_SIZE      50

typedef enum
{
  NORMAL_INIT = 0,
  NEW_INIT    = 1,
  INIT_ERROR  = 2,

} DATA_MODEL_INIT_t;

typedef enum
{
  TIME_0_1 = 0,
  TIME_0_5 = 1,
  TIME_1_0 = 2,
  TIME_2_0 = 3,
  TIME_3_0 = 4,
  TIME_5_0 = 5,
  TIME_10_0 = 6,
} SENSOR_COUNT_t;

#define MKV_MB_DIN 0
#define MKV_MB_RTU 1
#define MKV_MB_TCP 2

//#define EEPROM_REGISTER_COUNT   20
//#define  EEPROM_REGISTER_COUNT  20
#define RAM_REGISTER_COUNT      3
#define DATA_MODEL_REGISTERS   EEPROM_REGISTER_COUNT + RAM_REGISTER_COUNT
#define TOTAL_REGISTER_COUNT   DATA_MODEL_REGISTERS

typedef enum
{
  FILTER_ERROR = 0,
  LOW_VOLTAGE_ERROR = 1,
  HIGH_VOLTAGE_ERROR = 2,
  SETTING_ERROR   =3
} FMCH_ERROR_t;

void saveReg16( u16 reg_adress, u16 data);
void setRegFloat( u16 reg_adress, float data);
float getRegFloat(u16 reg_adress );
void SaveReg8( u16 reg_adress, u8 data);
void setReg16( u16 reg_adress, u16 data);
void setReg8( u16 reg_adress, u8 data);
u32 getReg32(u16 reg_adress );
u16 getReg16(u16 reg_adress );
void setReg32( u16 reg_adress, u32 data);
u8 getReg8( u16 reg_adress);
void saveRegFloat(u16 reg_adress, float data );
DATA_MODEL_INIT_t DataModel_Init();
void vADDRecord( uint8_t flag);
void vGetRecord( uint16_t addr,uint8_t * flag, HAL_TimeConfig_T * time, HAL_DateConfig_T * date);

#endif /* USER_DATA_MODEL_H_ */
