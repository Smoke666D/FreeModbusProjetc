/*
 * menu.c
 *
 *  Created on: Jul 16, 2024
 *      Author: i.dymov
 */
#include "menu.h"
#include "queue.h"


static xScreenSetObject* pCurrMenu        = NULL;
static QueueHandle_t     pKeyboard        = NULL;



void xInfoScreenCallBack ( xScreenSetObject* menu, char key )
{

}


void vGetControllerStatus( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{

}

void vMenuTask ( void )
{




}
