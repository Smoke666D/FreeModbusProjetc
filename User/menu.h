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


#define ENTER_KEY  4
#define EXIT_KEY   5
#define DOWN_KEY   2
#define UP_KEY     3
#define RIGTH_KEY  1
#define LEFT_KEY   0

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
#define CONTRAST_ID             41
#define FAN_START_TIMEOUT_ID    42
#define ALARM_COUNT_ID          43
#define CURRENT_ALARM_COUNT_ID  44
#define ZERO_CALIBRATE_ID       45

#define CVD_SETTING1_ID         46
#define CVD_FACT1_ID            47
#define CVD_SETTING2_ID         48
#define CVD_FACT2_ID            49
#define TEMPERATURE_AIT_ID      50
#define TEMPERATURE_ID          51
#define CO2_ID                  52
#define CDV_HUMMANITY_ID        53
#define SENSOR_ID               54
#define SETTING_MIN_ID          54
#define SETTING_AVER_ID         55
#define SETTING_MAX_ID          56
#define FAIL_SET_CH1_ID         57
#define FAIL_SET_CH2_ID         58
#define F_CHANNEL_ID            59
#define CLEAN_TIMER_ID          60
#define ZERO_POINT_ID           61
#define SENSOR_TYPE_ID          62
#define SENSOR_F_TYPE_ID        63
#define KK_SENSOR_TYPE_ID       64
#define CO2_SENSOR_TYPE_ID      65
#define H_SENSOR_TYPE_ID        66
#define  PRIOR_SENSOR_ID        67
#define T_SENSOR_MIN_ID         68
#define T_SENSOR_MAX_ID         69
#define CO2_SENSOR_MIN_ID       70
#define CO2_SENSOR_MAX_ID       71
#define H_SENSOR_MIN_ID         72
#define H_SENSOR_MAX_ID         73
#define T_SENSOR_OFFSET_ID      74
#define CO2_SENSOR_OFFSET_ID    75
#define H_SENSOR_OFFSET_ID      76
#define T_SENSOR_SETTING_ID     77
#define CO2_SENSOR_SETTING_ID   78
#define H_SENSOR_SETTING_ID     79
#define AFTER_ZONE_SETTING_ID   80
#define CDV_CH_COUNT_ID         81
#define MEASERING_UNIT_ID       82
#define OFFSET2_ID              83
#define COOF_P_1_ID             84
#define COOF_I_1_ID             85
#define COOF_P_2_ID             86
#define COOF_I_2_ID             87
#define COOF_P_3_ID             88
#define COOF_I_3_ID             89
#define BP_SETTING1_ID          90
#define BP_FACT_ID              91
#define BP_SIZE_ID              92
#define  BP_REGULATION_TYPE_ID  93
#define CH1_ERROR_ID            94
#define CH2_ERROR_ID            95
#define SETTING_BP_MIN_ID       96
#define SETTING_BP_AVER_ID      97
#define SETTING_BP_MAX_ID       98



#define VOLTAG_SCREEN_TITLE_ID  101
#define SETTING1_TITLE_ID       102
#define SETTING2_TITLE_ID       103
#define CALIBRATION_TITLE_ID    104

#define TITLE_FIRST         VOLTAG_SCREEN_TITLE_ID
#define TITLE_LAST          CALIBRATION_TITLE_ID

#define CDV_BP_FIRST        AFTER_ZONE_SETTING_ID
#define CDV_BP_LAST         CDV_CH_COUNT_ID

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


void MenuSetDevice();
void MENU_DrawString(u8 x, u8 y, char const * str);
void MENU_ClearScreen();
void vMenuInit(  );
void vDrawBitmap();
void vMenuTask ( void );

#endif /* USER_MENU_H_ */
