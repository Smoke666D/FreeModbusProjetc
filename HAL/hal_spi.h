/*
 * hal_spi.h
 *
 *  Created on: 14 灞戦偑瑜� 2024 璋�.
 *      Author: i.dymov
 */

#ifndef HAL_HAL_SPI_H_
#define HAL_HAL_SPI_H_

#include "hal_config.h"

#if MCU== CH32V2 || MCU == CH32V3

#if MCU == CH32V2
#include "ch32v20x_spi.h"
#endif
#if MCU == CH32V3
#include "ch32v30x_spi.h"
#endif

typedef enum
{
HAL_SPI_FULLDuplex = SPI_Direction_2Lines_FullDuplex,
HAL_SPI_2LINERX    = SPI_Direction_2Lines_RxOnly,
HAL_SPI_RX         = SPI_Direction_1Line_Rx,
HAL_SPI_TX         = SPI_Direction_1Line_Tx,
} HAL_SPI_Direction_t;

typedef enum
{
    SPI_8bit = SPI_DataSize_8b,
    SPI_16bit = SPI_DataSize_16b
} SPI_DATA_Size_t;


#endif

typedef struct
{
  HAL_SPI_Direction_t SPI_Direction;
  SPI_DATA_Size_t     SPI_DataSize;
  uint16_t SPI_CPOL;
  uint16_t SPI_CPHA;
  uint16_t SPI_NSS;
  uint16_t SPI_BaudRatePrescaler;
  uint16_t SPI_FirstBit;
  uint16_t SPI_CRCPolynomial;
}HAL_SPI_InitTypeDef;




typedef enum
{
    SPI_HARD_NSS,
    SPI_SOFT_NSS
} SPI_NSS_t;

typedef enum
{
   HAL_SPI1 = 0,
   HAL_SPI2 = 1
} HAL_SPI_t;


void HAL_SPI_MsterBaseInit(HAL_SPI_t spi, HAL_SPI_InitTypeDef *SPI_InitStruct);
void HAL_SPI_InitDMA(HAL_SPI_t spi , SPI_DATA_Size_t data_size );
void HAL_SPI_RXOveleyClear(HAL_SPI_t spi );
uint8_t HAL_SPI_GetBusy(HAL_SPI_t spi );

#endif /* HAL_HAL_SPI_H_ */
