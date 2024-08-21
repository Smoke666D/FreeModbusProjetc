/*
 * menu_data.c
 *
 *  Created on: Jul 16, 2024
 *      Author: i.dymov
 */

#include "menu_data.h"
#include "menu.h"


static xScreenObjet const InfoScreen1[]=
{
        {0,0,12,70,READ_DATA,"Состояние",PROCESS_STATE_ID},
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
        {0,2,12,70,READ_DATA,"Дата",CURENT_DATE_ADDR},
        {0,2,25,70,READ_DATA,"Время",CURENT_TIME_ADDR},
        {0,2,37,90,READ_DATA,"Напяжение",AC_VOLTAGE_ID },
        {0,2,50,70,READ_DATA,"Адрес ModBus",MB_RTU_ADDR_ID},
        {1,2,62,15,READ_DATA,"",IP_ADRESS_DATA_ID },
};


static xScreenObjet const InfoScreen4[]=
{
        {0,2,12,70,READ_DATA,"Температура воздуха",0},
        {0,2,25,70,READ_DATA,"MAC",0},
        {0,2,37,90,READ_DATA,"Напяжение",AC_VOLTAGE_ID },
        {0,2,50,70,READ_DATA,"Моточасы",0},
        {1,2,62,15,READ_DATA,"",IP_ADRESS_DATA_ID },
};

static xScreenObjet const SettingsScreen1[]=
{
        {0,10,12,70,TEXT_STRING,"Настройки      1/5",0},
        {0,2,25,70,WRITE_DATA,"Режим управл.",CONTROL_MODE_ID },
       // {0,2,37,90,WRITE_DATA,"",},
        {0,2,50,70,WRITE_DATA,"Адрес ModBus",MB_RTU_ADDR_ID},
        {1,2,62,15,WRITE_DATA,"",IP_ADRESS_DATA_ID },
};
static xScreenObjet const SettingsScreen2[]=
{
        {0,10,12,70,TEXT_STRING,"Настройки      2/5",0},
        {0,2,25,70,TEXT_STRING,"Режим управления",0},
       // {0,2,37,90,READ_DATA,"Напяжение",AC_VOLTAGE_ID },
       // {0,2,50,70,READ_DATA,"Адрес ModBus",MB_RTU_ADDR_ID},
        {1,2,62,15,READ_DATA,"",IP_ADRESS_DATA_ID },
};
static xScreenObjet const SettingsScreen3[]=
{
        {0,10,12,70,TEXT_STRING,"Настройки      3/5",0},
        {0,2,25,70,TEXT_STRING,"Режим управления",0},
       // {0,2,37,90,READ_DATA,"Напяжение",AC_VOLTAGE_ID },
       // {0,2,50,70,READ_DATA,"Адрес ModBus",MB_RTU_ADDR_ID},
        {1,2,62,15,READ_DATA,"",IP_ADRESS_DATA_ID },
};
static xScreenObjet const SettingsScreen4[]=
{
        {0,10,12,70,TEXT_STRING,"Настройки      4/5",0},
        {0,2,25,70,TEXT_STRING,"ПИ регулятор",0},
       // {0,2,37,90,READ_DATA,"Напяжение",AC_VOLTAGE_ID },
       // {0,2,50,70,READ_DATA,"Адрес ModBus",MB_RTU_ADDR_ID},
        {1,2,62,15,READ_DATA,"",IP_ADRESS_DATA_ID },
};
static xScreenObjet const SettingsScreen5[]=
{
        {0,10,12,70,TEXT_STRING,"Настройки      5/5",0},
        {0,2,25,70,TEXT_STRING,"Текущий перепад давл.",0},
       // {0,2,37,90,READ_DATA,"Напяжение",AC_VOLTAGE_ID },
       // {0,2,50,70,READ_DATA,"Адрес ModBus",MB_RTU_ADDR_ID},
        {1,2,62,15,READ_DATA,"",IP_ADRESS_DATA_ID },
};

//����ڧ�ѧߧڧ� ��ܧ�ѧߧ�� �ԧݧѧӧ�ߧԧ� �ާ֧ߧ�


xScreenType  xScreens1[SCREENS_COUNT] =
{
  {1,InfoScreen1,      3,   2,  9,  9, 0, 0 },
  {2,InfoScreen2,      1,   3,  0,  0, 0, 0 },
  {3,InfoScreen3,      2,   1,  0,  0, 4, 0 },
  {4,SettingsScreen1,  0,   0,  8,  5, ENTER_COMMNAD | MAX_MENU_COUNT, 3  },
  {5,SettingsScreen2,  0,   0,  4,  6, 0, 3 },
  {6,SettingsScreen3,  0,   0,  5,  7, 0, 3 },
  {7,SettingsScreen4,  0,   0,  6,  8, 0, 3 },
  {8,SettingsScreen5,  0,   0,  7,  4, 0, 3 },
  {9,InfoScreen4,      0,   0,  1,  1, 0, 0 },
};


