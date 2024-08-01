/*
 * menu_data.h
 *
 *  Created on: Jul 16, 2024
 *      Author: i.dymov
 */

#ifndef USER_MENU_DATA_H_
#define USER_MENU_DATA_H_

#include "main.h"
#include "lib_menu.h"

#define MENU_LEVEL1_COUNT      3U

#define LINE1        12U
#define LINE2        30U
#define LINE3        45U
#define LINE4        60U
#define LINE5        60U

enum
{
  SETTING_1 = 0x00,
  FACT1     = 0x01,
  SETTING2  = 0x02,
  FACT2     = 0x03,

} xParametrName_t;

extern xScreenSetObject xMainMenu;

#endif /* USER_MENU_DATA_H_ */
