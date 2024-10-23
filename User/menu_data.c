/*
 * menu_data.c
 *
 *  Created on: Jul 16, 2024
 *      Author: i.dymov
 */



#include "menu_data.h"
#include "menu.h"
#include "data_model.h"

static const char SETTING_TEXT[] = "Уставка";
static const char VOLTAGE_TEXT[] = "Напряжение";
static const char KOOFKPS_TEXT[] = "Кооэ к.п.с";

static  xScreenObjet const InfoScreen1[]=
{
        {0,0,LINE1,0,READ_DATA,"Состояние",PROCESS_STATE_ID},
        {0,0,25,0,READ_DATA,(char*)SETTING_TEXT,SETTING_ID },
        {0,0,37,0,READ_DATA,"Факт. расх.",FACT_RASH_ID},
        {0,0,50,0,READ_DATA,"Фильтр",FILTER_STATE_ID },
        {0,0,50,40,READ_DATA,"",SENS_FILTER_ID},
        {1,0,62,0,READ_DATA,"Режим",MODE_STATE_ID },
};

static xScreenObjet const InfoScreen2[]=
{
        {0,2,LINE1,0,READ_DATA,"Текущих аварий:",ALARM_COUNT_ID},
        {0,2,25,0,READ_DATA,"",CURRENT_ALARM_COUNT_ID},
        {0,2,50,0, TEXT_STRING,     "Журнaл аварий",0},
        {1,2,62,0,READ_DATA,"Записей:",JOURNAL_COUNT_ID},
};
static xScreenObjet const InfoScreen3[]=
{
        {0,2,LINE1,0,READ_DATA,"Дата",CURENT_DATE_ADDR},
        {0,2,25,0,READ_DATA,"Время",CURENT_TIME_ADDR},
        {0,2,37,0,READ_DATA,"Режим управл.",CONTROL_MODE_ID },
        {0,2,50,0,READ_DATA,"Адрес ModBus",MB_RTU_ADDR_ID},
        {1,2,62,0,READ_DATA,"",IP_ADRESS_DATA_ID },
};

static xScreenObjet const InfoScreen4[]=
{
        {0,2,LINE1,00,READ_DATA,"Темп. воздуха",CUR_TEMP_ID},
        {0,2,25,00,READ_DATA,"MAC",MAC_ADRESS_ID},
        {0,2,37,00,READ_DATA,(char*)VOLTAGE_TEXT,AC_VOLTAGE_ID },
        {0,2,50,00,TEXT_STRING,"Моточасы",0},
        {1,2,62,05,READ_DATA,"",HOURE_COUNTER_ID },
};

static xScreenObjet const SettingsScreen1[]=
{
        {0,10,LINE1,0,READ_DATA, "Настройки",          SETTING1_TITLE_ID },
        {0,2,25,0,WRITE_DATA,"Режим управл.",          CONTROL_MODE_ID },
        {0,2,37,0,WRITE_DATA,"Протокол связи.",        PROTOCOL_ID },
        {0,2,50,0,WRITE_DATA,"Адрес ModBus",           MB_RTU_ADDR_ID},
        {0,2,62,10,WRITE_DATA,"Таймаут связи",       MOD_BUS_TIMEOUT_ID},
        {1,120,62,0,TEXT_STRING,"c.",           0},
};

static xScreenObjet const SettingsScreen2[]=
{
        {0,10,LINE1,0,READ_DATA, "Настройки      ",    SETTING2_TITLE_ID },
        {0,2,25,0,WRITE_DATA,"IP",                     IP_ADRESS_DATA_ID},
        {0,2,37,0,WRITE_DATA,"IP Порт",                IP_PORT_ID},
        {0,2,50,0,WRITE_DATA,"Шлюз",                   IP_GATE_ID},
        {1,2,62,0,WRITE_DATA,"Маска",                  IP_SUBNETMASK_ID },
};

static xScreenObjet const SettingsScreen3[]=
{
        {0,15,LINE1,0,TEXT_STRING,"Настройки      3/10",    0},
        {0,2,25,0,TEXT_STRING,"Уставки реж.  м^3/ч",    0},
        {0,2,37,0,WRITE_DATA,"Режим 1 (Осн.)",                SETTING1_ID},
        {0,2,50,0,WRITE_DATA,"Режим 2 (Доп.)",                SETTING2_ID},
        {1,2,62,0,WRITE_DATA,(char*)KOOFKPS_TEXT,             KOOFKPS_ID},

};
static xScreenObjet const SettingsScreen4[]=
{
        {0,15,LINE1,0,TEXT_STRING,"Настройки      4/10",0},
        {0,2,25,0,TEXT_STRING,"Нач. сопр. фильтра",0},
        {0,2,37,20,WRITE_DATA,"",                       FILTER_LOW_ID},
        {0,115,37,0,TEXT_STRING,"Па",0},
        {0,2,50,0,TEXT_STRING,"Конеч. сопр. фильтра",0},
        {0,115,62,0,TEXT_STRING,"Па",0},
        {1,2,62,20,WRITE_DATA,"",                       FILTER_HIGH_ID },
};
static xScreenObjet const SettingsScreen5[]=
{
        {0,10,LINE1,0,READ_DATA,"Настройки      ",VOLTAG_SCREEN_TITLE_ID},
        {0,2,25,0,WRITE_DATA,"Напряжение min",     VOLTAGE_MIN_ON_ID},
        {0,2,37,0,WRITE_DATA,"Сброс аварии min",   VOLTAGE_MIN_OFF_ID },
        {0,2,50,0,WRITE_DATA,"Напряжение max",     VOLTAGE_MAX_ON_ID},
        {1,2,62,0,WRITE_DATA,"Cброс авари max",    VOLTAGE_MAX_OFF_ID },
};
static xScreenObjet const SettingsScreen6[]=
{
        {0,15,LINE1,0,TEXT_STRING,"Настройки      6/10",0},
        {0,2,25,0,TEXT_STRING,"ПИ регулятор",0},
        {0,2,37,0,WRITE_DATA,"Коэф. П",                COOF_P_ID },
        {0,2,50,0,WRITE_DATA,"Коэф. И",                COOF_I_ID },
        {1,2,62,0,WRITE_DATA,"Время усред. с",         SENS_COUNT_ID },

};
static xScreenObjet const SettingsScreen7[]=
{
        {0,15,LINE1,0,READ_DATA,"Настройки",  CALIBRATION_TITLE_ID},
        {0,2,25,0,TEXT_STRING,"Текущий перепад давл.",  0},
        {0,2,37,0,READ_DATA,"Вентилятор",               SENS_1_RAW_ID},
        {0,2,50,0,READ_DATA,"Фильтр",                   SENS_2_RAW_ID},
        {1,2,62,0,WRITE_DATA,"Откалибравать 0",         ZERO_CALIBRATE_ID },
};

static xScreenObjet const SettingsScreen8[]=
{
        {0,15,LINE1,0,TEXT_STRING,"Настройки      8/10",    0},
        {0,2,25,0,WRITE_DATA,"Контрастность", CONTRAST_ID},
        {0,2,37,0,WRITE_DATA,"Ост. вент с.", FAN_START_TIMEOUT_ID},
        {0,2,50,0,WRITE_DATA,"Дата:",       CURENT_DATE_ADDR  },
        {1,2,62,0,WRITE_DATA,"Время",       CURENT_TIME_ADDR  },

};


static xScreenObjet const SettingsScreen9[]=
{
        {0,15,LINE1,0,TEXT_STRING,"Настройки      9/10",    0},
        {0,2,25,0,TEXT_STRING,"  Для сброса журнала",  0},
        {0,2,37,0,TEXT_STRING,"      нажмите ВВОД",        0},
        {0,2,50,0,READ_DATA,"Записей:",JOURNAL_COUNT_ID},
        {1,2,62,0,WRITE_DATA,"",         JOURNAL_RESET_ID },
};


static xScreenObjet  ResetScreen[]=
{
        {0,15,LINE1,0,READ_DATA,"Настройки",  RESET_TITLE_ID},
        {0,2,25,0,TEXT_STRING,"  Для перезагрузки ",  0},
        {0,2,37,0,TEXT_STRING,"    нажмите ВВОД",        0},
        {1,2,62,0,WRITE_DATA,"",         DEVICE_RESET_ID },
};



static xScreenObjet const JournalViewScreen[]=
{
        {0,2,LINE1,0,READ_DATA,"Запись журнала:",JURNAL_RECORD_ID },
        {0,2,25,0,READ_DATA,"Дата:", JOURNAL_DATE_ID },
        {0,2,37,0,READ_DATA,"Время:",               JOURNAL_TIME_ID },
        {0,2,50,0,READ_DATA,"",        JOURNAL_INFO1_ID},
        {1,2,62,0,READ_DATA,"",        JOURNAL_INFO2_ID},
};

xScreenType  xScreenFMCH[FMCH_SCREEN_COUNT] =
{
  {1,InfoScreen1,      3,   2,  11,  11, 0, 0 },
  {2,InfoScreen2,      1,   3,  0,  0 , 12 | JOURNAL_VIEW_COMMAND, 1 },
  {3,InfoScreen3,      2,   1,  0,   0, 4  , 1  },
  {4,SettingsScreen1,  0,   0,  15,  5, ENTER_COMMNAD , 3  },
  {5,SettingsScreen2,  0,   0,  4,   6, ENTER_COMMNAD , 3 },
  {6,SettingsScreen3,  0,   0,  5,  7,  ENTER_COMMNAD , 3 },
  {7,SettingsScreen4,  0,   0,  6,  8,  ENTER_COMMNAD , 3  },
  {8,SettingsScreen5,  0,   0,  7,  9,  ENTER_COMMNAD , 3  },
  {9,SettingsScreen6,  0,   0,  8,  10, ENTER_COMMNAD,  3 },
  {10,SettingsScreen7, 0,   0,  9,  13, ENTER_COMMNAD,  3 },
  {11,InfoScreen4,      0,   0,  1,  1, 0, 3 },
  {12,JournalViewScreen, 0,   0, JOURNAL_PREV , JOURNAL_NEXT, 0,  2 },
  {13,SettingsScreen8, 0,   0,  10,   14,   ENTER_COMMNAD, 3 },
  {14,SettingsScreen9, 0,   0,  13,   15,   ENTER_COMMNAD, 3 },
  {15, ResetScreen    ,0,   0,  14,    4,   ENTER_COMMNAD ,3 },
};


/*
 *   Режим СDV
 */

static  xScreenObjet const CDVInfoScreen1[]=
{
        {0,0,LINE1,32, READ_DATA,"Уставка 1",  DCV_SETTING1_ID},
        {0,100,LINE1,0, READ_DATA,"",          DRAW_UNIT_ID},
        {0,0,25,32,    READ_DATA,"Факт 1",     DCV_FACT1_ID },
        {0,100,25,0,    READ_DATA,"",          MEASERING_UNIT_ID},
        {0,0,37,32,     READ_DATA,"Уставка 2", DCV_SETTING2_ID},
        {0,100,37,0,    READ_DATA,"",          MEASERING_UNIT_ID},
        {0,0,50,32,     READ_DATA,"Факт 2",    DCV_FACT2_ID },
        {0,100,50,0,    READ_DATA,"",          MEASERING_UNIT_ID},
        {1,10,62,0,     READ_DATA,"Режим:",    CDV_MODE_ID},
};

static  xScreenObjet const CDVInfoScreen1_1[]=
{
        {0,0,LINE1,32, READ_DATA,"Уставка 1",  DCV_SETTING1_ID},
        {0,100,LINE1,0, READ_DATA,"",          MEASERING_UNIT_ID},
        {0,0,25,32,    READ_DATA,"Факт 1",     DCV_FACT1_ID },
        {0,100,25,0,    READ_DATA,"",          MEASERING_UNIT_ID},
        {0,0,37,0,     READ_DATA,"Типоразмер ВР", BP_SZIE_ID},
        {0,0,50,0,     READ_DATA,"Тип регул.", BP_REG_TYPE_ID },
        {1,10,62,0,     READ_DATA,"Режим:",    CDV_MODE_ID},
};


static xScreenObjet  CDVSettingsScreen3[]=
{
        {0,15,LINE1,0,READ_DATA,"Настройки      ",       SETTING3_TITLE_ID },
        {0,2,25,0,TEXT_STRING,"Еденицы измерения",          0},
        {0,2,37,0,WRITE_DATA,"",                            MEASERING_UNIT_ID},
        {0,2,50,0,WRITE_DATA,"Кол-во каналов",              CDV_CH_COUNT_ID},
        {1,2,62,0,WRITE_DATA,"Вход:",                       INPUT_SIGNAL_MODE_ID},

};

static xScreenObjet  CDVSettingsScreen4[]=
{
        {0,15,LINE1,0,READ_DATA,"Настройки      ",   SETTING6_TITLE_ID},
        {0,2,25,0,TEXT_STRING,"Уставки",          0},
        {0,2,37,30,WRITE_DATA,"Мин.",               SETTING_MIN_ID},
        {0,100,37,0,READ_DATA,"",                  MEASERING_UNIT_ID},
        {0,2,50,30,WRITE_DATA,"Макс.",              SETTING_MAX_ID},
        {0,100,50,0,READ_DATA,"",                  MEASERING_UNIT_ID},
        {0,100,62,0,READ_DATA,"",                  MEASERING_UNIT_ID},
        {1,2,62,30,WRITE_DATA,"Сред.",              SETTING_AVER_ID},
};


static xScreenObjet  CDVSettingsScreen5[]=
{
        {0,15,LINE1,0,READ_DATA,"Настройки      ",   SETTING7_TITLE_ID},
        {0,2,25,0,TEXT_STRING,"Аваринайя уставка",          0},
        {0,100,37,0,READ_DATA,"",                  MEASERING_UNIT_ID},
        {0,2,37,30,WRITE_DATA,"Канал 1",              FAIL_SET_CH1_ID},
        {0,100,50,0,READ_DATA,"",                  MEASERING_UNIT_ID},
        {0,2,50,30,WRITE_DATA,"Канал 2",              FAIL_SET_CH2_ID},
        {0,2,62,30,WRITE_DATA,"Смещ 2",                      OFFSET2_ID},
        {1,100,62,0,READ_DATA,"",                           MEASERING_UNIT_ID},
};

static xScreenObjet  CDVSettingsScreen6[]=
{
        {0,15,LINE1,0,READ_DATA,"Настройки      ",    SETTING8_TITLE_ID},
        {0,2,25,0,TEXT_STRING,"Расчет потока",          0},
        {0,2,37,0,WRITE_DATA,(char*)KOOFKPS_TEXT,       KOOFKPS_ID},
        {1,2,50,0,WRITE_DATA,"F канала, м^2",           F_CHANNEL_ID},
};



static xScreenObjet  CDVSettingsScreen7[]=
{
        {0,15,LINE1,00,READ_DATA,"Настройки      ",  SETTING9_TITLE_ID},
        {0,2,25,0,TEXT_STRING,"Таймеры",          0},
        {0,2,37,0,WRITE_DATA,"Уборка (DI4) мин",                CLEAN_TIMER_ID},
        {0,2,50,00,WRITE_DATA,"Калибр. 0, с.:",          ZERO_POINT_TIMEOUT_ID},
        {1,2,62,00,WRITE_DATA,"Задержка уст. с.",          SETTING_TIMER_ID},
};





static xScreenObjet const CDVSettingsAnalogScreen2[]=
{
        {0,10,LINE1,0,READ_DATA,"Настройки     ",  SETTINGANALOG2_TITLE_ID},
        {0,2,25,0,TEXT_STRING,"Диапазон датчика T",          0},
        {0,2,37,70,WRITE_DATA,"от", SENSOR1_MIN_ID},
        {0,70,37,0,WRITE_DATA,"до" , SENSOR1_MAX_ID},
        {0,2,50,0,WRITE_DATA,"Тип",SENSOR1_TYPE_ID},
        {1,2,62,0,WRITE_DATA,"Смещение",SENSOR1_OFFSET_ID},
};


static xScreenObjet const CDVSettingsAnalogScreen3[]=
{
        {0,10,LINE1,0,READ_DATA,"Настройки     ",  SETTINGANALOG3_TITLE_ID},
        {0,2,25,0,TEXT_STRING,"Диапазон датчика CO2",          0},
        {0,2,37,70,WRITE_DATA,"от", SENSOR2_MIN_ID},
        {0,70,37,0,WRITE_DATA,"до" , SENSOR2_MAX_ID},
        {0,2,50,0,WRITE_DATA,"Тип",SENSOR2_TYPE_ID},
        {1,2,62,0,WRITE_DATA,"Смещение",SENSOR2_OFFSET_ID},
};

static xScreenObjet const CDVSettingsAnalogScreen4[]=
{
        {0,10,LINE1,0,READ_DATA,"Настройки     ",  SETTINGANALOG4_TITLE_ID},
        {0,2,25,0,TEXT_STRING,"Диапазон датчика H",          0},
        {0,2,37,70,WRITE_DATA,"от", SENSOR3_MIN_ID},
        {0,70,37,0,WRITE_DATA,"до" , SENSOR3_MAX_ID},
        {0,2,50,0,WRITE_DATA,"Тип",SENSOR3_TYPE_ID},
        {1,2,62,0,WRITE_DATA,"Смещение",SENSOR3_OFFSET_ID},
};


static xScreenObjet const CDVSettingsAnalogScreen5[]=
{
        {0,10,LINE1,0,READ_DATA,"Настройки     ",  SETTINGANALOG5_TITLE_ID},
        {0,2,25,0,TEXT_STRING,"Уставки датчиков",          0},
        {0,2,37,0,WRITE_DATA,"Уставка T",   SENSOR1_SETTING_ID},
        {0,2,50,0,WRITE_DATA,"Уставка CO2", SENSOR2_SETTING_ID},
        {1,2,62,0,WRITE_DATA,"Уставка H",   SENSOR3_SETTING_ID},
};

static xScreenObjet  CDVSettingsAnalogScreen1[]=
{
        {0,10,LINE1,0,READ_DATA,"Настройки     ",   SETTINGANALOG1_TITLE_ID},
        {0,2,25,0,WRITE_DATA,  "Приор регул.",              PRIOR_SENSOR_ID},
        {0,2,50,0,TEXT_STRING,"Послезонное регулирование", 0},
        {1,2,62,0,WRITE_DATA,"",AFTER_ZONE_SETTING_ID},
};

void vSetAfterZone( uint8_t enable)
{
    if (enable)
        CDVSettingsAnalogScreen1[2].last = 0;
    else
        CDVSettingsAnalogScreen1[2].last = 1;
}



static xScreenObjet  CDVSettingsPI1[]=
{
        {0,15,LINE1,0,READ_DATA,"Настройки     ",  SETTING10_TITLE_ID},
        {0,2,25,0,TEXT_STRING,"ПИ регулятор ", 0},
        {0,2,37,0,TEXT_STRING,"Ведуший канал 1.",0},
        {0,2,50,0,WRITE_DATA,"Кооф. П",COOF_P_ID },
        {1,2,62,0,WRITE_DATA,"Кооф. И",COOF_I_ID },
};
static xScreenObjet const CDVSettingsPI2[]=
{
        {0,15,LINE1,0,READ_DATA,"Настройки     ", SETTING11_TITLE_ID},
        {0,2,25,0,TEXT_STRING,"ПИ регулятор", 0},
        {0,2,37,0,TEXT_STRING,"Ведомый канал 2.",0},
        {0,2,50,0,WRITE_DATA,"Кооф. П",COOF_P_1_ID },
        {1,2,62,0,WRITE_DATA,"Кооф. И",COOF_I_1_ID },
};


static xScreenObjet const CDVSettingsPB[]=
{
        {0,15,LINE1,0,READ_DATA,"Настройки     ", SETTING11_TITLE_ID},
        {0,2,25,0,WRITE_DATA,"Типоразмер", BP_SZIE_ID},
        {1,2,37,0,WRITE_DATA,"Тип регул.",BP_REG_TYPE_ID},

};

static xScreenObjet const CDVSettingsScreen20[]=
{
        {0,10,LINE1,00,READ_DATA,"Настройки",               CALIBRATION_TITLE_ID},
        {0,2,25,0,TEXT_STRING,"Текущий перепад давл.",      0},
        {0,2,37,00,READ_DATA,"Канал 1",                     SENS_1_RAW_ID},
        {0,2,50,00,READ_DATA,"Канал 2",                     SENS_2_RAW_ID},
        {1,2,62,00,WRITE_DATA,"",                           ZERO_CAL_COMMAND },
};

static xScreenObjet const DeviceInit[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Тип устройства:",            0},
        {0,2,37,20,WRITE_DATA,"",                       DEVICE_TYPE_ID },
        {1,2,50,00,WRITE_DATA,"Тестовый режим:",        TEST_MODE_ID},
};

xScreenType  xDeiceInit[] =
{
        {1,DeviceInit,   0,   0,  0,  0, ENTER_COMMNAD, EXIT_COMMAND | 1 },
};


static xScreenObjet const InfoScreen2DCV[]=
{
        {0,2,LINE1,0,READ_DATA,"Текущих аварий:",ALARM_COUNT_ID},
        {0,2,25,0,READ_DATA,"",CURRENT_ALARM_COUNT_ID},
        {0,0,37,0,     READ_DATA,"Кол-во каналов",     CDV_CH_COUNT_ID},
        {1,2,50,0,     READ_DATA,"Напяжение",          AC_VOLTAGE_ID },
};

#define LAST_SET_SCREEN 11
#define PI1_SCREEN      12
#define PI2_SCREEN      13
#define ANALOG1_SCREEN  14
#define ANALOG2_SCREEN  15
#define ANALOG3_SCREEN  16
#define ANALOG4_SCREEN  17
#define ANALOG5_SCREEN  18
#define RESET_SCREEN    19

xScreenType  xScreenDCV[] =
{
  {1,CDVInfoScreen1             , 3,   2,  0,   0,   0, 0  },
  {2,InfoScreen2DCV             , 1,   3,  0,   0 ,  0, 1 },
  {3,InfoScreen3                , 2,   1,  0,   0,   4  , 1  },
  {4,SettingsScreen1            , 0,   0,  20,  5,   ENTER_COMMNAD ,3  },
  {5,SettingsScreen2            , 0,   0,  4,   6,   ENTER_COMMNAD ,3 },
  {6,SettingsScreen5            , 0,   0,  5,   7,   ENTER_COMMNAD, 3 },
  {7,CDVSettingsScreen20        , 0,   0,  6,   8,   ENTER_COMMNAD, 3 },
  {8,CDVSettingsScreen3         , 0,   0,  7,   9,   ENTER_COMMNAD ,3 },
  {9,CDVSettingsScreen4         , 0,   0,  8,   10,  ENTER_COMMNAD ,3 },
  {10,CDVSettingsScreen5        , 0,   0,  9,   11,  ENTER_COMMNAD ,3 },
  {11,CDVSettingsScreen6        , 0,   0,  10,  12,  ENTER_COMMNAD, 3 },
  {12,CDVSettingsScreen7        , 0,   0,  11,  13,  ENTER_COMMNAD, 3 },
  {13,CDVSettingsPI1            , 0,   0,  12,  14,  ENTER_COMMNAD, 3 }, //12
  {14,CDVSettingsPI2            , 0,   0,  13,  15,  ENTER_COMMNAD, 3 }, //13
  {15,CDVSettingsAnalogScreen1  , 0,   0,  14,  16,  ENTER_COMMNAD ,3 }, //14
  {16,CDVSettingsAnalogScreen2  , 0,   0,  15,  17,  ENTER_COMMNAD ,3 }, //15
  {17,CDVSettingsAnalogScreen3  , 0,   0,  16,  18,  ENTER_COMMNAD ,3 }, //16
  {18,CDVSettingsAnalogScreen4  , 0,   0,  17,  19,  ENTER_COMMNAD ,3 }, //17
  {19,CDVSettingsAnalogScreen5  , 0,   0,  18,  20,  ENTER_COMMNAD ,3 }, //18
  {20, ResetScreen              , 0,   0,  19,  4,   ENTER_COMMNAD ,3 },  //16
};

u8 seting_sting_count = 0;



u8 getScreenCount()
{
    return (seting_sting_count);
}

void SetPID2Screen(CHANNEL_COUNT_t state, INPUT_SENSOR_TYPE_t analog_state)
{
    switch (state)
    {
        case BP_CONFIG :
            //Режим ВР
            xScreenDCV[PI1_SCREEN].pDownScreenSet = PI2_SCREEN + 1;
            xScreenDCV[PI1_SCREEN].pUpScreenSet   = LAST_SET_SCREEN  + 1;
            switch (analog_state)
            {
                 case INP_DISCRETE_INPUT:
                 case INP_ROOM_CONTROLLER:
                     seting_sting_count =12;
                     xScreenDCV[PI2_SCREEN].pDownScreenSet = RESET_SCREEN + 1;
                     xScreenDCV[PI2_SCREEN].pUpScreenSet   = PI1_SCREEN+ 1;
                     xScreenDCV[RESET_SCREEN].pUpScreenSet = PI2_SCREEN+1;
                     break;
                 case INP_PASSIVE_T_SENSOR:
                     seting_sting_count =13;
                     xScreenDCV[PI2_SCREEN].pDownScreenSet =  ANALOG1_SCREEN +1;
                     xScreenDCV[PI2_SCREEN].pUpScreenSet   = PI1_SCREEN+ 1;
                     xScreenDCV[ANALOG1_SCREEN].pDownScreenSet = RESET_SCREEN+1;
                     xScreenDCV[ANALOG1_SCREEN].pUpScreenSet   = PI2_SCREEN+1;
                     xScreenDCV[RESET_SCREEN].pUpScreenSet   = ANALOG1_SCREEN +1;
                     break;
                 case INP_ANALOG_SENSOR:
                     seting_sting_count =17;
                     xScreenDCV[PI2_SCREEN].pDownScreenSet =  ANALOG1_SCREEN +1;
                     xScreenDCV[PI2_SCREEN].pUpScreenSet   = PI1_SCREEN+ 1;
                     xScreenDCV[ANALOG1_SCREEN].pDownScreenSet = ANALOG5_SCREEN +1;
                     xScreenDCV[ANALOG1_SCREEN].pUpScreenSet   = PI2_SCREEN+1;
                     xScreenDCV[RESET_SCREEN].pUpScreenSet   =  ANALOG5_SCREEN +1;
                     break;


                     break;

            }
            CDVSettingsScreen5[3].last =   1;
            xScreenDCV[13].pScreenCurObjets = CDVSettingsPB;
            break;
       case ONE_CH:
           xScreenDCV[PI1_SCREEN].pUpScreenSet   = LAST_SET_SCREEN  + 1;
           switch (analog_state)
           {
               case INP_DISCRETE_INPUT:

                        seting_sting_count =11;
                        xScreenDCV[PI1_SCREEN].pDownScreenSet = RESET_SCREEN + 1;
                        xScreenDCV[RESET_SCREEN].pUpScreenSet = PI1_SCREEN+1;
                        break;
               case INP_PASSIVE_T_SENSOR:
                        seting_sting_count =12;
                        xScreenDCV[PI1_SCREEN].pDownScreenSet = ANALOG1_SCREEN +1;
                        xScreenDCV[ANALOG1_SCREEN].pDownScreenSet = RESET_SCREEN+1;
                        xScreenDCV[ANALOG1_SCREEN].pUpScreenSet   = PI1_SCREEN+1;
                        xScreenDCV[RESET_SCREEN].pUpScreenSet   = ANALOG1_SCREEN +1;
                        break;
               case INP_ANALOG_SENSOR:
                        seting_sting_count =13;
                        xScreenDCV[PI1_SCREEN].pDownScreenSet = ANALOG1_SCREEN +1;
                        xScreenDCV[ANALOG1_SCREEN].pDownScreenSet = ANALOG2_SCREEN +1;
                        xScreenDCV[ANALOG1_SCREEN].pUpScreenSet   = PI1_SCREEN+1;
                        xScreenDCV[RESET_SCREEN].pUpScreenSet   =  ANALOG2_SCREEN +1;
                        break;
               case INP_ROOM_CONTROLLER:
                       break;
                }
           CDVSettingsScreen5[3].last = 1;
           break;
    case TWO_CH:
        xScreenDCV[PI1_SCREEN].pDownScreenSet = PI2_SCREEN + 1;
        xScreenDCV[PI1_SCREEN].pUpScreenSet   = LAST_SET_SCREEN  + 1;
        switch (analog_state)
                 {

                case INP_DISCRETE_INPUT:

                     seting_sting_count =12;
                     xScreenDCV[PI2_SCREEN].pDownScreenSet = RESET_SCREEN + 1;
                     xScreenDCV[PI2_SCREEN].pUpScreenSet   = PI1_SCREEN+ 1;
                     xScreenDCV[RESET_SCREEN].pUpScreenSet = PI2_SCREEN+1;
                     break;
                case INP_PASSIVE_T_SENSOR:
                    seting_sting_count =13;
                    xScreenDCV[PI2_SCREEN].pDownScreenSet =  ANALOG1_SCREEN +1;
                    xScreenDCV[PI2_SCREEN].pUpScreenSet   = PI1_SCREEN+ 1;
                    xScreenDCV[ANALOG1_SCREEN].pDownScreenSet = RESET_SCREEN+1;
                    xScreenDCV[ANALOG1_SCREEN].pUpScreenSet   = PI2_SCREEN+1;
                    xScreenDCV[RESET_SCREEN].pUpScreenSet   = ANALOG1_SCREEN +1;
                    break;
                case INP_ANALOG_SENSOR:
                    seting_sting_count =14;
                    xScreenDCV[PI2_SCREEN].pDownScreenSet =  ANALOG1_SCREEN +1;
                    xScreenDCV[PI2_SCREEN].pUpScreenSet   = PI1_SCREEN+ 1;
                    xScreenDCV[ANALOG1_SCREEN].pDownScreenSet = ANALOG2_SCREEN +1;
                    xScreenDCV[ANALOG1_SCREEN].pUpScreenSet   = PI2_SCREEN+1;
                    xScreenDCV[RESET_SCREEN].pUpScreenSet   =  ANALOG2_SCREEN +1;
                    break;
                case INP_ROOM_CONTROLLER:
                    break;
                 }
        CDVSettingsScreen5[3].last = 0;
        xScreenDCV[13].pScreenCurObjets = CDVSettingsPI2;
        break;
    }
}


void vSettingCoountCondfig( u8 setting)
{
    if (setting)
    {
        CDVSettingsScreen7[3].last = 0;
        CDVSettingsScreen4[5].last = 0;
    }
    else
    {
        CDVSettingsScreen7[3].last = 1;
        CDVSettingsScreen4[5].last = 1;
    }

}

void SetBPSetting(u8 set)
{
    if (set)
    {
         setReg8(MEASERING_UNIT,0);
         CDVSettingsScreen6[2].last = 1;
         xScreenDCV[0].pScreenCurObjets = CDVInfoScreen1_1;
    }
    else
    {
         CDVSettingsScreen6[2].last = 0;
         xScreenDCV[0].pScreenCurObjets = CDVInfoScreen1;
    }
}













