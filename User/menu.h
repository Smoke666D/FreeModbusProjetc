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

#define PROCESS_STATE_ID        1
#define CURENT_DATE_ADDR        2
#define CURENT_TIME_ADDR        3
#define AC_VOLTAGE_ID           4
#define IP_ADRESS_DATA_ID       5
#define MB_RTU_ADDR_ID          6
#define CONTROL_MODE_ID         7
#define SETTING_ID              8
#define FACT_RASH_ID            9
#define FILTER_STATE_ID         10
#define MODE_STATE_ID           11
#define VOLTAGE_MIN_ON_ID       12
#define VOLTAGE_MIN_OFF_ID      13
#define VOLTAGE_MAX_ON_ID       14
#define VOLTAGE_MAX_OFF_ID      15
#define PROTOCOL_ID             16
#define MAC_ADRESS_ID           17
#define SENS_1_RAW_ID           18
#define SENS_2_RAW_ID           19
#define COOF_P_ID               20
#define COOF_I_ID               21
#define IP_PORT_ID              22
#define IP_GATE_ID              23
#define IP_SUBNETMASK_ID        24
#define HOURE_COUNTER_ID        26


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

typedef enum
{
  CMD_EDIT_READ = 8,
  CMD_READ = 0,
  CMD_INC  = 1,
  CMD_DEC  = 2,
  CMD_START_EDIT = 3,
  CMD_NEXT_EDIT =4,
  CMD_EXIT_EDIT =5,
  CMD_SAVE_EDIT =6,
  CMD_PREV_EDIT = 7,
} DATA_VIEW_COMMAND_t;

#define SAVE_COMMAND   0x60
#define ENTER_COMMNAD  0x80
#define COMMNAD_MASK   0xC0

#define MAX_MENU_COUNT 0x3F

typedef struct
{
  u8 ScreenId;
  const xScreenObjet*  pScreenCurObjets;
  u8          pLeftScreen;
  u8          pRigthScreen;
  u8          pUpScreenSet;
  u8          pDownScreenSet;
  u8          pEnter;
  u8          pBack;
 // void          ( *pFunc )( void*, char );
} xScreenType;


void vMenuInit(  );
void vDrawBitmap();
void vMenuTask ( void );

#endif /* USER_MENU_H_ */
