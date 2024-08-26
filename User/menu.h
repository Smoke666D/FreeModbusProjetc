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
#define FILTER_LOW_ID           27
#define FILTER_HIGH_ID          28
#define SETTING1_ID             29
#define SETTING2_ID             30
#define KOOFKPS_ID              31
#define JOURNAL_COUNT_ID        32
#define JURNAL_RECORD_ID        33
#define JOURNAL_TIME_ID         34
#define JOURNAL_DATE_ID         35
#define JOURNAL_INFO1_ID        36
#define JOURNAL_INFO2_ID        37
#define JOURNAL_RESET_ID        38
#define SENS_COUNT_ID           39
#define MOD_BUS_TIMEOUT_ID      40


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

#define JOURNAL_VIEW_COMMAND 0x8100
#define JOURNAL_EXIT         0x8200
#define JOURNAL_NEXT         0x8300
#define JOURNAL_PREV         0x8400
#define SAVE_COMMAND         0x6000
#define ENTER_COMMNAD        0x8000
#define COMMNAD_MASK         0xFF00

#define MAX_MENU_COUNT 0xFF

typedef struct
{
  u8 ScreenId;
  const xScreenObjet*  pScreenCurObjets;
  u16          pLeftScreen;
  u16          pRigthScreen;
  u16          pUpScreenSet;
  u16          pDownScreenSet;
  u16          pEnter;
  u16          pBack;
 // void          ( *pFunc )( void*, char );
} xScreenType;


void vMenuInit(  );
void vDrawBitmap();
void vMenuTask ( void );

#endif /* USER_MENU_H_ */
