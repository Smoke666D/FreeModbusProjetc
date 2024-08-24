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

void SPI1_IRQHandler ( void )  __attribute__((interrupt()));
void SPI2_IRQHandler ( void )  __attribute__((interrupt()));
void SPI3_IRQHandler ( void )  __attribute__((interrupt()));
SPI_TypeDef * SPI[] ={SPI1,SPI2};

typedef struct
{
    void (* f)( void );

} SPI_CFG_t;

#define CTLR1_SPE_Set         ((uint16_t)0x0040)
#define CTLR1_SPE_Reset       ((uint16_t)0xFFBF)
/* SPI registers Masks */
#define CTLR1_CLEAR_Mask      ((uint16_t)0x3040)
#define I2SCFGR_CLEAR_Mask    ((uint16_t)0xF040)


 SPI_CFG_t CallBack[3][2];

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

void HAL_SPI_ConfgiIT(HAL_SPI_t spi , void (* spi_rx_it_callback) ( void ),  void (* spi_tx_it_callback) ( void ), uint8_t prior, uint8_t subprior)
{


#if MCU== CH32V3
    switch (spi)
    {
        case HAL_SPI1:
            PFIC_IRQ_ENABLE_PG2(SPI1_IRQn ,prior, subprior);
            if (spi_rx_it_callback!= NULL)
            {

                CallBack[0][0].f = spi_rx_it_callback;
            }
            if (spi_tx_it_callback!= NULL)
            {

                CallBack[0][1].f = spi_tx_it_callback;
            }
            break;
        case HAL_SPI2:
            PFIC_IRQ_ENABLE_PG2(SPI2_IRQn ,prior, subprior);
            if (spi_rx_it_callback!= NULL)
             {

                 CallBack[1][0].f = spi_rx_it_callback;
             }
                     if (spi_tx_it_callback!= NULL)
                     {

                         CallBack[1][1].f = spi_tx_it_callback;
                     }
            break;
        default:
            PFIC_IRQ_ENABLE_PG2(SPI3_IRQn ,prior, subprior);
            if (spi_rx_it_callback!= NULL)
                         {

                             CallBack[2][0].f = spi_rx_it_callback;
                         }
                                 if (spi_tx_it_callback!= NULL)
                                 {

                                     CallBack[2][1].f = spi_tx_it_callback;
                                 }
            break;
    }


#endif
    SPI_Cmd(SPI[spi], ENABLE);


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



u8 HAL_SPI_RXOveleyClear(HAL_SPI_t spi )
{
    if (SPI[spi]->STATR & 0x040)
    {
        return (SPI[spi]->DATAR);
    }
    else
        return (0);
}

u16 HAL_SPI_GetData(HAL_SPI_t spi)
{
    return (SPI[spi]->DATAR);
}

uint8_t HAL_SPI_GetBusy(HAL_SPI_t spi )
{
    return (((SPI[spi]->STATR & SPI_I2S_FLAG_BSY) == SET) ? HAL_SET : HAL_RESET);
}

void HAL_SPI_Send(HAL_SPI_t spi, u8 data )
{
   SPI[spi]->DATAR = data;
}

void SPI1_IRQHandler ( void )
{
    if( SPI_I2S_GetITStatus( SPI1, SPI_I2S_IT_TXE ) != RESET )
    {
        CallBack[0][1].f();
    }
    if( SPI_I2S_GetITStatus( SPI1, SPI_I2S_IT_RXNE ) != RESET )
    {
        CallBack[0][0].f();
    }
}

void SPI2_IRQHandler ( void )
{

    if( SPI_I2S_GetITStatus( SPI2, SPI_I2S_IT_TXE ) != RESET )
        {
            CallBack[1][1].f();
        }
        if( SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE ) != RESET )
        {

            CallBack[1][0].f();
        }
}
void SPI3_IRQHandler ( void )
        {
    if( SPI_I2S_GetITStatus( SPI3, SPI_I2S_IT_TXE ) != RESET )
            {
                CallBack[2][1].f();
            }
            if( SPI_I2S_GetITStatus( SPI3, SPI_I2S_IT_RXNE ) != RESET )
            {
                CallBack[2][0].f();
            }
        }


#endif
