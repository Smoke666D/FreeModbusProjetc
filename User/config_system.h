/*
 * config_system.h
 *
 *  Created on: Nov 1, 2024
 *      Author: i.dymov
 */

#ifndef USER_CONFIG_SYSTEM_H_
#define USER_CONFIG_SYSTEM_H_

#include "main.h"
#include "data_model.h"


#define CONFIG_START_BYTE 0x55


typedef enum
{
    RECIVE_STATUS_OK,
    RECIVE_STATUS_FAIL,
} CONFIG_SYSTEM_RX_STATUS_t;

typedef enum
{
   CONFIG_OK             = 0,
   ILLEGAL_CONFIG_ADDRES = 1,

} CONFIG_SYSTEM_ERROR_t;


typedef enum
{
  Byte_Config_Data = 0,
  Short_Config_Data = 1,
  Float_Config_Data  =2,
} ConfigDataType_t;

typedef enum
{
   CONFIG_RECIVE_IDLE = 0,
   CONFIG_RECIVE_START = 1,
   CONFIG_RECIVE_TYPE = 2,
   CONFIG_RECIVE_ADDR = 3,
   CONFIG_RECIVE_DATA = 4,
   CONIFG_RECIVE_CRC  = 5,
} CONFIG_RECIVE_FMS_t;


typedef struct
{
   u16 reg_addres;
   u8 data[4];
   ConfigDataType_t type;
} ConfigDataPacket_t;


TaskHandle_t * getConfigTaskHandle();
QueueHandle_t * xConfigSystemRxQueue( void );
void config_process_task(void *pvParameters);

#endif /* USER_CONFIG_SYSTEM_H_ */
