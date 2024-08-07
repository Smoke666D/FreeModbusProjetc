/*
 * HAL_SPI.c
 *
 *  Created on: 14 灞戦偑瑜� 2024 璋�.
 *      Author: i.dymov
 */


#include "hal_spi.h"

#if MCU == CH32V2 || MCU == CH32V3
#include "hal_irq.h"

#if MCU == CH32V2
#include "ch32v20x_dma.h"
#endif
#if MCU == CH32V3
#include "ch32v30x_spi.h"
#endif
SPI_TypeDef * SPI[] ={SPI1,SPI2};

#define CTLR1_SPE_Set         ((uint16_t)0x0040)
#define CTLR1_SPE_Reset       ((uint16_t)0xFFBF)
/* SPI registers Masks */
#define CTLR1_CLEAR_Mask      ((uint16_t)0x3040)
#define I2SCFGR_CLEAR_Mask    ((uint16_t)0xF040)


void HAL_SPI_MsterBaseInit(HAL_SPI_t spi, HAL_SPI_InitTypeDef *SPI_InitStruct)
{
    if ( spi == HAL_SPI1)
     {
         RCC->APB2PCENR |= RCC_APB2Periph_SPI1;
         RCC->APB2PRSTR |= RCC_APB2Periph_SPI1;
         RCC->APB2PRSTR &= ~RCC_APB2Periph_SPI1;

    }
    else
    {
          RCC->APB1PCENR |= RCC_APB1Periph_SPI2;
          RCC->APB1PRSTR |= RCC_APB1Periph_SPI2;
          RCC->APB1PRSTR &= ~RCC_APB1Periph_SPI2;
    }
    SPI[spi]->CTLR1 = (uint16_t)((uint32_t)SPI_InitStruct->SPI_Direction |  SPI_Mode_Master |
                         SPI_InitStruct->SPI_DataSize | SPI_InitStruct->SPI_CPOL |
                         SPI_InitStruct->SPI_CPHA | SPI_InitStruct->SPI_NSS |
                         SPI_InitStruct->SPI_BaudRatePrescaler | SPI_InitStruct->SPI_FirstBit);


    SPI[spi]->I2SCFGR &= SPI_Mode_Master;
    SPI[spi]->CRCR = SPI_InitStruct->SPI_CRCPolynomial;
}

void HAL_SPI_EnableDMA(HAL_SPI_t spi )
{

    SPI[spi]->CTLR2 |= (SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx);
    SPI[spi]->CTLR1 |= CTLR1_SPE_Set;
}

void HAL_SPI_InitDMA(HAL_SPI_t spi , SPI_DATA_Size_t data_size )
{
    if ( spi == HAL_SPI1)
    {
        HAL_InitAPB2(RCC_APB2Periph_SPI1);
   }
   else
   {
       HAL_InitAPB1(RCC_APB1Periph_SPI2);
   }
    uint16_t tmpreg = 0;
    tmpreg = SPI[spi]->CTLR1;
    tmpreg &= CTLR1_CLEAR_Mask;
    tmpreg |= (uint16_t)((uint32_t)SPI_Direction_1Line_Tx | SPI_Mode_Master |
                            data_size | SPI_CPOL_Low |
                             SPI_CPHA_1Edge | SPI_NSS_Soft |
                             SPI_BaudRatePrescaler_4 | SPI_FirstBit_MSB );

    SPI[spi]->CTLR1 = tmpreg;
    SPI[spi]->I2SCFGR &= SPI_Mode_Master;
    SPI[spi]->CRCR = 0;
    SPI[spi]->CTLR2 |= SPI_I2S_DMAReq_Tx;
    SPI[spi]->CTLR1 |= CTLR1_SPE_Set;
}

void HAL_SPI_RXOveleyClear(HAL_SPI_t spi )
{
    SPI[spi]->STATR = (uint16_t)~SPI_I2S_FLAG_OVR;
}

uint8_t HAL_SPI_GetBusy(HAL_SPI_t spi )
{
    return (((SPI[spi]->STATR & SPI_I2S_FLAG_BSY) == SET) ? HAL_SET : HAL_RESET);
}

#endif
