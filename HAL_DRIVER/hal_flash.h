/*
 * hal_flash.h
 *
 *  Created on: 15 мая 2024 г.
 *      Author: i.dymov
 */

#ifndef HAL_HAL_FLASH_H_
#define HAL_HAL_FLASH_H_

#include "main.h"

#if CORE == APM32
#include "apm32f4xx_fmc.h"
#endif





void HAL_FLASH_Lock();
void HAL_FLASH_Unlock();

typedef enum
{
  FLASH_OK,
  FLASH_ERROR_ADR,
  FLASH_ERROR_LENGTH,
  FLASH_ERROR_UNLOCK,
  FLASH_ERROR_LOCK,
  FLASH_ERROR_ACCESS,
  FLASH_ERROR_WRITING,
  FLASH_ERROR_WRITING_TIMEOUT,
  FLASH_ERROR_VERIFICATION
} FLASH_STATE;

#if CORE == APM32
FMC_STATUS_T  HAL_FLASH_WriteWord( uint16_t * src, uint32_t  address);
FMC_STATUS_T  HAL_FLASH_WriteByByte( uint8_t * src, uint32_t  address, uint32_t len);
FMC_STATUS_T  HAL_FLASH_WriteByWord( uint16_t * src, uint32_t  address, uint32_t len);
FMC_STATUS_T  HAL_FLASH_WriteBy32Word( uint32_t * src, uint32_t  address, uint32_t len);
FMC_STATUS_T  HAL_FLASH_WriteBy64Word( uint64_t * src, uint32_t  address, uint32_t len);
FMC_STATUS_T HAL_FLASH_ProgrammByte( uint32_t address, uint8_t data);
FMC_STATUS_T HAL_FLASH_ProgrammWord( uint32_t address, uint16_t data);
void HAL_FLASH_ErasePage(uint32_t Page_Address);
FMC_STATUS_T HAL_FLASH_EraseSector(FMC_SECTOR_T sector);
#endif

#if CORE == WCH32V2

FLASH_STATE HAL_FLASH_WriteByWord( uint8_t * src, uint8_t * dest, uint32_t len);
FLASH_STATE HAL_FLASH_ErasePage(uint32_t Page_Address);

#endif

#endif /* HAL_HAL_FLASH_H_ */
