/*
 * menu_data.c
 *
 *  Created on: Jul 16, 2024
 *      Author: i.dymov
 */

#include "menu_data.h"
#include "menu.h"


static xScreenObjet const Info1MainScreen[]=
{


};

static xScreenObjet const Info2MainScreen[]=
{


};


static xScreenObjet const SettingMainScreen[]=
{


};


//����ڧ�ѧߧڧ� ��ܧ�ѧߧ�� �ԧݧѧӧ�ߧԧ� �ާ֧ߧ�
xScreenType  xScreensLev1[MENU_LEVEL1_COUNT]=
{
  {Info1MainScreen,     &xMainMenu, NULL    ,0U,0U},
  {Info2MainScreen,     &xMainMenu, NULL    ,0U,0U},
  {SettingMainScreen,   &xMainMenu, NULL    ,0U,0U},
};
/*---------------------------------------------------------------------------------------------------*/
/*
 * ���ҧ�ӧէ֧ߧڧ� ��ҧ�֧ܧ�-�ܧѧ���֧ݧ� ��ܧ�ѧߧ�� �ӧ֧��ߧ֧ԧ� ����ӧߧ�
 */
xScreenSetObject xMainMenu =
{
  xScreensLev1,
  ( MENU_LEVEL1_COUNT - 1U ),
  0U,
  ( void* )&xInfoScreenCallBack,
};
