/*
 * data_model.h
 *
 *  Created on: Jul 14, 2024
 *      Author: i.dymov
 */

#ifndef USER_DATA_MODEL_H_
#define USER_DATA_MODEL_H_

#include "system_init.h"

#define VALID_CODE            0x32
#define VALID_CODE_ADDRES     0
/*§¢§Ý§à§Ü §Ò§Ñ§Û§ä§à§Ó§í§ç §å§ã§ä§Ó§à§Ü*/

#define CONTROL_TYPE         ( VALID_CODE_ADDRES+sizeof(uint8_t)  )
#define MB_RTU_ADDR          ( CONTROL_TYPE      +sizeof(uint8_t) )
#define MB_PROTOCOL_TYPE     ( MB_RTU_ADDR       +sizeof(uint8_t) )
#define LOW_VOLTAGE_ON       ( MB_PROTOCOL_TYPE  +sizeof(uint8_t) )
#define LOW_VOLTAGE_OFF      ( LOW_VOLTAGE_ON    +sizeof(uint8_t) )
#define HIGH_VOLTAGE_ON      ( LOW_VOLTAGE_OFF   +sizeof(uint8_t) )
#define HIGH_VOLTAGE_OFF     ( HIGH_VOLTAGE_ON   +sizeof(uint8_t) )
#define IP_1                 ( HIGH_VOLTAGE_OFF   +sizeof(uint8_t) )
#define IP_2                 ( IP_1   +sizeof(uint8_t) )
#define IP_3                 ( IP_2   +sizeof(uint8_t) )
#define IP_4                 ( IP_3   +sizeof(uint8_t) )
#define SENSOR1_ZERO         ( IP_4   +sizeof(uint8_t) )
#define SENSOR2_ZERO         ( SENSOR1_ZERO     + sizeof(uint16_t) )
#define RESURSE              ( SENSOR2_ZERO  +  sizeof(uint16_t)   )
#define EEPROM_REGISTER_COUNT  ( sizeof(uint32_t)  +SENSOR2_ZERO )


typedef enum
{
  NORMAL_INIT = 0,
  NEW_INIT    = 1,
  INIT_ERROR  = 2,

} DATA_MODEL_INIT_t;

#define MB_DIN 0
#define MB_RTU 1
#define MB_TCP 2

//#define EEPROM_REGISTER_COUNT   20
//#define  EEPROM_REGISTER_COUNT  20
#define RAM_REGISTER_COUNT      1
#define DATA_MODEL_REGISTERS   EEPROM_REGISTER_COUNT + RAM_REGISTER_COUNT
#define TOTAL_REGISTER_COUNT   DATA_MODEL_REGISTERS


void SaveReg8( u16 reg_adress, u8 data);
void setReg16( u16 reg_adress, u16 data);
void setReg8( u16 reg_adress, u8 data);
u32 getReg32(u16 reg_adress );
u16 getReg16(u16 reg_adress );
void setReg32( u16 reg_adress, u32 data);
u8 getReg8( u16 reg_adress);

DATA_MODEL_INIT_t DataModel_Init();

#endif /* USER_DATA_MODEL_H_ */
