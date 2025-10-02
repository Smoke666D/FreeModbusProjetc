/*
 * hal_dma.h
 *
 *  Created on: 25
 *      Author: i.dymov
 */

#ifndef HAL_HAL_DMA_H_
#define HAL_HAL_DMA_H_

#include "main.h"
//#include "system_init.h"




#if CORE == WCH32V3
#define  DMA2_PRESENT
#endif

#if CORE == WCH32V2 || CORE == WCH32V3

typedef enum
{
    DMA1_CH1 =0,
    DMA1_CH2 =1,
    DMA1_CH3 =2,
    DMA1_CH4 =3,
    DMA1_CH5 =4,
    DMA1_CH6 =5,
    DMA1_CH7 =6,
    DMA2_CH1 =7,
    DMA2_CH2 =8,
    DMA2_CH3 =9,
    DMA2_CH4 =10,
    DMA2_CH5 =11,
    DMA2_CH6 =12,
    DMA2_CH7 =13,
}
DMA_Stram_t;



typedef enum
{
  PTOM =  DMA_DIR_PeripheralSRC,
  MTOP =  DMA_DIR_PeripheralDST,
} DMA_Derection_t;

typedef enum
{
  DMA_BYTE,
  DMA_HWORD,
  DMA_WORD,
} DMA_Size_t;


typedef enum
{
    DMA_CH_0,  /*!< Select DMA channel 0 */
    DMA_CH_1,  /*!< Select DMA channel 1 */
    DMA_CH_2,  /*!< Select DMA channel 2 */
    DMA_CH_3,  /*!< Select DMA channel 3 */
    DMA_CH_4,  /*!< Select DMA channel 4 */
    DMA_CH_5,  /*!< Select DMA channel 5 */
    DMA_CH_6,  /*!< Select DMA channel 6 */
    DMA_CH_7   /*!< Select DMA channel 7 */
} DMA_CHANNEL_t;

typedef enum
{
    DMA_Circular  = DMA_Mode_Circular,
    DMA_Normal    = DMA_Mode_Normal,
} DMA_MODE_t;

typedef enum
{
    dma_VeryHigh =  DMA_Priority_VeryHigh,
    dma_High     =  DMA_Priority_High,
    dma_Medium   =  DMA_Priority_Medium,
    dma_Low      =  DMA_Priority_Low,

} DMA_Prioroty_t;

typedef struct
{
    DMA_Stram_t stream;
    DMA_Derection_t direction;
    DMA_Prioroty_t prioroty;
    DMA_Size_t dma_size;
    DMA_MODE_t mode;
    uint32_t paddr;
    uint32_t memadr;
    uint16_t bufsize;

} DMA_INIT_t;


extern DMA_Channel_TypeDef * DMACH[];

typedef struct
{
    void (* CallBack)( void );

} DMA_CFG_t;

void HAL_DMAInitIT(  DMA_INIT_t Init, uint8_t prior, uint8_t subprior, void (*f)(void));
void HAL_DMA_SetCouterAndEnable(DMA_Stram_t stream, uint32_t counter );
void HAL_DMA_Start(DMA_Stram_t stream, uint32_t counter, u32 memadr );

void HAL_DMA1InitMemToMemHwordIT(DMA_Stram_t stream ,
                    DMA_Prioroty_t dma_prior,
                    uint8_t prior,
                    uint8_t subprior,
                    void (*f)(void) );
void HAL_DMAMem2MemStart(DMA_Stram_t stream, uint32_t data_size, u32 source, u32 dest );

#define CFGR_CLEAR_Mask          ((uint32_t)0xFFFF800F)
/*
 * §£§Ü§Ý§ð§é§Ú§ä§î §Ó§í§Ò§â§Ó§ß§ß§í§Û §Ü§Ñ§ß§Ñ§Ý DMA
 */
void HAL_DMA_Enable(DMA_Stram_t stream  );
void HAL_DMA_Disable(DMA_Stram_t stream  );
void HAL_DMA_ITENABLE( DMA_Stram_t stream, uint32_t it );
void HAL_DMA_SetCounter( DMA_Stram_t stream, uint32_t counter );

#endif


#endif /* HAL_HAL_DMA_H_ */
