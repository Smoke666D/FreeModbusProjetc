#ifndef HW_LIB_EEPROM_I2C_H_
#define HW_LIB_EEPROM_I2C_H_

#include "main.h"
#include "hal_i2c.h"


#define EEPROM_ADRESS_TYPE uint16_t
#define EEPROM_MAX_ADRRES 0x7FF


typedef enum
{
  I2C_IDLE          = 0,
  I2C_MASTER_RECIVE_START  = 1,
  I2C_MASTER_RECIVE_WRITE_ADDR =2,
  I2C_MASTER_RECIVE_WRITE_ADDR2 = 11,
  I2C_MASTER_RECIVE_DA_WRITE = 3,
  I2C_MASTER_RECIVE_ADDR = 4,
  I2C_MASTER_RECIVE_MODE = 5,
  I2C_MASTER_RECIEVE     = 6,
  I2C_MASTER_TRANSMIT_START  = 7,
  I2C_MASTER_TRANSMIT_ADDR = 8,
  I2C_MASTER_TRANSMIT_ADDR2 = 12,
  I2C_MASTER_TRANSMIT_NEXT  = 9,
  I2C_MASTER_TRANSMIT_LAST = 10,

} I2C_STATE_t;

#define I2C_ACK_FAIL    0x0001
#define I2C_STOP        0x0002
#define I2C_READY       0x0004
#define EEPROM_DATA_READY  0x0008
#define EEPROM_RX_DATA  0x0010

#define SECTOR_SIZE     16U
#define Device_ADD 0x00A0
#define EEPROM_TIME_OUT 100U
#define EEPROM_SIZE (2U *1024U)

#define ADDRESS_SIZE_BYTES  2
#define ADDRESS_DATA        (ADDRESS_SIZE_BYTES - 1)

/*   §Ñ§Ý§Ô§à§â§Ú§ä§Þ §á§à§Ý§å§é§Ñ§ß§Ú§ñ §Õ§Ñ§ß§ß§í§ç §Õ§Ý§ñ §ã§ä§Ñ§â§ê§Ö§Ô§à §Ò§Ñ§Û§ä§Ñ §Ñ§Õ§â§Ö§ã§Ñ  */
#define BYTE_SHIFT          8U
#define MSB_SHIFT           0x07
#define MSB_MASK            0x0E
#define GET_ADDR_MSB( ADDR) (( ADDR >>MSB_SHIFT ) & MSB_MASK )

typedef enum {
    EEPROM_OK,
    EEPROM_INIT_ERROR,
    EEPROM_NOT_VALIDE_ADRESS,
    EEPROM_READ_ERROR,
    EEPROM_WRITE_ERROR,
    EEPROM_ACCESS_ERROR,
    EEPROM_BUSY,
} EERPOM_ERROR_CODE_t;

typedef struct
{
   uint8_t direction;
   uint16_t start_addres;
   uint16_t size;
   uint8_t Data[SECTOR_SIZE  ];

} EEPROM_SECTOR_t;

typedef struct
{
   uint8_t ucTaskNatificationIndex;
   uint8_t ADDR[2];
   uint8_t DevAdrres;
   uint16_t data_address;
   uint16_t DataLength;
   uint16_t Index;
   uint8_t * ReciveBuffer;
   I2C_NAME_t dev;
   TaskHandle_t NotifyTaskHeandle;
   I2C_STATE_t I2C_State;
} EEPOROM;



void I2C_FSM();
EERPOM_ERROR_CODE_t eEEPROMWrFast(  EEPROM_ADRESS_TYPE addr, uint8_t * data, EEPROM_ADRESS_TYPE len );
EERPOM_ERROR_CODE_t eEEPROMWr(  EEPROM_ADRESS_TYPE addr, uint8_t * data, EEPROM_ADRESS_TYPE len , uint8_t NotifyIndex );
EERPOM_ERROR_CODE_t eEEPROMRd(  EEPROM_ADRESS_TYPE addr, uint8_t * data,   EEPROM_ADRESS_TYPE len , uint8_t NotifyIndex );
uint8_t bReadEEPROM( EEPROM_ADRESS_TYPE addr, uint8_t NotifyIndex );
void vInitEEPROM_I2C(I2C_NAME_t i2c, uint8_t prior, uint8_t subprior);

#endif
