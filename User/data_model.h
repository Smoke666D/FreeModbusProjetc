/*
 * data_model.h
 *
 *  Created on: Jul 14, 2024
 *      Author: i.dymov
 */

#ifndef USER_DATA_MODEL_H_
#define USER_DATA_MODEL_H_

#include "system_init.h"

#define VALID_CODE            0x11
#define VALID_CODE_ADDRES     0
/*§¢§Ý§à§Ü §Ò§Ñ§Û§ä§à§Ó§í§ç §å§ã§ä§Ó§à§Ü*/
#define BYTE_BLOCK_COUNT       3
#define BYTE_BLOCK_SIZE        3
#define CONTROL_TYPE         0
#define MB_RTU_ADDR          1
#define MB_PROTOCOL_TYPE     2
#define WORD_BLOCK_SIZE      0
#define FLOAT_BLOCK_SIZE     0

#define BYTE_BLOCK_START       (VALID_CODE_ADDRES +1)
#define EEPROM_REGISTER_COUNT  ( 1 +(BYTE_BLOCK_COUNT *BYTE_BLOCK_SIZE) + WORD_BLOCK_SIZE + FLOAT_BLOCK_SIZE )


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


void DM_SetByteRegData( uint16_t addres, uint8_t data);
void DM_SetByteRegMax( uint16_t addres, uint8_t data);
void DM_SetByteRegMin( uint16_t addres, uint8_t data);
uint8_t DM_GetByteRegData( uint16_t addres);
uint8_t DM_GetByteRegMax( uint16_t addres);
uint8_t DM_GetByteRegMin( uint16_t addres);
void DM_SetByteData( uint16_t addres, uint8_t data);
uint8_t DM_GetByteData( uint16_t addres);



DATA_MODEL_INIT_t DataModel_Init();

#endif /* USER_DATA_MODEL_H_ */
