/*
 * hal_wdt.h
 *
 *  Created on: 26 апр. 2024 г.
 *      Author: i.dymov
 */

#ifndef HAL_HAL_WDT_H_
#define HAL_HAL_WDT_H_

#include "main.h"
#include "hal_config.h"



void HAL_WDTInit();
void HAL_WDTReset();

void HAL_WDT_Init1s();



#endif /* HAL_HAL_WDT_H_ */
