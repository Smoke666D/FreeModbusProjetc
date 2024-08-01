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

typedef enum
{
  EEPROM_IDLE_STATE     = 0,
  READ_ADDR_WRITE_STATE = 1,
  READ_DATA_READ_STATE  = 2,
  WRITE_ADDR_WRITE_STATE = 3,
  WRITE_DATA_WRITE_STATE = 4,
  WRITE_STATUS_STATE     = 5,
  READ_STATUS_STATE      = 6,

} EEPROM_FSM_t;

typedef struct
{
    uint8_t ucTaskNatificationIndex;
    TaskHandle_t NotifyTaskHeandle;
    DMA_Stram_t  dma_stream_read;
    DMA_Stram_t  dma_stream_write;
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


void InitEEPROM( HAL_SPI_t spi, DMA_Stram_t stream_read, DMA_Stram_t stream_write);
#endif /* HW_LIB_EEPROM_25C_H_ */
