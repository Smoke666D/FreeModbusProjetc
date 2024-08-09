/*
 * hal_dac.h
 *
 *  Created on: Aug 8, 2024
 *      Author: i.dymov
 */

#ifndef HAL_HAL_DAC_H_
#define HAL_HAL_DAC_H_

#include "main.h"

typedef enum
{
    HAL_DAC1 = 0,
    HAL_DAC2 = 1,
} HAL_DAC_t;


typedef struct
{
  HAL_DAC_t channel;
  uint32_t DAC_Trigger;
  uint32_t DAC_WaveGeneration;
  uint32_t DAC_LFSRUnmask_TriangleAmplitude;
  uint32_t DAC_OutputBuffer;
}HAL_DAC_InitTypeDef;


#endif /* HAL_HAL_DAC_H_ */
