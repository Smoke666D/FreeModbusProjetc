/*
 * menu.h
 *
 *  Created on: Jul 16, 2024
 *      Author: i.dymov
 */

#ifndef USER_MENU_H_
#define USER_MENU_H_

#include "main.h"
#include "queue.h"

#define MAX_STRING_NUMBER 5

#define CURENT_DATE_ADDR   2
#define CURENT_TIME_ADDR   3
#define AC_VOLTAGE_ID      4
#define IP_ADRESS_DATA_ID  5

typedef enum
{
  TEXT_STRING = 0,
  READ_DATA = 1,
  WRITE_DATA = 2,
} OBJECT_TYPE;

typedef struct
{
  uint8_t     last;
  uint8_t     x;
  uint8_t     y;
  uint8_t     x_data;
  OBJECT_TYPE xType;
  char*       pStringParametr;
  uint16_t    DataID;
} xScreenObjet;




typedef struct
{
  u8 ScreenId;
  const xScreenObjet*  pScreenCurObjets;
  u8          pLeftScreen;
  u8          pRigthScreen;
  u8          pUpScreenSet;
  u8          pDownScreenSet;
 // void          ( *pFunc )( void*, char );
} xScreenType;


void vMenuInit(  );

void vMenuTask ( void );

#endif /* USER_MENU_H_ */
