/*
 * menu_data.h
 *
 *  Created on: Jul 16, 2024
 *      Author: i.dymov
 */

#ifndef USER_MENU_DATA_H_
#define USER_MENU_DATA_H_

#include "main.h"
#include "menu.h"

#define MENU_LEVEL1_COUNT      3U

#define LINE1        12U
#define LINE2        30U
#define LINE3        45U
#define LINE4        60U
#define LINE5        60U

#define SCREENS_COUNT 100
#define FMCH_SCREEN_COUNT 16
#define CDV_SCREENS_COUNT 26
#define BP_SCREENS_COUNT 25

void SetSnsorAnalog(  uint8_t state);
extern xScreenType  xScreenVAV[];
extern xScreenType  xScreenCAV[];
extern xScreenType  xScreenDCV[];
extern xScreenType  xDeiceInit[];
extern xScreenType  xScreenFMCH[];
extern xScreenType  xScreenCVD[];
extern xScreenType  xScreenBP[];

#endif /* USER_MENU_DATA_H_ */
