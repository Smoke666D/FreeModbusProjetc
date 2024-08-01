/*
 * data_model.h
 *
 *  Created on: Jul 14, 2024
 *      Author: i.dymov
 */

#ifndef USER_DATA_MODEL_H_
#define USER_DATA_MODEL_H_

#include "system_init.h"

#define VALID_CODE            0x44
#define VALID_CODE_ADDRES     0


//#define EEPROM_REGISTER_COUNT   20
#define  EEPROM_REGISTER_COUNT  20
#define RAM_REGISTER_COUNT      1
#define DATA_MODEL_REGISTERS      EEPROM_REGISTER_COUNT + RAM_REGISTER_COUNT



void DataModel_Init();

#endif /* USER_DATA_MODEL_H_ */
