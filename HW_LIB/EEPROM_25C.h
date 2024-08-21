/*
 * EEPROM_25C.h
 *
 *  Created on: Jul 12, 2024
 *      Author: i.dymov
 */

#ifndef HW_LIB_EEPROM_25C_H_
#define HW_LIB_EEPROM_25C_H_

#include "hal_spi.h"
#include "hal_dma.h"
#include "main.h"

#define WREN_COMMAND  0x06  //ENABLE WRITE OPERATION COMMAND
#define WRDI_COONAND  0x04  //DISABLE WRITE OPERATION COMMAND
#define RDSR_COMMAND  0x05  //Read status regisrer
#define WRSR_COMMNAD  0x01  //Wtire status register
#define READ_COMMAND  0x03  //Read data memory
#define WRITE_COMMAND 0x02  //Wrute data memory

#define WPEN_BIT      0x80
#define RDY_BIT       0x01
#define WEL_BIT       0x02
#define BP0_BIT       0x04
#define BP1_BIT       0x08

#define EEPROM_SIZE 32768
#define SECTOR_SIZE 64

typedef enum
{
  EEPROM_IDLE_STATE     = 0,
  READ_ADDR_WRITE_STATE = 1,
  READ_ADDR_WRITE_STATE1 = 2,
  READ_DATA_READ_STATE  = 3,
  READ_DATA_READ_STATE1 = 10,
  WRITE_ADDR_WRITE_STATE = 4,
  WRITE_ADDR_WRITE_STATE1 = 5,
  WRITE_DATA_WRITE_STATE = 6,
  WRITE_STATUS_STATE     = 7,
  READ_STATUS_STATE      = 8,
  WRITE_COMMAND_STATE   = 9,
  WRITE_COMMANDw        = 11,
  WRITE_ADRR_LSB       = 12,
  WRITE_ADRR_LSBr       = 13,
  WRTIE_STATE_1,
  WRTIE_STATE_2,
  WRTIE_STATE_3,
  WRTIE_STATE_4,
  WRTIE_STATE_5,
  WRTIE_STATE_6,
  WRTIE_STATE_7,
  WRTIE_STATE_8,
} EEPROM_FSM_t;

typedef struct
{
    uint8_t ucTaskNatificationIndex;
    TaskHandle_t NotifyTaskHeandle;

    HAL_SPI_t spi;
    EEPROM_FSM_t state;
    u16          data_addres;
    u8 *         pdata;
    u16          data_index;
    u16          data_size;
    u8           dma_command[3];

} EEPROM_25C_t;

typedef enum {
    EEPROM_OK,
    EEPROM_INIT_ERROR,
    EEPROM_NOT_VALIDE_ADRESS,
    EEPROM_READ_ERROR,
    EEPROM_WRITE_ERROR,
    EEPROM_ACCESS_ERROR,
    EEPROM_BUSY,
} EERPOM_ERROR_CODE_t;

EERPOM_ERROR_CODE_t ReadEEPROMData( u16 data_adress, u8 * data, u16 data_size, u32 timeout, u8 TASK_NOTIFICATION_INDEX);
EERPOM_ERROR_CODE_t WriteEEPROMData( u16 data_adress, u8 * data, u16 data_size, u32 timeout, u8 TASK_NOTIFICATION_INDEX);
EERPOM_ERROR_CODE_t WriteEEPROM( u16 data_adress, u8 * data, u16 data_size, u32 timeout, u8 TASK_NOTIFICATION_INDEX);
EERPOM_ERROR_CODE_t SetEEPROMUnprotect( u32 timeout,u8 TASK_NOTIFICATION_INDEX);
void InitEEPROM( HAL_SPI_t spi);
#endif /* HW_LIB_EEPROM_25C_H_ */
