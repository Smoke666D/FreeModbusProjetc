/*
 * menu_data.c
 *
 *  Created on: Jul 16, 2024
 *      Author: i.dymov
 */

#include "menu_data.h"
#include "menu.h"

static  xScreenObjet const InfoScreen1[]=
{
        {0,0,LINE1,70,READ_DATA,"Состояние",PROCESS_STATE_ID},
        {0,0,25,70,READ_DATA,"Уставка",SETTING_ID },
        {0,0,37,70,READ_DATA,"Факт. расх.",FACT_RASH_ID},
        {0,0,50,70,READ_DATA,"Фильтр",FILTER_STATE_ID },
        {1,0,62,70,READ_DATA,"Режим",MODE_STATE_ID },
};

static xScreenObjet const InfoScreen2[]=
{
        {0,2,LINE1,70,READ_DATA,"Текущаих аварий:",ALARM_COUNT_ID},
        {0,2,25,70,READ_DATA,"",CURRENT_ALARM_COUNT_ID},
        {0,2,50,0, TEXT_STRING,     "Журанл аварий",0},
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
        {0,2,37,90,READ_DATA,"Напяжение",AC_VOLTAGE_ID },
        {0,2,50,70,TEXT_STRING,"Моточасы",0},
        {1,2,62,15,READ_DATA,"",HOURE_COUNTER_ID },
};

static xScreenObjet const SettingsScreen1[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      1/9",0},
        {0,2,25,70,WRITE_DATA,"Режим управл.",          CONTROL_MODE_ID },
        {0,2,37,70,WRITE_DATA,"Протокол связи.",        PROTOCOL_ID },
        {0,2,50,70,WRITE_DATA,"Адрес ModBus",           MB_RTU_ADDR_ID},
        {1,2,62,70,WRITE_DATA,"Таймаут связи с.",       MOD_BUS_TIMEOUT_ID},
};

static xScreenObjet const SettingsScreen2[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      2/9",    0},
        {0,2,25,70,WRITE_DATA,"IP",                         IP_ADRESS_DATA_ID},
        {0,2,37,90,WRITE_DATA,"IP Порт",                    IP_PORT_ID},
        {0,2,50,70,WRITE_DATA,"Шлюз",                       IP_GATE_ID},
        {1,2,62,15,WRITE_DATA,"Маска",                      IP_SUBNETMASK_ID },
};

static xScreenObjet const SettingsScreen3[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      3/9",    0},
        {0,2,25,70,TEXT_STRING,"Уставки режимов  м^3/ч",    0},
        {0,2,37,70,WRITE_DATA,"Режим 1",                SETTING1_ID},
        {0,2,50,90,WRITE_DATA,"Режим 2",                SETTING2_ID},
        {1,2,62,70,WRITE_DATA,"Коэф Кп.с.",             KOOFKPS_ID},

};
static xScreenObjet const SettingsScreen4[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      4/9",0},
        {0,2,25,70,TEXT_STRING,"Нижний дипазон фильтра",0},
        {0,2,37,90,WRITE_DATA,"",                       FILTER_LOW_ID},
        {0,2,50,70,TEXT_STRING,"Верхний дипазон фильтра",0},
        {1,2,62,15,WRITE_DATA,"",                       FILTER_HIGH_ID },
};
static xScreenObjet const SettingsScreen5[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      5/9",0},
        {0,2,25,70,WRITE_DATA,"Напряжение min",     VOLTAGE_MIN_ON_ID},
        {0,2,37,90,WRITE_DATA,"Сборс аварии min",   VOLTAGE_MIN_OFF_ID },
        {0,2,50,70,WRITE_DATA,"Напряжение max",     VOLTAGE_MAX_ON_ID},
        {1,2,62,15,WRITE_DATA,"Cборс авари max",    VOLTAGE_MAX_OFF_ID },
};
static xScreenObjet const SettingsScreen6[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      6/9",0},
        {0,2,25,70,TEXT_STRING,"ПИ регулятор",0},
        {0,2,37,90,WRITE_DATA,"Коэф. П",                COOF_P_ID },
        {0,2,50,70,WRITE_DATA,"Коэф. И",                COOF_I_ID },
        {1,2,62,15,WRITE_DATA,"Время усред. с",         SENS_COUNT_ID },

};
static xScreenObjet const SettingsScreen7[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      7/9",    0},
        {0,2,25,70,TEXT_STRING,"Текущий перепад давл.",  0},
        {0,2,37,90,READ_DATA,"Вентилятор",               SENS_1_RAW_ID},
        {0,2,50,70,READ_DATA,"Фильтр",                   SENS_2_RAW_ID},
        {1,2,62,15,WRITE_DATA,"Откалибравать 0",         0 },
};

static xScreenObjet const SettingsScreen8[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      8/9",    0},
        {0,2,25,70,WRITE_DATA,"Контрасность", CONTRAST_ID},
        {0,2,37,90,WRITE_DATA,"Ост. вент с.", FAN_START_TIMEOUT_ID},
        {0,2,50,70,WRITE_DATA,"Дата:",       CURENT_DATE_ADDR  },
        {1,2,62,15,WRITE_DATA,"Время",       CURENT_TIME_ADDR  },

};


static xScreenObjet const SettingsScreen9[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      9/9",    0},
        {0,2,25,70,TEXT_STRING,"  Для сборса журнала",  0},
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

xScreenType  xScreens1[SCREENS_COUNT] =
{
  {1,InfoScreen1,      3,   2,  11,  11, 0, 0 },
  {2,InfoScreen2,      1,   3,  0,  0 , 12 | JOURNAL_VIEW_COMMAND, 0 },
  {3,InfoScreen3,      2,   1,  0,   0, 4  , 0  },
  {4,SettingsScreen1,  0,   0,  14,  5, ENTER_COMMNAD , 3  },
  {5,SettingsScreen2,  0,   0,  4,   6, ENTER_COMMNAD , 3 },
  {6,SettingsScreen3,  0,   0,  5,  7,  ENTER_COMMNAD , 3 },
  {7,SettingsScreen4,  0,   0,  6,  8,  ENTER_COMMNAD , 3  },
  {8,SettingsScreen5,  0,   0,  7,  9,  ENTER_COMMNAD , 3  },
  {9,SettingsScreen6,  0,   0,  8,  10, ENTER_COMMNAD,  3 },
  {10,SettingsScreen7, 0,   0,  9,  13, ENTER_COMMNAD,  3 },
  {11,InfoScreen4,      0,   0,  1,  1, 0, 0 },
  {12,JournalViewScreen, 0,   0, JOURNAL_NEXT , JOURNAL_PREV, 0,  2 },
  {13,SettingsScreen8, 0,   0,  10,   14,   ENTER_COMMNAD, 3 },
  {14,SettingsScreen9, 0,   0,  13,   4,   ENTER_COMMNAD, 3 },
};


