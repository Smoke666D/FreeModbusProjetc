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

#define MAX_STRING_NUMBER 10

#define TEST_MODE_ID            0
#define DEVICE_TYPE_ID          1
#define PROCESS_STATE_ID        2
#define CURENT_DATE_ADDR        3
#define CURENT_TIME_ADDR        4
#define AC_VOLTAGE_ID           5
#define IP_ADRESS_DATA_ID       6
#define MB_RTU_ADDR_ID          7
#define CONTROL_MODE_ID         8
#define HOURE_COUNTER_ID        9
#define IP_SUBNETMASK_ID        10
#define VOLTAGE_MIN_OFF_ID      11
#define VOLTAGE_MIN_ON_ID       12
#define CONTRAST_ID             13
#define MOD_BUS_TIMEOUT_ID      14
#define SENS_COUNT_ID           15
#define KOOFKPS_ID              16
#define PROTOCOL_ID             17
#define MAC_ADRESS_ID           18
#define SENS_1_RAW_ID           19
#define SENS_2_RAW_ID           20
#define COOF_P_ID               21
#define COOF_I_ID               22
#define IP_PORT_ID              23
#define IP_GATE_ID              24
#define VOLTAGE_MAX_ON_ID       25
#define ALARM_COUNT_ID          26
#define CURRENT_ALARM_COUNT_ID  27
#define ZERO_CALIBRATE_ID       28
#define DEVICE_RESET_ID         29
#define VOLTAGE_MAX_OFF_ID      30
#define FACT_RASH_ID            31   //§¯§¡§¹§¡§­§°
#define SETTING_ID              32
#define FILTER_LOW_ID           33
#define FILTER_HIGH_ID          34
#define SETTING1_ID             35
#define SETTING2_ID             36
#define CUR_TEMP_ID             37
#define JOURNAL_COUNT_ID        38
#define JURNAL_RECORD_ID        39
#define JOURNAL_TIME_ID         40
#define JOURNAL_DATE_ID         41
#define JOURNAL_INFO1_ID        42
#define JOURNAL_INFO2_ID        43
#define JOURNAL_RESET_ID        44
#define FAN_START_TIMEOUT_ID    45
#define FILTER_STATE_ID         46
#define SENS_FILTER_ID          47
#define MODE_STATE_ID           48   //§¬§°§¯§¦§¸




#define DCV_SETTING1_ID         ( MODE_STATE_ID           +1 )   //0
#define DCV_FACT1_ID            ( DCV_SETTING1_ID       + 1 )    //1
#define DCV_SETTING2_ID         ( DCV_FACT1_ID          + 1 )    //2
#define DCV_FACT2_ID            ( DCV_SETTING2_ID       + 1 )    //3
#define TEMPERATURE_AIT_ID      ( DCV_FACT2_ID          + 1 )    //4
#define TEMPERATURE_ID          ( TEMPERATURE_AIT_ID    + 1 )    //5
#define CO2_ID                  ( TEMPERATURE_ID        + 1 )    //6
#define CDV_HUMMANITY_ID        ( CO2_ID                + 1 )    //7
#define SENSOR_ID               ( CDV_HUMMANITY_ID      + 1 )    //8
#define SETTING_MIN_ID          ( SENSOR_ID             + 1 )    //9
#define SETTING_AVER_ID         ( SETTING_MIN_ID        + 1 )    //10
#define SETTING_MAX_ID          ( SETTING_AVER_ID       + 1 )    //11
#define FAIL_SET_CH1_ID         ( SETTING_MAX_ID        + 1 )    //12
#define FAIL_SET_CH2_ID         ( FAIL_SET_CH1_ID       + 1 )    //13
#define F_CHANNEL_ID            ( FAIL_SET_CH2_ID       + 1 )    //14
#define SENSOR_TYPE_ID          ( F_CHANNEL_ID          + 1 )    //15
#define SENSOR_MIN_ID           ( SENSOR_TYPE_ID      + 1 )      //16
#define SENSOR_MAX_ID           ( SENSOR_MIN_ID       + 1 )    //17
#define SENSOR_OFFSET_ID        ( SENSOR_MAX_ID       + 1 )    //18
#define SENSOR_SETTING_ID       ( SENSOR_OFFSET_ID    + 1 )    //19
#define AFTER_ZONE_SETTING_ID   ( SENSOR_SETTING_ID   + 1 )    //20
#define CDV_CH_COUNT_ID         ( AFTER_ZONE_SETTING_ID + 1 )    //21
#define MEASERING_UNIT_ID       ( CDV_CH_COUNT_ID       + 1 )    //22
#define CLEAN_TIMER_ID          ( MEASERING_UNIT_ID     + 1 )    //23
#define ZERO_POINT_TIMEOUT_ID   ( CLEAN_TIMER_ID        + 1 )    //24
#define SETTING_TIMER_ID        ( ZERO_POINT_TIMEOUT_ID + 1 )    //25
#define PRIOR_SENSOR_ID         ( SETTING_TIMER_ID       + 1 )    //26
#define INPUT_SENSOR_TYPE_ID    ( PRIOR_SENSOR_ID       + 1 )    //27
#define OFFSET2_ID              (INPUT_SENSOR_TYPE_ID      + 1 )    //28
#define COOF_P_1_ID             ( OFFSET2_ID            + 1 )    //29
#define COOF_I_1_ID             ( COOF_P_1_ID           + 1 )    //30
#define BP_SETTING1_ID          ( COOF_I_1_ID           + 1 )    //31
#define BP_FACT_ID              ( BP_SETTING1_ID        + 1 )    //32
#define BP_SIZE_ID              ( BP_FACT_ID            + 1 )    //33
#define BP_REGULATION_TYPE_ID  ( BP_SIZE_ID            + 1 )    //34
#define CH1_ERROR_ID            ( BP_REGULATION_TYPE_ID + 1 )    //35
#define CH2_ERROR_ID            ( CH1_ERROR_ID          + 1 )    //36
#define SETTING_BP_MIN_ID       ( CH2_ERROR_ID          + 1 )    //37
#define SETTING_BP_AVER_ID      ( SETTING_BP_MIN_ID     + 1 )    //38
#define SETTING_BP_MAX_ID       ( SETTING_BP_AVER_ID    + 1 )    //39
#define CDV_MODE_ID             (SETTING_BP_MAX_ID      + 1 )    //40
#define BP_REG_TYPE_ID          ( CDV_MODE_ID      + 1 )          //41
#define BP_SZIE_ID              ( BP_REG_TYPE_ID      + 1 )        //42
#define DRAW_UNIT_ID            ( BP_SZIE_ID      + 1 )           //43
#define ZERO_CAL_COMMAND        ( DRAW_UNIT_ID      + 1 )        //44



#define VOLTAG_SCREEN_TITLE_ID  ( ZERO_CAL_COMMAND       + 1 )
#define SETTING1_TITLE_ID       ( VOLTAG_SCREEN_TITLE_ID + 1 )
#define SETTING2_TITLE_ID       ( SETTING1_TITLE_ID      + 1 )
#define SETTING3_TITLE_ID       ( SETTING2_TITLE_ID      + 1 )
#define SETTING6_TITLE_ID       ( SETTING3_TITLE_ID      + 1 )
#define SETTING7_TITLE_ID       ( SETTING6_TITLE_ID      + 1 )
#define SETTING8_TITLE_ID       ( SETTING7_TITLE_ID      + 1 )
#define SETTING9_TITLE_ID       ( SETTING8_TITLE_ID      + 1 )
#define SETTING10_TITLE_ID      ( SETTING9_TITLE_ID      + 1 )
#define SETTING11_TITLE_ID      ( SETTING10_TITLE_ID      + 1 )
#define SETTINGANALOG1_TITLE_ID ( SETTING11_TITLE_ID      + 1 )
#define SETTINGANALOG2_TITLE_ID ( SETTINGANALOG1_TITLE_ID      + 1 )
#define SENSOR_TYPE_TITLE_ID    ( SETTINGANALOG2_TITLE_ID      + 1 )
#define RESET_TITLE_ID           ( SENSOR_TYPE_TITLE_ID   + 1 )
#define SENSOR_TITLE_ID         ( RESET_TITLE_ID  + 1 )
#define CALIBRATION_TITLE_ID    ( SENSOR_TITLE_ID   + 1 )



#define TITLE_FIRST         VOLTAG_SCREEN_TITLE_ID
#define TITLE_LAST          CALIBRATION_TITLE_ID

#define FMCH_FIRST          FACT_RASH_ID
#define FMCH_LAST           MODE_STATE_ID

#define CDV_BP_FIRST        DCV_SETTING1_ID
#define CDV_BP_LAST         ZERO_CAL_COMMAND

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
  CMD_READ = 0,
  CMD_EDIT_READ = 1,
  CMD_INC  = 8,
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
#define EXIT_COMMAND         0x6300
#define RESET_COMMNAD        0x8800
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

const char * getModeString(u8 mode);
void MenuSetDeviceMenu();
void MenuSetDevice();
void MENU_DrawString(u8 x, u8 y, char const * str);
void MENU_ClearScreen();
void vMenuInit(  );
void vDrawBitmap();
void vMenuTask ( void );

#endif /* USER_MENU_H_ */
