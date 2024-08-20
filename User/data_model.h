/*
 * data_model.h
 *
 *  Created on: Jul 14, 2024
 *      Author: i.dymov
 */

#ifndef USER_DATA_MODEL_H_
#define USER_DATA_MODEL_H_

#include "system_init.h"

#define VALID_CODE            0x22
#define VALID_CODE_ADDRES     0
#define MB_PROTOCOL_TYPE      (VALID_CODE_ADDRES+1)
#define MB_RTU_ADDR           (MB_PROTOCOL_TYPE +1 )
#define EEPROM_REGISTER_COUNT  (MB_RTU_ADDR +1 )


typedef enum
{
  NORMAL_INIT = 0,
  NEW_INIT    = 1,
  INIT_ERROR  = 2,

} DATA_MODEL_INIT_t;

#define MB_RTU 0x00
#define MB_TCP 0x01

//#define EEPROM_REGISTER_COUNT   20
//#define  EEPROM_REGISTER_COUNT  20
#define RAM_REGISTER_COUNT      1
#define DATA_MODEL_REGISTERS      EEPROM_REGISTER_COUNT + RAM_REGISTER_COUNT
#define TOTAL_REGISTER_COUNT   DATA_MODEL_REGISTERS


DATA_MODEL_INIT_t DataModel_Init();
uint8_t int8GetRegister(uint16_t addres);
void int8SetRegister( uint16_t addres , uint8_t data );

#endif /* USER_DATA_MODEL_H_ */
