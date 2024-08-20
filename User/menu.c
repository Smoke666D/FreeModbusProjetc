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
#include "data_model.h"

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

static menu_mode = 0;

void ViewScreenCallback( u8 key_code)
{

    switch ( key_code )
    {
        case 0:
           if (xScreens1[pCurrMenu].pBack < COMMNAD_MASK)
                  pCurrMenu= GetID(xScreens1[pCurrMenu].pBack);
           break;
        case 1:
           if (xScreens1[pCurrMenu].pEnter < COMMNAD_MASK)
                  pCurrMenu= GetID(xScreens1[pCurrMenu].pEnter );
          else
          {
              if (xScreens1[pCurrMenu].pEnter == ENTER_COMMNAD)  menu_mode = 1;
           }
           break;
        case 2:
           pCurrMenu= GetID(xScreens1[pCurrMenu].pUpScreenSet);
           break;
        case 3:
           pCurrMenu= GetID(xScreens1[pCurrMenu].pDownScreenSet);
           break;
        case 4:
           pCurrMenu= GetID(xScreens1[pCurrMenu].pRigthScreen);
           break;
        case 5:
           pCurrMenu = GetID(xScreens1[pCurrMenu].pLeftScreen);
            break;
     }

}


u8 edit_data[MAX_STRING_NUMBER];
u8 blink_counter = 0;

void vMenuTask ( void )
{

   if ( menu_mode == 1)
   {
       memset(edit_data,0,MAX_STRING_NUMBER);
       u8 edit_flag = 0;
       for (u8 i=0;i<MAX_STRING_NUMBER;i++)
       {
          if ( xScreens1[pCurrMenu].pScreenCurObjets->xType == WRITE_DATA)
          {
              if( edit_flag == 0)
              {
                  edit_flag = 1;
                  edit_data[i] = 2;
              }
              else
              {
                  edit_data[i] = 1;
              }
          }
       }
   }
   else
   {
       if ( uxQueueMessagesWaiting(pKeyboard) != 0)
       {
           if ( xQueueReceive(pKeyboard, &TempEvent, 0U ) == pdPASS )
           {
              if ( TempEvent.Status == MAKECODE )
              {
                  switch (menu_mode)
                  {
                      case 0:
                          ViewScreenCallback (TempEvent.KeyCode );
                          break;
                          case 2:

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

                                                     break;
                                                 case 5:

                                                     break;


                                             }
                  }



              }
            }
          }


   }
   vDraw(xScreens1[pCurrMenu].pScreenCurObjets);
   if (++blink_counter>10) blink_counter=0;

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
    case MB_RTU_ADDR_ID :
        sprintf(str,"%02i",int8GetRegister(MB_RTU_ADDR));
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
            if (( menu_mode >0) && (edit_data[i]>=2))
            {
                if (edit_data[i]==2)
                {
                   if ( blink_counter <= 5)
                   {
                       u8g2_SetDrawColor(&u8g2,0);
                   }
                }
            }
            x = pScreenDraw[i].x_data;
            vGetData(pScreenDraw[i].DataID,str);
            u8g2_DrawUTF8(&u8g2,x,y,str);
            u8g2_SetDrawColor(&u8g2,1);
            u8g2_SetFontMode(&u8g2,0);
            break;
        }
        if (pScreenDraw[i].last) break;
    }
}
