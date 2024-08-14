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
        {0,2,12,70,READ_DATA,"Уставка 1",0},
        {0,2,25,70,READ_DATA,"Факт 1",1},
        {0,2,37,70,READ_DATA,"Уставка 2",0},
        {1,2,50,70,READ_DATA,"Факт 1",0},
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
        {0,2,50,70,READ_DATA,"Адрес ModBus",0},
        {1,2,62,15,READ_DATA,"",IP_ADRESS_DATA_ID },

};

//����ڧ�ѧߧڧ� ��ܧ�ѧߧ�� �ԧݧѧӧ�ߧԧ� �ާ֧ߧ�
xScreenType  xScreens1[] =
{
  {1,InfoScreen1,  0,   2,  0  ,0 },
  {2,InfoScreen2,  1,   0,  0,  0 },
  {0,InfoScreen3,  2,   1,  0,  0 },
};


