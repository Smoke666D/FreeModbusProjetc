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


//§°§á§Ú§ã§Ñ§ß§Ú§Ö §ï§Ü§â§Ñ§ß§à§Ó §Ô§Ý§Ñ§Ó§à§ß§Ô§à §Þ§Ö§ß§ð
xScreenType  xScreensLev1[MENU_LEVEL1_COUNT]=
{
  {Info1MainScreen,     &xMainMenu, NULL    ,0U,0U},
  {Info2MainScreen,     &xMainMenu, NULL    ,0U,0U},
  {SettingMainScreen,   &xMainMenu, NULL    ,0U,0U},
};
/*---------------------------------------------------------------------------------------------------*/
/*
 * §°§Ò§ñ§Ó§Õ§Ö§ß§Ú§Ö §à§Ò§ì§Ö§Ü§Ñ-§Ü§Ñ§â§å§ã§Ö§Ý§Ú §ï§Ü§â§Ñ§ß§à§Ó §Ó§Ö§â§ç§ß§Ö§Ô§à §å§â§à§Ó§ß§ñ
 */
xScreenSetObject xMainMenu =
{
  xScreensLev1,
  ( MENU_LEVEL1_COUNT - 1U ),
  0U,
  ( void* )&xInfoScreenCallBack,
};
