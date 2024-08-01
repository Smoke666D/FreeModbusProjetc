/*
 * menu.h
 *
 *  Created on: Jul 16, 2024
 *      Author: i.dymov
 */

#ifndef USER_MENU_H_
#define USER_MENU_H_

#include "main.h"
#include "lib_menu.h"


void xInfoScreenCallBack ( xScreenSetObject* menu, char key );
void vGetControllerStatus( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );

#endif /* USER_MENU_H_ */
