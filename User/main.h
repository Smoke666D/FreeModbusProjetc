/*
 * main.h
 *
 *  Created on: Feb 5, 2024
 *      Author: i.dymov
 */

#ifndef USER_MAIN_H_
#define USER_MAIN_H_

#include "string.h"
#include "eth_driver.h"
#include "debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "wchnet.h"
#include "port.h"
#include "mb.h"
#include "event_groups.h"
#include "ADC.H"

EventGroupHandle_t xGetADCEvent();

#endif /* USER_MAIN_H_ */
