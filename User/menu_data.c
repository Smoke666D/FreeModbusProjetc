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
        {0,0,37,70,READ_DATA,"Факт. расход",FACT_RASH_ID},
        {0,0,50,70,READ_DATA,"Фильтр",FILTER_STATE_ID },
        {1,0,62,70,READ_DATA,"Режим",MODE_STATE_ID },
};

static xScreenObjet const InfoScreen2[]=
{
        {1,2,37,0,TEXT_STRING,"Журанл аварий",0},
};
static xScreenObjet const InfoScreen3[]=
{
        {0,2,LINE1,70,READ_DATA,"Дата",CURENT_DATE_ADDR},
        {0,2,25,70,READ_DATA,"Время",CURENT_TIME_ADDR},
       // {0,2,37,90,READ_DATA,"Напяжение",AC_VOLTAGE_ID },
        {0,2,50,70,READ_DATA,"Адрес ModBus",MB_RTU_ADDR_ID},
        {1,2,62,15,READ_DATA,"",IP_ADRESS_DATA_ID },
};

static xScreenObjet const InfoScreen4[]=
{
        {0,2,LINE1,70,READ_DATA,"Температура воздуха",0},
        {0,2,25,70,READ_DATA,"MAC",MAC_ADRESS_ID},
        {0,2,37,90,READ_DATA,"Напяжение",AC_VOLTAGE_ID },
        {0,2,50,70,READ_DATA,"Моточасы",0},
        {1,2,62,15,READ_DATA,"",HOURE_COUNTER_ID },
};

static xScreenObjet const SettingsScreen1[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      1/7",0},
        {0,2,25,70,WRITE_DATA,"Режим управл.",          CONTROL_MODE_ID },
        {0,2,37,70,WRITE_DATA,"Протокол связи.",        PROTOCOL_ID },
        {1,2,50,70,WRITE_DATA,"Адрес ModBus",           MB_RTU_ADDR_ID},
      //  {1,2,62,15,WRITE_DATA,"",                       IP_ADRESS_DATA_ID },
};

static xScreenObjet const SettingsScreen2[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      2/7",    0},
        {0,2,25,70,WRITE_DATA,"Адрес ModBus",            IP_ADRESS_DATA_ID},
        {0,2,37,90,WRITE_DATA,"IP Порт",                 IP_PORT_ID},
        {0,2,50,70,WRITE_DATA,"Шлюз",                    IP_GATE_ID},
        {1,2,62,15,WRITE_DATA,"Маска подсети",           IP_SUBNETMASK_ID },
};

static xScreenObjet const SettingsScreen3[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      3/7",    0},
        {0,2,25,70,TEXT_STRING,"Текущий перепад давл.",  0},
        {0,2,37,90,READ_DATA,"Вентилятор",               SENS_1_RAW_ID},
        {0,2,50,70,READ_DATA,"Фильтр",                   SENS_2_RAW_ID},
        {1,2,62,15,WRITE_DATA,"Откалибравать 0",         0 },
};
static xScreenObjet const SettingsScreen4[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      4/7",0},
        {0,2,25,70,TEXT_STRING,"Режим управления",0},
       // {0,2,37,90,READ_DATA,"Напяжение",AC_VOLTAGE_ID },
       // {0,2,50,70,READ_DATA,"Адрес ModBus",MB_RTU_ADDR_ID},
        {1,2,62,15,READ_DATA,"",IP_ADRESS_DATA_ID },
};
static xScreenObjet const SettingsScreen5[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      5/7",0},
        {0,2,25,70,WRITE_DATA,"Напряжение min",     VOLTAGE_MIN_ON_ID},
        {0,2,37,90,WRITE_DATA,"Сборс аварии min",   VOLTAGE_MIN_OFF_ID },
        {0,2,50,70,WRITE_DATA,"Напряжение max",     VOLTAGE_MAX_ON_ID},
        {1,2,62,15,WRITE_DATA,"Cборс авари max",    VOLTAGE_MAX_OFF_ID },
};
static xScreenObjet const SettingsScreen6[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      6/7",0},
        {0,2,25,70,TEXT_STRING,"ПИ регулятор",0},
        {0,2,37,90,WRITE_DATA,"Коэф. П",                COOF_P_ID },
        {1,2,50,70,WRITE_DATA,"Коэф. И",                COOF_I_ID },

};
static xScreenObjet const SettingsScreen7[]=
{
        {0,10,LINE1,70,TEXT_STRING,"Настройки      7/7",    0},
        {0,2,25,70,TEXT_STRING,"Текущий перепад давл.",  0},
        {0,2,37,90,READ_DATA,"Вентилятор",               SENS_1_RAW_ID},
        {0,2,50,70,READ_DATA,"Фильтр",                   SENS_2_RAW_ID},
        {1,2,62,15,WRITE_DATA,"Откалибравать 0",         0 },
};

xScreenType  xScreens1[SCREENS_COUNT] =
{
  {1,InfoScreen1,      3,   2,  11,  11, 0, 0 },
  {2,InfoScreen2,      1,   3,  0,  0, 0, 0   },
  {3,InfoScreen3,      2,   1,  0,  0, 4, 0   },
  {4,SettingsScreen1,  0,   0,  8,  5, ENTER_COMMNAD , 3  },
  {5,SettingsScreen2,  0,   0,  4,  6, 0 , 3 },
  {6,SettingsScreen3,  0,   0,  5,  7, ENTER_COMMNAD , 3 },
  {7,SettingsScreen4,  0,   0,  6,  8, 0, 3  },
  {8,SettingsScreen5,  0,   0,  7,  9, 0, 3  },
  {9,SettingsScreen6,  0,   0,  8,  10, 0, 3 },
  {10,SettingsScreen7, 0,   0, 9,   4,  0, 3 },
  {11,InfoScreen4,      0,   0,  1,  1, 0, 0 },
};


