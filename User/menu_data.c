/*
 * menu_data.c
 *
 *  Created on: Jul 16, 2024
 *      Author: i.dymov
 */



#include "menu_data.h"
#include "menu.h"

static const char   SETTING_TEXT[] = "Уставка";
static const char   VOLTAGE_TEXT[] = "Напряжение";
static const char   KOOFKPS_TEXT[] = "Кооэ к.п.с";

static  xScreenObjet const InfoScreen1[]=
{
        {0,0,LINE1,70,READ_DATA,"Состояние",PROCESS_STATE_ID},
        {0,0,25,70,READ_DATA,(char*)SETTING_TEXT,SETTING_ID },
        {0,0,37,70,READ_DATA,"Факт. расх.",FACT_RASH_ID},
        {0,0,50,70,READ_DATA,"Фильтр",FILTER_STATE_ID },
        {1,0,62,70,READ_DATA,"Режим",MODE_STATE_ID },
};

static xScreenObjet const InfoScreen2[]=
{
        {0,2,LINE1,70,READ_DATA,"Текущих аварий:",ALARM_COUNT_ID},
        {0,2,25,70,READ_DATA,"",CURRENT_ALARM_COUNT_ID},
        {0,2,50,0, TEXT_STRING,     "Журнaл аварий",0},
        {1,2,62,70,READ_DATA,"Записей:",JOURNAL_COUNT_ID},
};
static xScreenObjet const InfoScreen3[]=
{
        {0,2,LINE1,70,READ_DATA,"Дата",CURENT_DATE_ADDR},
        {0,2,25,70,READ_DATA,"Время",CURENT_TIME_ADDR},
        {0,2,37,70,WRITE_DATA,"Режим управл.",CONTROL_MODE_ID },
        {0,2,50,70,READ_DATA,"Адрес ModBus",MB_RTU_ADDR_ID},
        {1,2,62,15,READ_DATA,"",IP_ADRESS_DATA_ID },
};

static xScreenObjet const InfoScreen4[]=
{
        {0,2,LINE1,70,READ_DATA,"Темп. воздуха",0},
        {0,2,25,70,READ_DATA,"MAC",MAC_ADRESS_ID},
        {0,2,37,90,READ_DATA,(char*)VOLTAGE_TEXT,AC_VOLTAGE_ID },
        {0,2,50,70,TEXT_STRING,"Моточасы",0},
        {1,2,62,15,READ_DATA,"",HOURE_COUNTER_ID },
};

static xScreenObjet const SettingsScreen1[]=
{
        {0,10,LINE1,70,READ_DATA, "Настройки",          SETTING1_TITLE_ID },
        {0,2,25,70,WRITE_DATA,"Режим управл.",          CONTROL_MODE_ID },
        {0,2,37,70,WRITE_DATA,"Протокол связи.",        PROTOCOL_ID },
        {0,2,50,70,WRITE_DATA,"Адрес ModBus",           MB_RTU_ADDR_ID},
        {1,2,62,70,WRITE_DATA,"Таймаут связи с.",       MOD_BUS_TIMEOUT_ID},
};

static xScreenObjet const SettingsScreen2[]=
{
        {0,10,LINE1,70,READ_DATA, "Настройки      ",    SETTING2_TITLE_ID },
        {0,2,25,70,WRITE_DATA,"IP",                     IP_ADRESS_DATA_ID},
        {0,2,37,90,WRITE_DATA,"IP Порт",                IP_PORT_ID},
        {0,2,50,70,WRITE_DATA,"Шлюз",                   IP_GATE_ID},
        {1,2,62,15,WRITE_DATA,"Маска",                  IP_SUBNETMASK_ID },
};

static xScreenObjet const SettingsScreen3[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки       3/9",    0},
        {0,2,25,70,TEXT_STRING,"Уставки реж.  м^3/ч",    0},
        {0,2,37,70,WRITE_DATA,"Режим 1 (Осн.)",                SETTING1_ID},
        {0,2,50,90,WRITE_DATA,"Режим 2 (Доп.)",                SETTING2_ID},
        {1,2,62,70,WRITE_DATA,(char*)KOOFKPS_TEXT,             KOOFKPS_ID},

};
static xScreenObjet const SettingsScreen4[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки       4/9",0},
        {0,2,25,70,TEXT_STRING,"Нач. сопр. фильтра",0},
        {0,2,37,90,WRITE_DATA,"",                       FILTER_LOW_ID},
        {0,2,50,70,TEXT_STRING,"Конеч. сопр. фильтра",0},
        {1,2,62,15,WRITE_DATA,"",                       FILTER_HIGH_ID },
};
static xScreenObjet const SettingsScreen5[]=
{
        {0,10,LINE1,70,WRITE_DATA,"Настройки      ",VOLTAG_SCREEN_TITLE_ID},
        {0,2,25,70,WRITE_DATA,"Напряжение min",     VOLTAGE_MIN_ON_ID},
        {0,2,37,90,WRITE_DATA,"Сброс аварии min",   VOLTAGE_MIN_OFF_ID },
        {0,2,50,70,WRITE_DATA,"Напряжение max",     VOLTAGE_MAX_ON_ID},
        {1,2,62,15,WRITE_DATA,"Cброс авари max",    VOLTAGE_MAX_OFF_ID },
};
static xScreenObjet const SettingsScreen6[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки       6/9",0},
        {0,2,25,70,TEXT_STRING,"ПИ регулятор",0},
        {0,2,37,90,WRITE_DATA,"Коэф. П",                COOF_P_ID },
        {0,2,50,70,WRITE_DATA,"Коэф. И",                COOF_I_ID },
        {1,2,62,15,WRITE_DATA,"Время усред. с",         SENS_COUNT_ID },

};
static xScreenObjet const SettingsScreen7[]=
{
        {0,10,LINE1,70,READ_DATA,"Настройки",  CALIBRATION_TITLE_ID},
        {0,2,25,70,TEXT_STRING,"Текущий перепад давл.",  0},
        {0,2,37,90,READ_DATA,"Вентилятор",               SENS_1_RAW_ID},
        {0,2,50,70,READ_DATA,"Фильтр",                   SENS_2_RAW_ID},
        {1,2,62,15,WRITE_DATA,"Откалибравать 0",         ZERO_CALIBRATE_ID },
};

static xScreenObjet const SettingsScreen8[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки       8/9",    0},
        {0,2,25,70,WRITE_DATA,"Контрастность", CONTRAST_ID},
        {0,2,37,90,WRITE_DATA,"Ост. вент с.", FAN_START_TIMEOUT_ID},
        {0,2,50,70,WRITE_DATA,"Дата:",       CURENT_DATE_ADDR  },
        {1,2,62,15,WRITE_DATA,"Время",       CURENT_TIME_ADDR  },

};


static xScreenObjet const SettingsScreen9[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки       9/9",    0},
        {0,2,25,70,TEXT_STRING,"  Для сброса журнала",  0},
        {0,2,37,90,TEXT_STRING,"      нажмите ВВОД",        0},
        {0,2,50,70,READ_DATA,"Записей:",JOURNAL_COUNT_ID},
        {1,2,62,15,WRITE_DATA,"",         JOURNAL_RESET_ID },
};


static xScreenObjet const JournalViewScreen[]=
{
        {0,2,LINE1,70,READ_DATA,"Запись журнала:",JURNAL_RECORD_ID },
        {0,2,25,70,READ_DATA,"Дата:", JOURNAL_DATE_ID },
        {0,2,37,90,READ_DATA,"Время:",               JOURNAL_TIME_ID },
        {0,2,50,70,READ_DATA,"",        JOURNAL_INFO1_ID},
        {1,2,62,15,READ_DATA,"",        JOURNAL_INFO2_ID},
};

xScreenType  xScreenFMCH[SCREENS_COUNT] =
{
  {1,InfoScreen1,      3,   2,  11,  11, 0, 0 },
  {2,InfoScreen2,      1,   3,  0,  0 , 12 | JOURNAL_VIEW_COMMAND, 1 },
  {3,InfoScreen3,      2,   1,  0,   0, 4  , 1  },
  {4,SettingsScreen1,  0,   0,  14,  5, ENTER_COMMNAD , 3  },
  {5,SettingsScreen2,  0,   0,  4,   6, ENTER_COMMNAD , 3 },
  {6,SettingsScreen3,  0,   0,  5,  7,  ENTER_COMMNAD , 3 },
  {7,SettingsScreen4,  0,   0,  6,  8,  ENTER_COMMNAD , 3  },
  {8,SettingsScreen5,  0,   0,  7,  9,  ENTER_COMMNAD , 3  },
  {9,SettingsScreen6,  0,   0,  8,  10, ENTER_COMMNAD,  3 },
  {10,SettingsScreen7, 0,   0,  9,  13, ENTER_COMMNAD,  3 },
  {11,InfoScreen4,      0,   0,  1,  1, 0, 3 },
  {12,JournalViewScreen, 0,   0, JOURNAL_NEXT , JOURNAL_PREV, 0,  2 },
  {13,SettingsScreen8, 0,   0,  10,   14,   ENTER_COMMNAD, 3 },
  {14,SettingsScreen9, 0,   0,  13,   4,   ENTER_COMMNAD, 3 },
};


/*
 *   Режим СDV
 */

static  xScreenObjet const CDVInfoScreen1[]=
{
        {0,0,LINE1,70,  READ_DATA,"Уставка 1",  CVD_SETTING1_ID},
        {0,0,25,70,     READ_DATA,"Факт 1",     CVD_FACT1_ID },
        {0,0,37,70,     READ_DATA,"Уставка 2",  CVD_SETTING2_ID},
        {1,0,50,70,     READ_DATA,"Факт 2",     CVD_FACT2_ID },
};

static  xScreenObjet const CDVInfoScreen2[]=
{
        {0,0,LINE1,70,  TEXT_STRING,"Показания датчиков",0},
        {0,0,25,70,     READ_DATA,"Температура AIT",     TEMPERATURE_AIT_ID },
        {0,0,37,70,     READ_DATA,"Температура",         TEMPERATURE_ID},
        {0,0,50,70,     READ_DATA,"CO2",                 CO2_ID },
        {1,2,62,15,     READ_DATA,"Влажность",           CDV_HUMMANITY_ID},
};


static  xScreenObjet const CDVInfoScreen3[]=
{
        {0,0,LINE1,70,  TEXT_STRING,"Датчик",0},
        {0,0,25,70,     READ_DATA,"",                   SENSOR_ID },
        {0,0,37,70,     READ_DATA,"Кол-во каналов",     CDV_CH_COUNT_ID},
        {1,2,50,90,     READ_DATA,"Напяжение",          AC_VOLTAGE_ID },
};


static xScreenObjet const CDVSettingsScreen3[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      3/21",    0},
        {0,2,25,70,TEXT_STRING,"Еденицы измерения",          0},
        {0,2,37,70,WRITE_DATA,"",                            MEASERING_UNIT_ID},
        {0,2,50,90,WRITE_DATA,"Кол-во каналов",              CDV_CH_COUNT_ID},
        {1,2,62,70,WRITE_DATA,"Смещ 2",                      OFFSET2_ID},
};

static xScreenObjet const CDVSettingsScreen4[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      4/21",    0},
        {0,2,25,70,TEXT_STRING,"Уставки",          0},
        {0,2,37,70,WRITE_DATA,"Мин.",               SETTING_MIN_ID},
        {0,2,50,90,WRITE_DATA,"Сред.",              SETTING_AVER_ID},
        {1,2,62,70,WRITE_DATA,"Макс.",              SETTING_MAX_ID},
};

static xScreenObjet const CDVSettingsScreen5[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      5/21",    0},
        {0,2,25,70,TEXT_STRING,"Аваринайя уставка",          0},
        {0,2,37,70,WRITE_DATA,"Канал 1",              FAIL_SET_CH1_ID},
        {1,2,50,90,WRITE_DATA,"Канал 2",              FAIL_SET_CH2_ID},
};

static xScreenObjet const CDVSettingsScreen6[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      6/21",    0},
        {0,2,25,70,TEXT_STRING,"Расчет потока",          0},
        {0,2,37,70,WRITE_DATA,KOOFKPS_TEXT,              KOOFKPS_ID},
        {1,2,50,90,WRITE_DATA,"F канала, м^2",           F_CHANNEL_ID},
};

static xScreenObjet const CDVSettingsScreen7[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      7/21",    0},
        {0,2,25,70,TEXT_STRING,"Таймеры",          0},
        {0,2,37,70,WRITE_DATA,"Уборка (DI4) мин",              CLEAN_TIMER_ID},
        {0,2,50,70,TEXT_STRING,"Калибровка 0 точки ожид.",          0},
        {1,2,62,90,WRITE_DATA,"закрытия клапана, сек.",          ZERO_POINT_TIMEOUT_ID},
};

static xScreenObjet const CDVSettingsScreen8[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      8/21",    0},
        {0,2,25,70,TEXT_STRING,"Выбор датчика:",          0},
        {0,2,37,70,WRITE_DATA,"",              SENSOR_TYPE_ID},
        {1,2,50,70,WRITE_DATA,"Датчики",       SENSOR_F_TYPE_ID},

};

static xScreenObjet const CDVSettingsScreen9[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      9/21",    0},
        {0,2,25,70,TEXT_STRING,"Выбор сигнала AI",          0},
        {0,2,37,70,WRITE_DATA,"KK/T",          KK_SENSOR_TYPE_ID},
        {0,2,50,70,WRITE_DATA,"CO2",           CO2_SENSOR_TYPE_ID},
        {1,2,62,90,WRITE_DATA,"H",             H_SENSOR_TYPE_ID},
};

static xScreenObjet const CDVSettingsScreen10[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      10/21",    0},
        {0,2,25,70,TEXT_STRING,"Приоритет регулирования",          0},
        {1,2,37,70,WRITE_DATA,"",              PRIOR_SENSOR_ID},
};

static xScreenObjet const CDVSettingsScreen11[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      11/21",    0},
        {0,2,25,70,WRITE_DATA,"Дат. Т мин", T_SENSOR_MIN_ID},
        {0,2,37,70,WRITE_DATA,"Дат. T маx ",T_SENSOR_MAX_ID},
        {0,2,50,70,WRITE_DATA,"Дат. CO2 мин",CO2_SENSOR_MIN_ID},
        {1,2,62,90,WRITE_DATA,"Дат. CO2 мах",CO2_SENSOR_MAX_ID},
};

static xScreenObjet const CDVSettingsScreen12[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      12/21",    0},
        {0,2,25,70,WRITE_DATA,"Дат. H мин", H_SENSOR_MIN_ID},
        {1,2,37,70,WRITE_DATA,"Дат. H маx ",H_SENSOR_MAX_ID},

};


static xScreenObjet const CDVSettingsScreen13[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      13/21",    0},
        {0,2,25,70,TEXT_STRING,"Смещение AI датчиков", 0},
        {0,2,37,70,WRITE_DATA,"T,град. С ",T_SENSOR_OFFSET_ID},
        {0,2,50,70,WRITE_DATA,"CO2 ppm",CO2_SENSOR_OFFSET_ID},
        {1,2,62,90,WRITE_DATA,"H,%",H_SENSOR_OFFSET_ID},
};

static xScreenObjet const CDVSettingsScreen14[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      14/21",    0},
        {0,2,25,70,TEXT_STRING,"Уставки датчиков", 0},
        {0,2,37,70,WRITE_DATA,"T,град. С ",T_SENSOR_SETTING_ID},
        {0,2,50,70,WRITE_DATA,"CO2 ppm",CO2_SENSOR_SETTING_ID},
        {1,2,62,90,WRITE_DATA,"H,%",H_SENSOR_SETTING_ID},
};

static xScreenObjet const CDVSettingsScreen15[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      15/21",    0},
        {0,2,25,70,TEXT_STRING,"Послезонное регулирование", 0},
        {1,2,37,70,WRITE_DATA,"",AFTER_ZONE_SETTING_ID},

};


static xScreenObjet const CDVSettingsScreen16[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      16/21",    0},
        {0,2,25,70,TEXT_STRING,"ПИ регулятор", 0},
        {0,2,37,70,TEXT_STRING,"Пассив T,Т преобраз.",0},
        {0,2,50,70,WRITE_DATA,"Кооф. П",COOF_P_ID },
        {1,2,62,90,WRITE_DATA,"Кооф. И",COOF_I_ID },
};
static xScreenObjet const CDVSettingsScreen17[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      17/21",    0},
        {0,2,25,70,TEXT_STRING,"ПИ регулятор", 0},
        {0,2,37,70,TEXT_STRING,"Датчик конц СO2.",0},
        {0,2,50,70,WRITE_DATA,"Кооф. П",COOF_P_1_ID },
        {1,2,62,90,WRITE_DATA,"Кооф. И",COOF_I_1_ID },
};

static xScreenObjet const CDVSettingsScreen18[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      18/21",    0},
        {0,2,25,70,TEXT_STRING,"ПИ регулятор", 0},
        {0,2,37,70,TEXT_STRING,"Датчик влажности H",0},
        {0,2,50,70,WRITE_DATA,"Кооф. П",COOF_P_2_ID },
        {1,2,62,90,WRITE_DATA,"Кооф. И",COOF_I_2_ID },
};

static xScreenObjet const CDVSettingsScreen19[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      19/21",    0},
        {0,2,25,70,TEXT_STRING,"ПИ регулятор", 0},
        {0,2,37,70,TEXT_STRING,"Аналог выход AO1/AO2",0},
        {0,2,50,70,WRITE_DATA,"Кооф. П",COOF_P_3_ID },
        {1,2,62,90,WRITE_DATA,"Кооф. И",COOF_I_3_ID },
};

static xScreenObjet const CDVSettingsScreen20[]=
{
        {0,10,LINE1,70,READ_DATA,"Настройки",               CALIBRATION_TITLE_ID},
        {0,2,25,70,TEXT_STRING,"Текущий перепад давл.",    0},
        {0,2,37,90,READ_DATA,"Канал 1",                     SENS_1_RAW_ID},
        {0,2,50,70,READ_DATA,"Канал 2",                     SENS_2_RAW_ID},
        {1,2,62,15,WRITE_DATA,"Откалибравать 0",            ZERO_CALIBRATE_ID },
};

xScreenType  xScreenCVD[CDV_SCREENS_COUNT] =
{
  {1,CDVInfoScreen1,   3,   2,  11,  11, 0, 0 },
  {2,CDVInfoScreen3,   1,   3,  0,  0 ,  0, 1 },
  {3,InfoScreen2,      1,   3,  0,  0 , 12 | JOURNAL_VIEW_COMMAND, 1 },
  {4,InfoScreen3,      2,   1,  0,   0, 4  , 1  },
  {5,SettingsScreen1,  0,   0,  25,  5,   ENTER_COMMNAD , 3  },
  {6,SettingsScreen2,  0,   0,  4,   6, ENTER_COMMNAD , 3 },
  {7,CDVSettingsScreen3,  0,   0,  5,  7,  ENTER_COMMNAD , 3 },
  {8,CDVSettingsScreen4,  0,   0,  6,  8,  ENTER_COMMNAD , 3  },
  {9,CDVSettingsScreen5,  0,   0,  7,  9,  ENTER_COMMNAD , 3  },
  {10,CDVSettingsScreen6,  0,   0,  8,  10, ENTER_COMMNAD,  3 },
  {11,CDVSettingsScreen7, 0,   0,  9,  12, ENTER_COMMNAD,  3 },
  {12,CDVInfoScreen2,      0,   0,  1,  1, 0, 0 },
  {13,CDVSettingsScreen8, 0,   0, JOURNAL_NEXT , JOURNAL_PREV, 0,  2 },
  {14,CDVSettingsScreen9, 0,   0,  10,   14,   ENTER_COMMNAD, 3 },
  {15,CDVSettingsScreen10, 0,   0,  13,   15,   ENTER_COMMNAD, 3 },
  {16,CDVSettingsScreen11, 0,   0,  14,   16,   ENTER_COMMNAD, 3 },
  {17,CDVSettingsScreen12, 0,   0,  15,   17,   ENTER_COMMNAD, 3 },
  {18,CDVSettingsScreen13, 0,   0,  16,   18,   ENTER_COMMNAD, 3 },
  {19,CDVSettingsScreen14, 0,   0,  17,   19,   ENTER_COMMNAD, 3 },
  {20,CDVSettingsScreen15, 0,   0,  18,   20,   ENTER_COMMNAD, 3 },
  {21,CDVSettingsScreen16, 0,   0,  19,   21,   ENTER_COMMNAD, 3 },
  {22,CDVSettingsScreen17, 0,   0,  20,   22,   ENTER_COMMNAD, 3 },
  {23,CDVSettingsScreen18, 0,   0,  21,   23,   ENTER_COMMNAD, 3 },
  {24,CDVSettingsScreen19, 0,   0,  22,   24,   ENTER_COMMNAD, 3 },
  {25,SettingsScreen5,     0,   0,  23,   25,   ENTER_COMMNAD, 3 },
  {26,CDVSettingsScreen20, 0,   0,  24,   4,    ENTER_COMMNAD, 3 },
};

/*
 *   Режим BP
 */

static  xScreenObjet const BPInfoScreen1[]=
{
        {0,0,LINE1,70,  READ_DATA,SETTING_TEXT,         BP_SETTING1_ID},
        {0,0,25,70,     READ_DATA,"Факт",               BP_FACT_ID },
        {0,0,37,70,     READ_DATA,"Типоразмер BP",      BP_SIZE_ID},
        {1,0,50,70,     TEXT_STRING,"Тип регулирования",     0 },
        {1,2,62,15,     READ_DATA,"",         BP_REGULATION_TYPE_ID },
};


static  xScreenObjet const BPInfoScreen3[]=
{
        {0,0,LINE1,70,  TEXT_STRING,"Датчик",0},
        {0,0,25,70,     READ_DATA,"",                   SENSOR_ID },
        {1,2,50,90,     READ_DATA,(char*)VOLTAGE_TEXT,          AC_VOLTAGE_ID },
};

static xScreenObjet const BPSettingsScreen3[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      3/21",    0},
        {0,2,25,70,WRITE_DATA,"Типоразмер BP",               BP_SIZE_ID},
        {0,2,37,70,WRITE_DATA,(char*)KOOFKPS_TEXT,                  KOOFKPS_ID},
        {0,2,50,90,TEXT_STRING,"Тип регулирования BP",       0},
        {1,2,62,70,WRITE_DATA,"",                            BP_REGULATION_TYPE_ID },
};

static xScreenObjet const BPSettingsScreen4[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      4/20",    0},
        {0,2,25,70,WRITE_DATA,"Кол-во каналов",              CDV_CH_COUNT_ID },
        {0,2,37,70,WRITE_DATA,"Смещ 2",                      OFFSET2_ID},
        {0,2,50,90,WRITE_DATA,"Авар кан.1",                  CH1_ERROR_ID},
        {1,2,62,70,WRITE_DATA,"Авар кан.2",                  CH2_ERROR_ID},
};

static xScreenObjet const PBSettingsScreen5[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      5/20",    0},
        {0,2,25,70,TEXT_STRING,"Уставки",          0},
        {0,2,37,70,WRITE_DATA,"Мин.",               SETTING_BP_MIN_ID},
        {0,2,50,90,WRITE_DATA,"Сред.",              SETTING_BP_AVER_ID},
        {1,2,62,70,WRITE_DATA,"Макс.",              SETTING_BP_MAX_ID},
};



xScreenType  xScreenBP[BP_SCREENS_COUNT] =
{
  {1,BPInfoScreen1,   3,   2,  11,  11, 0, 0 },
  {2,BPInfoScreen3,   1,   3,  0,  0 ,  0, 1 },
  {3,InfoScreen2,      1,   3,  0,  0 , 12 | JOURNAL_VIEW_COMMAND, 1 },
  {4,InfoScreen3,      2,   1,  0,   0, 4  , 1  },
  {5,SettingsScreen1,  0,   0,  25,  5,   ENTER_COMMNAD , 3  },
  {6,SettingsScreen2,  0,   0,  4,   6, ENTER_COMMNAD , 3 },
  {7,BPSettingsScreen3,  0,   0,  5,  7,  ENTER_COMMNAD , 3 },
  {8,BPSettingsScreen4,  0,   0,  6,  8,  ENTER_COMMNAD , 3  },
  {9,PBSettingsScreen5,  0,   0,  7,  9,  ENTER_COMMNAD , 3  },
  {10,CDVSettingsScreen7,  0,   0,  8,  10, ENTER_COMMNAD,  3 },
  {11,CDVSettingsScreen14, 0,   0,  9,  12, ENTER_COMMNAD,  3 },
  {12,CDVInfoScreen2,      0,   0,  1,  1, 0, 0 },
  {13,CDVSettingsScreen13, 0,   0, JOURNAL_NEXT , JOURNAL_PREV, 0,  2 },
  {14,CDVSettingsScreen11, 0,   0,  10,   14,   ENTER_COMMNAD, 3 },
  {15,CDVSettingsScreen12, 0,   0,  13,   15,   ENTER_COMMNAD, 3 },
  {16,CDVSettingsScreen10, 0,   0,  14,   16,   ENTER_COMMNAD, 3 },
  {17,CDVSettingsScreen9, 0,   0,  15,   17,   ENTER_COMMNAD, 3 },
  {18,CDVSettingsScreen8, 0,   0,  16,   18,   ENTER_COMMNAD, 3 },
  {19,CDVSettingsScreen15, 0,   0,  17,   19,   ENTER_COMMNAD, 3 },
  {20,CDVSettingsScreen16, 0,   0,  18,   20,   ENTER_COMMNAD, 3 },
  {21,CDVSettingsScreen17, 0,   0,  19,   21,   ENTER_COMMNAD, 3 },
  {22,CDVSettingsScreen18, 0,   0,  20,   22,   ENTER_COMMNAD, 3 },
  {23,CDVSettingsScreen19, 0,   0,  21,   23,   ENTER_COMMNAD, 3 },
  {24,SettingsScreen5,     0,   0,  23,   25,   ENTER_COMMNAD, 3 },
  {25,CDVSettingsScreen20, 0,   0,  24,   4,    ENTER_COMMNAD, 3 },
};
