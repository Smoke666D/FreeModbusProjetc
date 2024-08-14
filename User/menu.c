/*
 * menu.c
 *
 *  Created on: Jul 16, 2024
 *      Author: i.dymov
 */
#include "menu.h"
#include "system_init.h"
#include "hw_lib_keyboard.h"
#include "menu_data.h"
#include "u8g2.h"
#include "led.h"
#include "hal_rtc.h"

u8 pCurrMenu = 0;
static QueueHandle_t     pKeyboard        = NULL;
static KeyEvent          TempEvent        = { 0U };

void vDraw( xScreenObjet * pScreenDraw);


void vMenuInit(  )
{

    pKeyboard = *( xKeyboardQueue());
}

u8 GetID( u8 id)
{
    for (u8 i = 0; i< 100;i++)
    {

        if ((xScreens1[i].ScreenId == 0) || (xScreens1[i].ScreenId ==id)) return (i);
    }
}

void vMenuTask ( void )
{

   u8 key;
   if ( uxQueueMessagesWaiting(pKeyboard) != 0)
    {
        if ( xQueueReceive(pKeyboard, &TempEvent, 0U ) == pdPASS )
        {
        if ( TempEvent.Status == MAKECODE )
        {

          switch ( TempEvent.KeyCode )
          {
              case 0:

                  break;
              case 1:
                  break;
              case 2:
                  break;
              case 3:
                  break;
              case 4:
                  pCurrMenu= GetID(xScreens1[pCurrMenu].pRigthScreen);
                  break;
              case 5:
                  pCurrMenu = GetID(xScreens1[pCurrMenu].pLeftScreen);
                  break;


          }
        }
      }
    }
    vDraw(xScreens1[pCurrMenu].pScreenCurObjets);


}

void vGetData(u16 data_id, u8 * str)
{
    HAL_TimeConfig_T time;
    HAL_DateConfig_T date;
    switch (data_id)
    {
    case IP_ADRESS_DATA_ID:
        strcpy(str,"000.000.000.000");
        break;
    case AC_VOLTAGE_ID:
        sprintf(str,"%i В",(uint16_t)getAIN(AC220));
        break;
    case CURENT_TIME_ADDR:
        HAL_RTC_ReadTime( &time);
        sprintf(str,"%02i:%02i:%02i",time.hours,time.minutes,time.seconds);
        break;
    case CURENT_DATE_ADDR:
        HAL_RTC_ReadDate(&date);
        sprintf(str,"%02i:%02i:%02i",date.date,date.month,date.year);
        break;
    default:
        strcpy(str,"0,0");
        break;
    }
}


void vDraw( xScreenObjet * pScreenDraw)
{
    u8g2_ClearBuffer(&u8g2);
    u8 x,y;
    u8 len;
    u8 str[20];
    for (u8 i=0;i<MAX_STRING_NUMBER;i++)
    {
        switch (pScreenDraw[i].xType)
        {
        case TEXT_STRING:
            x = pScreenDraw[i].x;
            y = pScreenDraw[i].y;
            u8g2_DrawUTF8(&u8g2,x,y,pScreenDraw[i].pStringParametr);
            break;
        case READ_DATA:
            x = pScreenDraw[i].x;
            y = pScreenDraw[i].y;
            u8g2_DrawUTF8(&u8g2,x,y,pScreenDraw[i].pStringParametr);
            x = pScreenDraw[i].x_data;
            vGetData(pScreenDraw[i].DataID,str);
            len = u8g2_GetUTF8Width(&u8g2,str);
            u8g2_DrawUTF8(&u8g2,128-len,y,str);
            break;
        case WRITE_DATA:
            x = pScreenDraw[i].x;
            y = pScreenDraw[i].y;
            u8g2_DrawUTF8(&u8g2,x,y,pScreenDraw[i].pStringParametr);
            x = pScreenDraw[i].x_data;
            vGetData(pScreenDraw[i].DataID,str);
            u8g2_DrawUTF8(&u8g2,x,y,str);
            break;
        }
        if (pScreenDraw[i].last) break;
    }
}
