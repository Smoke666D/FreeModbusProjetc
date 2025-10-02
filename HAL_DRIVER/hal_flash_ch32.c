/*
 * hal_flash.c
 *
 *  Created on: 15 мая 2024 г.
 *      Author: i.dymov
 */


#include "hal_flash.h"

#if CORE == WCH32V2

/* FLASH Keys */
#define FLASH_KEY1                 ((uint32_t)0x45670123)
#define FLASH_KEY2                 ((uint32_t)0xCDEF89AB)
#define CR_LOCK_Set                ((uint32_t)0x00000080)
/* Flash Control Register bits */
#define CR_PG_Set                  ((uint32_t)0x00000001)
#define CR_PG_Reset                ((uint32_t)0xFFFFFFFE)
#define CR_PER_Set                 ((uint32_t)0x00000002)
#define CR_PER_Reset               ((uint32_t)0xFFFFFFFD)
#define CR_MER_Set                 ((uint32_t)0x00000004)
#define CR_MER_Reset               ((uint32_t)0xFFFFFFFB)
#define CR_OPTPG_Set               ((uint32_t)0x00000010)
#define CR_OPTPG_Reset             ((uint32_t)0xFFFFFFEF)
#define CR_OPTER_Set               ((uint32_t)0x00000020)
#define CR_OPTER_Reset             ((uint32_t)0xFFFFFFDF)
#define CR_STRT_Set                ((uint32_t)0x00000040)
#define CR_LOCK_Set                ((uint32_t)0x00000080)
#define CR_FAST_LOCK_Set           ((uint32_t)0x00008000)
#define CR_PAGE_PG                 ((uint32_t)0x00010000)
#define CR_PAGE_ER                 ((uint32_t)0x00020000)
#define CR_BER32                   ((uint32_t)0x00040000)
#define CR_BER64                   ((uint32_t)0x00080000)
#define CR_PG_STRT                 ((uint32_t)0x00200000)
/* Delay definition */
#define EraseTimeout               ((uint32_t)0x000B0000)
#define ProgramTimeout             ((uint32_t)0x00005000)



FLASH_Status HAL_FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data)
{
    FLASH_Status status = FLASH_WaitForLastOperation(ProgramTimeout);
    if(status == FLASH_COMPLETE)
    {
        FLASH->CTLR |= CR_PG_Set;
        *(__IO uint16_t *)Address = Data;
        status = FLASH_WaitForLastOperation(ProgramTimeout);
        FLASH->CTLR &= CR_PG_Reset;
    }
    return status;
}


 FLASH_STATE  HAL_FLASH_WriteByWord( uint8_t * src, uint8_t * dest, uint32_t len)
{
	 for (uint32_t i=0U; i<len; i+=2U )
	 {
	       if (  HAL_FLASH_ProgramHalfWord( ( uint32_t )( dest + i ), *( uint16_t* )( src + i ) ) !=  FLASH_COMPLETE)
	       {
	           return (FLASH_ERROR_WRITING);
	       }
	       else if ( *( uint16_t* )( src + i ) != *( uint16_t* )( dest + i ) )
	       {
	             return ( FLASH_ERROR_VERIFICATION);
	       }
	  }
	 return (FLASH_OK);
}


 void HAL_FLASH_Unlock()
 {
     FLASH->KEYR = FLASH_KEY1;
     FLASH->KEYR = FLASH_KEY2;
 }

 void HAL_FLASH_Lock()
 {
     FLASH->CTLR |= CR_LOCK_Set;
 }
/*
 *  Функция стирания заднной страницы
 *  Page_Address  - адрес начала страницы памяти
 */
 FLASH_STATE  HAL_FLASH_ErasePage(uint32_t Page_Address)
 {
     HAL_FLASH_Unlock();
     FLASH_Status status = FLASH_WaitForLastOperation(EraseTimeout);

     if(status == FLASH_COMPLETE)
      {
           FLASH->CTLR |= CR_PER_Set;
           FLASH->ADDR = Page_Address;
           FLASH->CTLR |= CR_STRT_Set;
           status = FLASH_WaitForLastOperation(EraseTimeout);
           FLASH->CTLR &= CR_PER_Reset;

      }
     if (status == FLASH_COMPLETE)
         return FLASH_OK;
     else
         return  FLASH_ERROR_UNLOCK;
 }
#endif
