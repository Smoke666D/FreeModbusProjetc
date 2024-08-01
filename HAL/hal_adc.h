/*
 * hal_adc.h
 *
 *  Created on: 11 閭攲瑜�. 2024 璋�.
 *      Author: i.dymov
 */

#ifndef HAL_HAL_ADC_H_
#define HAL_HAL_ADC_H_


#include "main.h"
#include "hal_config.h"
#include "hal_dma.h"

typedef enum
{
    ADC_1 = 0,
    ADC_2 = 1,
} ADC_NUMBER_t;

typedef enum
{
    ADC_CH_0 = 0,  /*!< ADC Channel 0 */
    ADC_CH_1 = 1,  /*!< ADC Channel 1 */
    ADC_CH_2 = 2,  /*!< ADC Channel 2 */
    ADC_CH_3 = 3,  /*!< ADC Channel 3 */
    ADC_CH_4 = 4,  /*!< ADC Channel 4 */
    ADC_CH_5 = 5,  /*!< ADC Channel 5 */
    ADC_CH_6 = 6,  /*!< ADC Channel 6 */
    ADC_CH_7 = 7,  /*!< ADC Channel 7 */
    ADC_CH_8 = 8,  /*!< ADC Channel 8 */
    ADC_CH_9 = 9,  /*!< ADC Channel 9 */
    ADC_CH_10 = 10, /*!< ADC Channel 10 */
    ADC_CH_11 = 11, /*!< ADC Channel 11 */
    ADC_CH_12 = 12, /*!< ADC Channel 12 */
    ADC_CH_13 = 13, /*!< ADC Channel 13 */
    ADC_CH_14 = 14, /*!< ADC Channel 14 */
    ADC_CH_15 = 15, /*!< ADC Channel 15 */
    ADC_CH_16 = 16, /*!< ADC Channel 16 */
    ADC_CH_17 = 17, /*!< ADC Channel 17 */
    ADC_CH_18 = 18, /*!< ADC Channel 18 */
} ADC_CH_T;


typedef struct
{
    void (* CallBack)( void );

} ADC_CF_t;


typedef struct
{
  void (*awdt_callback)(void);

} ADC_t;

#if MCU == APM32
    #define ADC_NUMBER_t ADC_T*
    #define ADC_1  ADC1
    #define ADC_2  ADC2
    #define ADC_3  ADC3
#endif
#if MCU == CH32V2 || MCU == CH32V3


uint16_t HAL_ADC_DATA(ADC_NUMBER_t adc );

u16 Get_ConversionVal( ADC_NUMBER_t adc , s16 val);
   /* ADC ADON mask */
   #define CTLR2_ADON_Set                   ((uint32_t)0x00000001)
   #define CTLR2_ADON_Reset                 ((uint32_t)0xFFFFFFFE)
   /* ADC DMA mask */
   #define CTLR2_DMA_Set                    ((uint32_t)0x00000100)
   #define CTLR2_DMA_Reset                  ((uint32_t)0xFFFFFEFF)
   /* ADC Software start mask */
   #define CTLR2_EXTTRIG_SWSTART_Set        ((uint32_t)0x00500000)
   #define CTLR2_EXTTRIG_SWSTART_Reset      ((uint32_t)0xFFAFFFFF)
   /* ADC RSTCAL mask */
   #define CTLR2_RSTCAL_Set                 ((uint32_t)0x00000008)
   /* ADC CAL mask */
   #define CTLR2_CAL_Set                    ((uint32_t)0x00000004)
   /* ADC EXTTRIG mask */
   #define CTLR2_EXTTRIG_Set                ((uint32_t)0x00100000)
   #define CTLR2_EXTTRIG_Reset              ((uint32_t)0xFFEFFFFF)
#endif

void HAL_ADC_ContiniusScanTrigCinvertionDMA( ADC_NUMBER_t adc, uint8_t channel_count, uint8_t *  channel_nmber, u32 ext_trig);
void HAL_ADC_InitIT( ADC_NUMBER_t adc, u32 ext_trig, u8 prior, u8 subprior, void (*f)(void) );
void HAL_ADC_ContiniusScanCinvertionDMA( ADC_NUMBER_t adc, uint8_t channel_count, uint8_t * channel_nmber);

#if MCU == CH32V2
void HAL_ADC_StartDMA( DMA_Stram_t chanel,  uint16_t size);
#endif
#if MCU == APM32
void HAL_ADC_CommonConfig();
void HAL_ADC_Enable(ADC_NUMBER_t adc_number);
void HAL_ADCDMA_Disable(ADC_NUMBER_t adc_number);
void HAL_ADC_TempEnable();
void HAL_ADC_VrefEnable();
void HAL_ADC_StartDMA( DMA_Stram_t chanel, uint16_t * data, uint16_t size);
#endif
#endif /* HAL_HAL_ADC_H_ */
