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
#include "hw_lib_din.h"


static const unsigned char rcp0606536715761_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x1F, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xF9,
  0x07, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xFC, 0xFF, 0x7F, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x1F, 0x00, 0x1C, 0x00, 0x80, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x0C, 0x00, 0x18,
  0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80,
  0x71, 0x00, 0x00, 0x20, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xE0, 0x03, 0xE0, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x07, 0x30, 0x06, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x06, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x07, 0x1C,
  0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x78, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xE0, 0x07, 0x0E, 0xFC, 0x01, 0x00, 0x00, 0x00, 0x00, 0x0C, 0xE0,
  0x01, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x87, 0x03, 0xC0, 0x1F, 0x00, 0x00,
  0x00, 0x00, 0x0E, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xC7, 0x01,
  0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x0F, 0x00, 0x00, 0x00,
  0x00, 0xE0, 0xE7, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00,
  0x3C, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x7F, 0x00, 0x00, 0x80, 0x01, 0x00,
  0x00, 0x00, 0x06, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x3F, 0x00,
  0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x06, 0x00, 0xE0, 0x00, 0x00, 0x00,
  0x00, 0xC0, 0x1F, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x06, 0x00,
  0xC0, 0x03, 0x00, 0x00, 0x00, 0xC0, 0x0F, 0x00, 0x00, 0x80, 0x01, 0x00,
  0x00, 0x00, 0x06, 0x00, 0x80, 0x07, 0x00, 0x00, 0x00, 0xC0, 0x0F, 0x00,
  0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x0F, 0x00, 0x00,
  0x00, 0xC0, 0x0F, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x0C, 0x00,
  0x00, 0x1E, 0x00, 0x00, 0x00, 0xE0, 0x0F, 0x00, 0x00, 0x80, 0x01, 0x00,
  0x00, 0x00, 0x18, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0xE0, 0x0F, 0x00,
  0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x78, 0x00, 0x00,
  0x00, 0xF0, 0x1F, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x3C, 0x00,
  0x00, 0xF0, 0x00, 0x00, 0x00, 0xF8, 0x1F, 0x00, 0x00, 0xC0, 0x00, 0x00,
  0x00, 0x00, 0x1E, 0x00, 0x00, 0xE0, 0x01, 0x00, 0x00, 0xBC, 0x1F, 0x00,
  0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0xC0, 0x03, 0x00,
  0x00, 0xBC, 0x1F, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x80, 0x07, 0x00, 0x00, 0x1E, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x0F, 0x3F, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x00,
  0x00, 0x0F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x3E, 0x00, 0x80, 0x07, 0x3E, 0x00, 0x00, 0xE0, 0x07, 0x00,
  0x00, 0x00, 0xF0, 0x01, 0x00, 0x0C, 0x3C, 0x00, 0xC0, 0x03, 0x7E, 0xC0,
  0xFF, 0xE0, 0x87, 0xE3, 0xFF, 0x0F, 0xFE, 0x0F, 0x00, 0x0F, 0x78, 0x00,
  0xC0, 0x03, 0x7E, 0xC0, 0xFF, 0xE3, 0xC7, 0xE7, 0xFF, 0x0F, 0xFF, 0x1F,
  0xC0, 0x0F, 0xF8, 0x00, 0xE0, 0x03, 0xFE, 0xC0, 0xFF, 0xE7, 0xE3, 0xEF,
  0xFF, 0x8F, 0xFF, 0x3F, 0xF0, 0x1F, 0xF0, 0x00, 0xE0, 0x07, 0xFC, 0xC0,
  0xEF, 0xE7, 0xF3, 0xE7, 0xFF, 0xCF, 0xFF, 0x7F, 0xF8, 0x0F, 0xE0, 0x01,
  0xF0, 0x0F, 0xFC, 0xC0, 0xCF, 0xE7, 0xFB, 0x03, 0x7C, 0xE0, 0x0F, 0x7F,
  0xFC, 0x03, 0xE0, 0x03, 0xF0, 0x1F, 0xFC, 0xC1, 0xCF, 0xE7, 0xFF, 0x01,
  0xFC, 0xE0, 0x07, 0x7E, 0xFE, 0x00, 0xC0, 0x03, 0xF8, 0x3F, 0xF8, 0xC1,
  0xCF, 0xE7, 0xFF, 0x00, 0xFC, 0xE0, 0x07, 0x7E, 0x7E, 0x00, 0xC0, 0x07,
  0x78, 0xFF, 0xF8, 0xC1, 0xEF, 0xE7, 0xFF, 0x00, 0xFC, 0xE0, 0x07, 0x7E,
  0x3E, 0x00, 0x80, 0x07, 0x7C, 0xFE, 0xF9, 0xC3, 0xFF, 0xE3, 0xFF, 0x01,
  0xFC, 0xE0, 0x07, 0x7E, 0x3F, 0x00, 0x00, 0x0F, 0x3C, 0xFC, 0xF3, 0xC3,
  0xFF, 0xE1, 0xFB, 0x03, 0xFC, 0xE0, 0x07, 0x7E, 0x7F, 0x00, 0x00, 0x0F,
  0x3C, 0xF8, 0xFF, 0xC7, 0x7F, 0xE0, 0xF3, 0x07, 0xFC, 0xE0, 0x0F, 0x3F,
  0xFE, 0x01, 0x00, 0x1F, 0x3E, 0xE0, 0xFF, 0xC7, 0x0F, 0xE0, 0xF3, 0x07,
  0xFC, 0xC0, 0xFF, 0x3F, 0xFE, 0x1F, 0x00, 0x1E, 0x1E, 0xC0, 0xFF, 0xCF,
  0x0F, 0xE0, 0xE3, 0x07, 0xFC, 0xC0, 0xFF, 0x1F, 0xFC, 0x1F, 0x00, 0x3E,
  0x1E, 0x80, 0xFF, 0xCF, 0x0F, 0xE0, 0xC7, 0x07, 0xFC, 0x00, 0xFF, 0x0F,
  0xF8, 0x1F, 0x00, 0x3C, 0x1F, 0x00, 0xFE, 0xDF, 0x0F, 0xE0, 0x87, 0x01,
  0xFC, 0x00, 0xFE, 0x03, 0xE0, 0x1F, 0x00, 0x7C, 0x0F, 0x00, 0xFC, 0x1F,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78,
  0x0F, 0x00, 0xF0, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xF8, 0x0F, 0x00, 0xE0, 0x7F, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x07, 0x00, 0x80, 0xFF,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0,
  0x00, 0x00, 0x00, 0xFE, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0xF8, 0x3F, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0xE0,
  0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0,
  0x00, 0x00, 0x00, 0x80, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x3F, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xF0, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0x0F,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x80, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, };


static u8 menu_mode = 0;
static u8 pCurrMenu = 0;
static QueueHandle_t     pKeyboard        = NULL;
static KeyEvent          TempEvent        = { 0U };
static u8 edit_data[MAX_STRING_NUMBER];
static u8 blink_counter = 0;
static u8 SelectEditFlag = 0;
static HAL_TimeConfig_T time;
static uint8_t error_flag;
static u8 * SENSOR_COUNT_STRING[]={"0.1","0.5","1.0","2.0","3.0","5.0","10.0"};
static u8 * ControlModeStrig[]={"DIput","RS-485","TCP IP"};
static u8 * AfterZoneStrig[]={"Tканала<Tпомещения","Tканала>Tпомещения","Автомат"};
static u8 * MUnitStrig[] = {"м^3/ч","м/c","Па"};
static u8 * PriorSentStrig[]= {"T","CO2","H"};
static const char * DevString[3]={"Режим ФМЧ","CAV/VAV/DCV","Режим BP"};
static u8 * SensorTypeStrig[]= {"0-10 В","2-10 В","4-20 мA"};
static xScreenType * pMenu;
static u8 journal_index =0;
static uint16_t curr_edit_data_id = 0;
static u8 cur_edit_index = 0;
static uint8_t start_edit_flag =0;
static uint16_t edit_data_buffer_byte;
static const uint32_t coof[]={1,10,100,1000};

static HAL_DateConfig_T date;
static u8 edit_ip_addres[4];

static u8 cur_max_stirng = 0;

#define  DateFormatString  "%02i.%02i.%02i"
#define  TimeFormatString  "%02i:%02i:%02i"

static void vDraw( xScreenObjet * pScreenDraw);
u8 vGetData(u16 data_id, u8 * str, DATA_VIEW_COMMAND_t command, u8 * index, u8 * len);


void vDrawBitmap()
{
    u8g2_DrawXBM(&u8g2,0,0,128,58,rcp0606536715761_bits);
}


void vMenuInit(  )
{
   pKeyboard = *( xKeyboardQueue());
}

u8 GetID( u8 id)
{
    for (u8 i = 0; i< SCREENS_COUNT;i++)
    {
        if ( ((pMenu[i].ScreenId & ~COMMNAD_MASK) ==id)) return (i);
    }
    return (1);
}

static void SetFirtsEditString( )
{
    memset(edit_data,0,MAX_STRING_NUMBER);
    u8 edit_flag = 0;
    for (u8 i=0;i<MAX_STRING_NUMBER;i++)
    {
        if ( pMenu[pCurrMenu].pScreenCurObjets[i].xType == WRITE_DATA)
        {
            if( edit_flag == 0)
            {
                 edit_flag    = 1;
                 edit_data[i] = 2;
            }
            else
            {
                edit_data[i] = 1;
            }
        }
        if ( pMenu[pCurrMenu].pScreenCurObjets[i].last == 1 )
        {
            cur_max_stirng  = i +1;
            break;
        }
     }
}

void ViewScreenCallback( u8 key_code)
{
    uint16_t pscreen = 0;
    switch ( key_code )
    {
        case EXIT_KEY:

           pscreen =  pMenu[pCurrMenu].pBack ;
           break;
        case ENTER_KEY:
           pscreen =  pMenu[pCurrMenu].pEnter ;
           break;
        case UP_KEY:
            pscreen = pMenu[pCurrMenu].pUpScreenSet;
           break;
        case DOWN_KEY:
            pscreen = pMenu[pCurrMenu].pDownScreenSet;
           break;
        case RIGTH_KEY:
            pscreen = pMenu[pCurrMenu].pRigthScreen;
           break;
        default:
            pscreen = pMenu[pCurrMenu].pLeftScreen;
            break;
     }
     if (pscreen)
     {
         switch (pscreen  & COMMNAD_MASK )
         {
             case ENTER_COMMNAD:
                  SetFirtsEditString();
                  menu_mode = 2;
                  SelectEditFlag  = 1;
                  break;
             case JOURNAL_VIEW_COMMAND:
                 journal_index = 0;
                 break;
             case JOURNAL_NEXT:
                 if ((journal_index+1) < getReg16(RECORD_COUNT)) journal_index++;
                 break;
              case JOURNAL_PREV:
                 if ((journal_index+1) > 1) journal_index--;
                 break;
              case EXIT_COMMAND:
                  NVIC_SystemReset();
                  break;
         }
         if (pscreen & ~COMMNAD_MASK)
             pCurrMenu = GetID(pscreen);
     }
}



static void vSelect( u8 direction)
{
    uint8_t index;
    for (index =0; index < cur_max_stirng ;index ++)
    {
        if (edit_data[index ] == 2)
        {
            edit_data[index ] = 1;
            break;
        }
    }
   for (uint8_t i = 0; i < cur_max_stirng ; i++)
   {
        if (direction==0)
        {
          if (index == 0) index = (cur_max_stirng -1);
              else
          index--;
        }
        else
        {
          if (++index >= cur_max_stirng ) index = 0;
        }
        if (edit_data[index ] == 1)
        {
           edit_data[index ] = 2;
           break;
        }
    }
}



u8 vSetEdit()
{
    u8 res= 1;
    for (u8 i =0; i < cur_max_stirng ;i ++)
    {
        if (edit_data[i ] == 2)
        {
              edit_data[i] = 3;
              curr_edit_data_id = pMenu[pCurrMenu].pScreenCurObjets[i].DataID;
              res = vGetData( curr_edit_data_id , 0,CMD_START_EDIT ,0,0);
              break;
        }
    }
    SelectEditFlag = 1;
    return (res);

}

void vSetCommnad( DATA_VIEW_COMMAND_t cmd )
{
    for (u8 i =0; i < cur_max_stirng ;i ++)
    {
      if (edit_data[i ] == 3)
      {
          edit_data[i] = 2;
          vGetData( pMenu[pCurrMenu].pScreenCurObjets[i].DataID, 0,cmd,0,0);
          break;
      }
   }
   menu_mode = 2;
}


static DATA_VIEW_COMMAND_t const COMMNAD_MAP_ARRAY[]={CMD_NEXT_EDIT,CMD_PREV_EDIT,CMD_INC,CMD_DEC};

void vMenuTask ( void )
{
       if ( uxQueueMessagesWaiting(pKeyboard) != 0)
       {
           if ( xQueueReceive(pKeyboard, &TempEvent, 0U ) == pdPASS )
           {
              if ( TempEvent.Status == MAKECODE )
              {
                  switch (menu_mode)
                  {
                      default:
                          ViewScreenCallback (TempEvent.KeyCode );
                          break;
                      case 2:
                          switch ( TempEvent.KeyCode )
                          {
                               case EXIT_KEY: if  (SelectEditFlag) SelectEditFlag = 0; else { menu_mode = 0;pCurrMenu = GetID(pMenu[pCurrMenu].pBack); } break;
                               case ENTER_KEY:
                                       if  (!SelectEditFlag )
                                        {
                                           SelectEditFlag  = 1;
                                           SetFirtsEditString();
                                        }
                                       else
                                       {
                                           if (vSetEdit() == 1)
                                           {
                                               vSetCommnad(CMD_EXIT_EDIT);
                                               SelectEditFlag = 0;
                                               menu_mode = 0;
                                           }
                                           else
                                           {
                                               menu_mode = 3;
                                           }
                                       }
                                       break;
                               case UP_KEY: if   (SelectEditFlag)  vSelect(1);
                                       else
                                       {
                                           pCurrMenu = GetID(pMenu[pCurrMenu].pUpScreenSet);
                                       }
                                       break;
                               case DOWN_KEY: if   (SelectEditFlag) vSelect(0);
                                       else
                                       {
                                           pCurrMenu = GetID(pMenu[pCurrMenu].pDownScreenSet);
                                       }
                                       break;
                               default:  break;
                           }
                          break;
                     case 3:
                         if ( TempEvent.KeyCode  <=  UP_KEY )
                             vGetData( curr_edit_data_id, 0,COMMNAD_MAP_ARRAY[TempEvent.KeyCode],0,0);
                         else
                             vSetCommnad(( TempEvent.KeyCode == ENTER_KEY ) ? CMD_SAVE_EDIT : CMD_EXIT_EDIT);
                         break;
                  }
              }
            }
          }
   vDraw((xScreenObjet *) pMenu[pCurrMenu].pScreenCurObjets);
   if (++blink_counter>10) blink_counter=0;
}



void MenuSetDevice()
{
    switch  ( SystemInitGetDevType())
    {
        case DEV_FMCH:
            pMenu = xScreenFMCH;
            break;
        case DEV_CDV:
            pMenu = xScreenCVD;
            break;
        case DEV_BP:
            pMenu = xScreenBP;
            break;
    }
}

void MenuSetDeviceMenu()
{
    pMenu = xDeiceInit;
}


void vByteDataEdit(u8 size, u16 data_id, DATA_VIEW_COMMAND_t command ,u8 max_index , uint16_t max_data, uint16_t min_data )
{
    switch (command)
    {
        case CMD_START_EDIT:
            edit_data_buffer_byte = (size == 0) ?(u16) getReg8( data_id ) : getReg16( data_id ) ;
            start_edit_flag = 1;
            cur_edit_index = 0;
            break;
        case CMD_SAVE_EDIT:
            if (size == 0)
                SaveReg8( data_id, (u8)edit_data_buffer_byte );
            else
                saveReg16( data_id, edit_data_buffer_byte );
            start_edit_flag = 0;
             break;
        case CMD_NEXT_EDIT:
             if (++cur_edit_index > max_index) cur_edit_index = 0;
             break;
        case CMD_PREV_EDIT:
            if (cur_edit_index ==0) cur_edit_index = max_index; else cur_edit_index--;
               break;
        case CMD_INC:
             if ((edit_data_buffer_byte + coof[cur_edit_index]) <=  max_data )
                 edit_data_buffer_byte=edit_data_buffer_byte+coof[cur_edit_index];
             else
                 edit_data_buffer_byte = max_data;
             break;
       case CMD_DEC:
             if (  (edit_data_buffer_byte - min_data) >=  coof[cur_edit_index] )
                 edit_data_buffer_byte=edit_data_buffer_byte-coof[cur_edit_index];
             else
                  edit_data_buffer_byte = min_data;
             break;
       default:
             start_edit_flag = 0;
             break;
     }

}

float edit_data_buffer_float;
static const float coof_float[]={0.01,0.1,1.0,10.0,100.0,1000.0,10000.0};

void vFloatDataEdit( u16 data_id, DATA_VIEW_COMMAND_t command ,u8 max_index , u8 min_index, float max_data, float min_data )
{
    u8 offset;
    u8 temp_index;
    switch ( min_index )
    {
       case 0:
           offset  =2;
           break;
       case 1:
           offset = 1;
           break;
       case 2:
           offset = 0;
           break;

    }

    switch (command)
    {
        case CMD_START_EDIT:
            edit_data_buffer_float= getRegFloat( data_id );
            start_edit_flag = 1;
            cur_edit_index = min_index+1;
            break;
        case CMD_SAVE_EDIT:
            saveRegFloat( data_id, edit_data_buffer_float );
            start_edit_flag = 0;
             break;
        case CMD_NEXT_EDIT:
             if (++cur_edit_index >=max_index + min_index) cur_edit_index = 0;
             if (cur_edit_index == min_index) cur_edit_index++;
             break;
        case CMD_PREV_EDIT:
            if (cur_edit_index == 0) cur_edit_index = max_index+1; else cur_edit_index--;
            if (cur_edit_index == min_index) cur_edit_index--;
               break;
        case CMD_INC:
            temp_index = cur_edit_index;
            if (temp_index > min_index)  temp_index--;

            if ((edit_data_buffer_float + coof_float[temp_index]) <=  max_data )
                 edit_data_buffer_float = edit_data_buffer_float+ coof_float [temp_index + offset];
             else
                 edit_data_buffer_float = max_data;
             break;
       case CMD_DEC:
            temp_index = cur_edit_index;
            if (temp_index > min_index)  temp_index--;
             if (  (edit_data_buffer_float - min_data) >=  coof_float[temp_index] )
                 edit_data_buffer_float = edit_data_buffer_float-coof_float[temp_index + offset];
             else
                 edit_data_buffer_float = min_data;
             break;
       default:
             start_edit_flag = 0;
             break;
     }

}



void vIPDataEdit( u16 data_id, DATA_VIEW_COMMAND_t command )
{
    switch (command)
    {
        case CMD_START_EDIT:
            for (u8 i=0;i<4;i++)
                edit_ip_addres[i]=  getReg8( data_id +i);
            start_edit_flag = 1;
            cur_edit_index = 0;
            break;
        case CMD_SAVE_EDIT:
            for (u8 i=0;i<4;i++)
                SaveReg8( data_id+ i , edit_ip_addres[i] );
            start_edit_flag = 0;
             break;
        case CMD_NEXT_EDIT:
             if (++cur_edit_index >=15) cur_edit_index = 0;
             if ((cur_edit_index == 3) || (cur_edit_index == 7) ||  (cur_edit_index == 11)) cur_edit_index ++;
             break;
        case CMD_PREV_EDIT:
            if (cur_edit_index ==0 ) cur_edit_index = 0; else cur_edit_index--;
            if ((cur_edit_index == 3) || (cur_edit_index == 7) ||  (cur_edit_index == 11)) cur_edit_index --;
               break;
        case CMD_INC:
             if (cur_edit_index < 3)
             {
                 if (( edit_ip_addres[3] + coof[cur_edit_index %3]) <=  255 )
                     edit_ip_addres[3] =edit_ip_addres[3]+ coof[cur_edit_index %3];
                 else
                     edit_ip_addres[3] = 255;

             } else if (cur_edit_index < 7)
             {
                 if (( edit_ip_addres[2] + coof[(cur_edit_index - 4)%3]) <=  255 )
                           edit_ip_addres[2] =edit_ip_addres[2]+  coof[(cur_edit_index - 4) %3];
                 else
                          edit_ip_addres[2] = 255;
             }
             else if (cur_edit_index < 11)
             {
                  if (( edit_ip_addres[1] + coof[(cur_edit_index -8)%3]) <=  255 )
                      edit_ip_addres[1] =edit_ip_addres[1]+ coof[(cur_edit_index - 8) %3];
                  else
                      edit_ip_addres[1] = 255;
              }
             else
              {
                  if (( edit_ip_addres[0] + coof[(cur_edit_index - 12)%3]) <=  255 )
                                        edit_ip_addres[0] =edit_ip_addres[0]+  coof[(cur_edit_index - 12) %3];
                  else
                      edit_ip_addres[0] = 255;
              }
             break;
       case CMD_DEC:
              if (cur_edit_index < 3)
              {
                  if (( edit_ip_addres[3]  >=  coof[cur_edit_index %3]))
                        edit_ip_addres[3] =edit_ip_addres[3]- coof[cur_edit_index %3];
                  else
                       edit_ip_addres[3] = 0;
              }
              else if (cur_edit_index < 7)
              {
                  if (( edit_ip_addres[2] >= coof[(cur_edit_index - 4)%3]))
                         edit_ip_addres[2] =edit_ip_addres[2]-  coof[(cur_edit_index - 4) %3];
                  else
                        edit_ip_addres[2] = 0;
             }
             else if (cur_edit_index < 11)
             {
                  if (( edit_ip_addres[1] >= coof[(cur_edit_index -8)%3]))
                         edit_ip_addres[1] =edit_ip_addres[1]-  coof[(cur_edit_index - 8) %3];
                  else
                         edit_ip_addres[1] = 255;
             }
             else
             {
                 if (( edit_ip_addres[0] >= coof[(cur_edit_index - 12)%3]))
                        edit_ip_addres[0] =edit_ip_addres[0]-  coof[(cur_edit_index - 12) %3];
                 else
                       edit_ip_addres[0] = 255;
             }
             break;
       default:
             start_edit_flag = 0;
             break;
     }

}


void  vTimeDataEdit(DATA_VIEW_COMMAND_t command, u8 * str)
{
    if ( command == CMD_READ)  HAL_RTC_ReadTime( &time);
    switch(command)
    {
       case CMD_READ:
       case CMD_EDIT_READ:
            sprintf(str,TimeFormatString,time.hours,time.minutes,time.seconds);
            break;
       case CMD_START_EDIT:;
            start_edit_flag = 1;
            cur_edit_index = 0;
            break;
       case CMD_SAVE_EDIT:
            HAL_RTC_ConfigTime(&time);
            start_edit_flag = 0;
            break;
       case CMD_NEXT_EDIT:
            if (++cur_edit_index >7) cur_edit_index = 0;
            if ((cur_edit_index == 2) || (cur_edit_index == 5)  ) cur_edit_index ++;
            break;
       case CMD_PREV_EDIT:
            if (cur_edit_index ==0 ) cur_edit_index = 7; else cur_edit_index--;
            if ((cur_edit_index == 2) || (cur_edit_index == 5) ) cur_edit_index --;
            break;
       case CMD_INC:
            if (cur_edit_index < 2)
            {
                if (( time.seconds + coof[cur_edit_index %2]) <=  59 )
                    time.seconds += coof[cur_edit_index %2];
                else
                    time.seconds = 59;
            }
            else if (cur_edit_index < 5)
            {
                if (( time.minutes + coof[(cur_edit_index - 3)%2]) <=  59 )
                    time.minutes +=  coof[(cur_edit_index - 3) %2];
                else
                    time.minutes = 59;
            }
            else
            {
                if (( time.hours + coof[(cur_edit_index - 6)%2]) <=  23 )
                    time.hours +=  coof[(cur_edit_index -6) %2];
                else
                    time.hours = 23;
            }
            break;
       case CMD_DEC:
            if (cur_edit_index < 2)
            {
                if (( time.seconds  >=  coof[cur_edit_index %2]))
                    time.seconds -=coof[cur_edit_index %2];
                else
                     time.seconds = 0;
            }
            else if (cur_edit_index < 5 )
            {
                if (( time.minutes >= coof[(cur_edit_index - 3)%2]))
                    time.minutes -= coof[(cur_edit_index - 3) %2];
                else
                    time.minutes = 0;
            }
            else
            {
                if (( time.hours >= coof[(cur_edit_index - 6)%2]))
                    time.hours -=  coof[(cur_edit_index - 6) %2];
                else
                    time.hours = 0;
            }
            break;
       case CMD_EXIT_EDIT:
            start_edit_flag = 0;
            break;
    }
}


static void vDateDataEdit(DATA_VIEW_COMMAND_t command, char * str)
{
    if ( command ==  CMD_READ) HAL_RTC_ReadDate(&date);
    switch(command)
    {
       case CMD_READ:
       case CMD_EDIT_READ:
             sprintf(str,DateFormatString,date.date,date.month,date.year);
             break;
       case CMD_START_EDIT:
             start_edit_flag = 1;
             cur_edit_index = 0;
             break;
      case CMD_SAVE_EDIT:
             HAL_RTC_ConfigDate(&date);
             start_edit_flag = 0;
             break;
      case CMD_NEXT_EDIT:
             if (++cur_edit_index >7) cur_edit_index = 0;
             if ((cur_edit_index == 2) || (cur_edit_index == 5)  ) cur_edit_index ++;
             break;
      case CMD_PREV_EDIT:
             if (cur_edit_index ==0 ) cur_edit_index = 0; else cur_edit_index--;
             if ((cur_edit_index == 2) || (cur_edit_index == 5) ) cur_edit_index --;
              break;
      case CMD_INC:
              if (cur_edit_index < 2)
              {
                 if (( date.year + coof[cur_edit_index %2]) <=  99 )
                     date.year += coof[cur_edit_index %2];
                 else
                     date.year = 99;
               }
               else if (cur_edit_index < 5)
               {
                  if (( date.month +  coof[(cur_edit_index - 3)%2]) <=  12)
                        date.month += coof[(cur_edit_index - 3)%2];
                  else
                      date.month = 12;
               }
               else
               {
                   if (( date.date + coof[(cur_edit_index - 6)%2]) <=  31 )
                       date.date +=  coof[(cur_edit_index -6) %2];
                    else
                        date.date = 31;
               }
               break;
              case CMD_DEC:
                     if (cur_edit_index < 2)
                     {
                         if (( date.year  >  coof[cur_edit_index %2]))
                             date.year -=    coof[cur_edit_index %2];
                         else
                             date.year = 1;
                     }
                     else if (cur_edit_index < 5)
                     {
                         if (( date.month > coof[(cur_edit_index - 3)%2]))
                             date.month -=  coof[(cur_edit_index - 3)%2];
                         else
                             date.month = 1;
                    }
                    else
                    {
                        if (( date.date > coof[(cur_edit_index - 6)%2]))
                            date.date -=  coof[(cur_edit_index - 6)%2];
                        else
                            date.date = 1;
                    }
                    break;
              case CMD_EXIT_EDIT:
                    start_edit_flag = 0;
                    break;
    }
}



static const u16 MenuRegMap[]={ DEVICE_TYPE,
                                0,   //1
                                0,   //2
                                0,   //3
                                AC220,   //4
                                IP_1, //5
                                MB_RTU_ADDR, //6
                                CONTROL_TYPE, //7
                                0,           //8
                                MASK_1,        //9
                                LOW_VOLTAGE_OFF,            //10
                                LOW_VOLTAGE_ON,           //11
                                CONTRAST,   //12
                                MOD_BUS_TIMEOUT,   //13
                                SENSOR_COUNT,    //14
                                KOOFKPS,    //15
                                MB_PROTOCOL_TYPE,    //16
                                0,                  //17
                                SENS1,              //18
                                SENS2,              //19
                                COOF_P,            //20
                                COOF_I,            //21
                                IP_PORT,                  //22
                                GATE_1,           //23
                                HIGH_VOLTAGE_ON,      //24
                                0,                    //25
                                0,                    //26
                                0,                    //27
                                0,                    //28
                                HIGH_VOLTAGE_OFF,           //29



};
static const u16 MenuFMCHRegMap[]={
0,                   //30
                             0,                   //31
                             FILTER_LOW,          //32
                             FILTER_HIGH,          //32
                             SETTING1,           //34
                             SETTING2,           //35
                             FAN_START_TIMEOUT, //36
                             0,                    //37
                             0,                    //38
                             0,                    //39
                             0,                    //40
                             0,                    //41
                             0,                    //42
                             0,                    //43
                             0,      //44
                             0,      //45
                             0,       //46
};

static const u16 MenuCDV_BPRegMap[54]=
                        {
                                 0,    //0
                                 0,    //1
                                 0,    //2
                                 0,    //3
                                 0,    //4
                                 0,    //5
                                 0,    //6
                                 0,    //7
                                 0,    //8
                                 SETTING_MIN ,    //9
                                 SETTING_MID ,    //10
                                 SETTING_MAX,    //11
                                 CH1_SETTING,    //12
                                 CH2_SETTING,    //13
                                 F_CHANNEL,    //14
                                 0,    //15
                                 0,    //16
                                 TEMP_MIN_SET,    //17
                                 TEMP_MAX_SET,    //18
                                 CO2_MIN_SET,    //19
                                 CO2_MAX_SET,    //20
                                 H_MIN_SET,    //21
                                 H_MAX_SET,    //22
                                 0,    //23
                                 0,    //24
                                 0,    //25
                                 0,    //26
                                 0,    //27
                                 0,    //28
                                 AFTER_ZONE_SETTING,    //29
                                 CDV_BP_CH_COUNT,    //30
                                 MEASERING_UNIT,    //31
                                 CLEAN_TIMER,    //32
                                 ZERO_POINT_TIMEOUT,    //33
                                 PRIOR_SENSOR ,    //34
                                 KK_SENSOR_TYPE,    //35
                                 CO2_SENSOR_TYPE,    //36
                                 H_SENSOR_TYPE,    //37
                                 OFFSET_CH2,    //38
                                 COOF_P1,    //39
                                 COOF_I1,    //40
                                 COOF_P2,    //41
                                 COOF_I2,    //42
                                 COOF_P3,    //43
                                 COOF_I3,    //44
                                 0,    //45
                                 0,    //46
                                 0,    //47
                                 0,    //48
                                 0,    //49
                                 0,    //50
                                 0,    //51
                                 0,    //52
                                 0,    //53
                                 0,    //54

                         };




#define FILTER_ERROR  0x01
#define SETTING_ERROR 0x02
#define LOW_VOLTAGE_ERROR 0x04
#define HIGH_VOLTAGE_ERROR 0x08


static u8 error_shif = 0;
static u8 const *  ErrorString[]={"HEPA Фильтр засорен","Невозможно","Низкое напряжение","Высокое напряжение"};
static u8 const *  ViewErrorString[]={"HEPA Фильтр засорен","Невоз. поддер. устав!","Низкое напряжение","Высокое напряжение"};
static u8 const *  SettingTilteStirnf[]={"1/9","1/19","1/18"};
static u8 const *  Setting2TilteStirnf[]={"2/9","2/19","2/18"};
static u8 const *  VoltageTilteStirnf[] ={"5/9","18/19","17/18"};
static u8 const *  CalTilteStirnf[] ={"7/9","19/19","20/20"};

void vSetTitle(u16 data_id, u8 * str )
{
    u8 dev_type = getReg8(DEVICE_TYPE);
    switch (data_id)
    {
           case CALIBRATION_TITLE_ID:
               strcpy(str,CalTilteStirnf[dev_type]);
               break;
           case SETTING1_TITLE_ID:
               strcpy(str,SettingTilteStirnf[dev_type]);
               break;
           case SETTING2_TITLE_ID:
               strcpy(str,Setting2TilteStirnf[dev_type]);
               break;
           case VOLTAG_SCREEN_TITLE_ID:
               strcpy(str,VoltageTilteStirnf[dev_type]);
               break;
    }

}
void vSetCDV_PB(u16 data_id, u8 * str, DATA_VIEW_COMMAND_t command,  u8 * len, u8 * res)
{
    u16 reg_id = MenuCDV_BPRegMap[data_id - CVD_SETTING1_ID];
    switch (data_id)
    {

    case COOF_P_1_ID:
    case COOF_I_1_ID:
    case COOF_P_2_ID:
    case COOF_I_2_ID:
    case COOF_P_3_ID:
    case COOF_I_3_ID:
        switch (command)
                  {
                      case CMD_READ:
                          sprintf(str,"%+07.2f",getRegFloat(reg_id));
                          break;
                      case CMD_EDIT_READ:
                          sprintf(str,"%+07.2f",edit_data_buffer_float );
                          break;
                     default:
                         vFloatDataEdit(reg_id, command,6,2,999.99,-999.99);
                         break;
                  }
                  break;
        case KK_SENSOR_TYPE_ID:
        case CO2_SENSOR_TYPE_ID:
        case H_SENSOR_TYPE_ID:
            *len = 0;
            switch (command)
            {
                case CMD_EDIT_READ:
                    strcpy(str, SensorTypeStrig[ edit_data_buffer_byte ] );
                    break;
                case CMD_READ:
                    strcpy(str, SensorTypeStrig[ getReg8( reg_id)] );
                    break;
                default:
                    vByteDataEdit(0,reg_id,command,0,T4_20, T0_10 );
                    break;
            }

            break;
        case ZERO_POINT_TIMEOUT_ID:
            switch (command)
            {
                case CMD_READ:
                     sprintf(str,"%03i",getReg16(reg_id) );
                     break;
               case CMD_EDIT_READ:
                     sprintf(str,"%03i",edit_data_buffer_byte );
                     break;
               default:
                    vByteDataEdit(1,reg_id,command,2,999,0);
                    break;
            }
            break;
        case CLEAN_TIMER_ID:
            switch (command)
            {
                 case CMD_READ:
                       sprintf(str,"%02i",getReg8(reg_id) );
                       break;
                 case CMD_EDIT_READ:
                       sprintf(str,"%02i",edit_data_buffer_byte );
                       break;
                default:
                      vByteDataEdit(0,reg_id,command,2,99,0);
                      break;
            }
            break;
        case AFTER_ZONE_SETTING_ID:
            *len = 0;
            switch (command)
            {
                case CMD_EDIT_READ:
                    strcpy(str, AfterZoneStrig[ edit_data_buffer_byte ] );
                    break;
                case CMD_READ:
                    strcpy(str, AfterZoneStrig[ getReg8( reg_id)] );
                    break;
                default:
                    vByteDataEdit(0,reg_id,command,0,T_AUTO , TCH_TROOM );
                    break;
            }
            break;
        case CDV_CH_COUNT_ID:
            switch (command)
              {
                  case CMD_READ:
                      sprintf(str,"%01i",getReg8(reg_id) );
                      break;
                  case CMD_EDIT_READ:
                      sprintf(str,"%01i",edit_data_buffer_byte );
                      break;
                  default:
                      vByteDataEdit(0,reg_id,command,0,2,1);
                      break;
              }
            break;
        case MEASERING_UNIT_ID:
            *len = 0;
             switch (command)
             {
                  case CMD_EDIT_READ:
                      strcpy(str,MUnitStrig[ edit_data_buffer_byte ] );
                      break;
                 case CMD_READ:
                      strcpy(str, MUnitStrig[ getReg8( reg_id)] );
                      break;
                 default:
                      vByteDataEdit(0,reg_id,command,0,PA_U , M3_CH_U );
                      break;
             }
            break;
        case OFFSET2_ID:
        case SETTING_MIN_ID:
        case SETTING_AVER_ID:
        case SETTING_MAX_ID:
        case FAIL_SET_CH1_ID:
        case FAIL_SET_CH2_ID:
            switch (command)
            {
                 case CMD_READ:
                      sprintf(str,"%+07.1f",getRegFloat(reg_id));
                      break;
                 case CMD_EDIT_READ:
                      sprintf(str,"%+07.1f",edit_data_buffer_float );
                      break;
                default:
                      vFloatDataEdit(reg_id, command,5,1,9999.9,-9999.9);
                      break;
            }
            break;
        case PRIOR_SENSOR_ID:
             *len = 0;
              switch (command)
              {
                   case CMD_EDIT_READ:
                        strcpy(str,PriorSentStrig[ edit_data_buffer_byte ] );
                        break;
                   case CMD_READ:
                        strcpy(str, PriorSentStrig[ getReg8( reg_id)] );
                        break;
                   default:
                        vByteDataEdit(0,reg_id,command,0,H_PRIOR , TEM_PRIOR );
                        break;
             }
             break;
        case F_CHANNEL_ID:
            switch (command)
                      {
                          case CMD_READ:
                              sprintf(str,"%+07.4f",getRegFloat(reg_id));
                              break;
                          case CMD_EDIT_READ:
                              sprintf(str,"%+07.4f",edit_data_buffer_float );
                              break;
                         default:
                             vFloatDataEdit(reg_id, command,4,1,9.999,-9.999);
                             break;
                      }
            break;

            case T_SENSOR_MIN_ID:
            case T_SENSOR_MAX_ID:
            case H_SENSOR_MIN_ID:
            case H_SENSOR_MAX_ID:
                          switch (command)
                          {
                               case CMD_READ:
                                    sprintf(str,"%03i",getReg16(reg_id) );
                                    break;
                               case CMD_EDIT_READ:
                                    sprintf(str,"%03i",edit_data_buffer_byte );
                                    break;
                               default:
                                    vByteDataEdit(1,reg_id,command,2,999,0);
                                    break;
                         }
                         break;
            case CO2_SENSOR_MIN_ID:
            case CO2_SENSOR_MAX_ID:
                           switch (command)
                                       {
                                            case CMD_READ:
                                                 sprintf(str,"%04i",getReg16(reg_id) );
                                                 break;
                                            case CMD_EDIT_READ:
                                                 sprintf(str,"%04i",edit_data_buffer_byte );
                                                 break;
                                            default:
                                                 vByteDataEdit(1,reg_id,command,3,9999,0);
                                                 break;
                                      }
                                      break;

                break;
                case ZERO_CAL_COMMAND:
                    switch (command)
                    {
                        case CMD_READ:
                            if ((USER_GetProccesState() == USER_RROCCES_WORK))
                                  strcpy(str,"Откалибравать 0?");
                            else
                            {
                                if (SelectEditFlag )  strcpy(str,"Отменить калиборвку?");
                                else
                                  strcpy(str,"Калиборвка...");
                            }
                            break;

                        case CMD_START_EDIT:
                            if ((USER_GetProccesState() == USER_RROCCES_WORK))
                                SystemCalibraionStart();
                            else
                                SystemCalibraionStop();
                             *res = 1;
                             break;
                        default:
                            start_edit_flag = 0;
                            break;

                    }
                    break;
    }
}

static char * const FMCH_MODE_STRING[]= {"1 (Основной)","2 (Доп.)"};

void vSetFMCH(u16 data_id, u8 * str, DATA_VIEW_COMMAND_t command,  u8 * len, u8 * res)
{
    int16_t temp_int;
    u8 temp_byte,temp_state = 0;
    u16   reg_id = MenuFMCHRegMap[data_id- FMCH_FIRST];
    switch (data_id)
    {
         case MODE_STATE_ID:
                 strcpy(str,FMCH_MODE_STRING[getReg8(MODE )]);
                 break;
        case JOURNAL_TIME_ID:
               if (getReg16(RECORD_COUNT)!=0)
                   sprintf(str,TimeFormatString,time.hours,time.minutes,time.seconds);
               break;
           case JOURNAL_DATE_ID:
               if (getReg16(RECORD_COUNT)!=0)
                   sprintf(str,DateFormatString,date.date,date.month,date.year);
               break;
           case JOURNAL_INFO1_ID:
               if (getReg16(RECORD_COUNT)!=0)
                   strcpy(str,ErrorString[error_flag]);
               break;
           case JOURNAL_INFO2_ID:
               if (getReg16(RECORD_COUNT)!=0)
               {
                   switch (error_flag)
                   {
                       case 0:
                           strcpy(str,"более 90%");
                           break;
                       case 2:
                           sprintf(str,"сети < %i В",getReg8(LOW_VOLTAGE_ON));
                           break;
                       case 3:
                           sprintf(str,"сети > %i В",getReg8(HIGH_VOLTAGE_ON));
                           break;
                       default:
                           strcpy(str,"поддерживать уставку");
                           break;
                   }
               }
                break;
           case JURNAL_RECORD_ID:
               if  (getReg16(RECORD_COUNT) == 0)
                   strcpy(str,"00/00");
               else
               {
                   sprintf(str,"%02i/%02i",journal_index+1, getReg16(RECORD_COUNT));
                   vGetRecord(journal_index,&error_flag,&time,&date);
               }
               break;
           case JOURNAL_COUNT_ID:
                 sprintf(str,"%02i",getReg16(RECORD_COUNT) );
                 break;
           case FILTER_HIGH_ID:
           case FILTER_LOW_ID:
               switch (command)
               {
                    case CMD_READ:
                         sprintf(str,"%03i",getReg16(reg_id) );
                         break;
                    case CMD_EDIT_READ:
                         sprintf(str,"%03i",edit_data_buffer_byte );
                         break;
                    default:
                         vByteDataEdit(1,reg_id,command,2,999,0);
                         break;
              }
              break;
           case SETTING1_ID:
           case SETTING2_ID:
               switch (command)
               {
                   case CMD_READ:
                       sprintf(str,"%04i",getReg16(reg_id) );
                       break;
                 case CMD_EDIT_READ:
                       sprintf(str,"%04i",edit_data_buffer_byte );
                       break;
                 default:
                      vByteDataEdit(1,reg_id,command,3,9999,0);
                      break;
             }
             break;
               case FACT_RASH_ID:
                       temp_int = USER_GetFact(&temp_state);
                       if (temp_state)
                           sprintf(str,"%04i м^3/ч", temp_int);
                       else {
                           sprintf(str,"---- м^3/ч");
                       }
                       break;
           case FAN_START_TIMEOUT_ID:
               switch (command)
               {
                       case CMD_READ:
                           sprintf(str,"%02i",getReg8(reg_id) );
                           break;
                       case CMD_EDIT_READ:
                           sprintf(str,"%02i",edit_data_buffer_byte );
                           break;
                       default:
                           vByteDataEdit(0,reg_id,command,2,99,1);
                           break;
                }
                break;
               case JOURNAL_RESET_ID:
                   switch (command )
                   {
                       case CMD_EDIT_READ:
                           break;
                       case CMD_READ:
                           if ( SelectEditFlag )
                               strcpy(str,"     Сбросить журнал?   ");
                           else
                               strcpy(str,"");
                           break;
                       case CMD_START_EDIT:
                            saveReg16(RECORD_INDEX, 0);
                            saveReg16(RECORD_COUNT, 0);
                            *res = 1;
                            break;
                      default:
                          start_edit_flag = 0;
                          break;
                   }
                   break;
               case CUR_TEMP_ID:
                      sprintf(str, "%i C",(u8)getAIN(DCAIN5));
                      break;
               case SETTING_ID:
                     sprintf(str,"%04i м^3/ч", USER_GetSetting());
                     break;
               case FILTER_STATE_ID:
                   temp_byte = USER_FilterState(&temp_state);
                   if (temp_state)
                   {
                       sprintf(str,"%i Па %03i %%",getAIN(SENS2),temp_byte);
                   }
                   else
                   {
                       sprintf(str,"%i Па ---%%",getAIN(SENS2));
                   }
                   break;
    }
}

u8 vGetData(u16 data_id, u8 * str, DATA_VIEW_COMMAND_t command, u8 * index, u8 * len)
{
    u8 res = 0;
    u8 MACAddr[6];
    u8 temp_byte,temp_state = 0;
    u16 max,min;
    str[0]=0;
    if (index!=0) *index = cur_edit_index;
    if (len!=0)   *len = 1;
    if (( data_id >=TITLE_FIRST) &&  ( data_id <=TITLE_LAST))
    {
        vSetTitle(data_id,str);
    }
    else
    if ((data_id>= CDV_BP_FIRST) && ( data_id<=CDV_BP_LAST))
    {
        vSetCDV_PB(data_id,str,command,len,&res);
    }
    else
    if ((data_id>= FMCH_FIRST) && ( data_id<=FMCH_LAST))
    {
        vSetFMCH(data_id,str,command,len,&res);
    }
    else
    {
        u16 reg_id = MenuRegMap[data_id];
        switch (data_id)
        {
            case DEVICE_TYPE_ID:
                *len = 0;
                switch (command)
                {
                      case CMD_EDIT_READ:
                          strcpy(str, DevString[ edit_data_buffer_byte ] );
                          break;
                      case CMD_READ:
                          strcpy(str, DevString[ getReg8( reg_id)] );
                          break;
                      default:
                          vByteDataEdit(0,reg_id,command,0,2 , 0);
                          break;
                }
                break;
           case ALARM_COUNT_ID:
               temp_state =0;
               temp_byte =  USER_GerErrorState();
               for (u8 i=0;i<8;i++)
               {
                   if (temp_byte & 0x01) temp_state++;
                   temp_byte = temp_byte>>1;
               }
               sprintf(str,"%02i",temp_state);
               break;
          case CURRENT_ALARM_COUNT_ID:
               temp_byte =  USER_GerErrorState();
               if (temp_byte != 0)
               {
                   if ((blink_counter ==0) || ((temp_byte>>error_shif) & 0x01)==0)
                   {
                     for (u8 i=0;i<5;i++)
                     {
                          if (++error_shif>3) error_shif = 0;

                           if ((temp_byte>>error_shif) & 0x01)
                           {
                               break;
                           }
                     }
                   }
                   strcpy(str,ViewErrorString[error_shif]);
               }
               break;
        case COOF_P_ID:
        case COOF_I_ID:
        case KOOFKPS_ID:
            switch (command)
            {
                case CMD_READ:
                    sprintf(str,"%+07.2f",getRegFloat(reg_id));
                    break;
                case CMD_EDIT_READ:
                    sprintf(str,"%+07.2f",edit_data_buffer_float );
                    break;
               default:
                   vFloatDataEdit(reg_id, command,6,2,999.99,-999.99);
                   break;
            }
            break;
        case SENS_1_RAW_ID:
        case SENS_2_RAW_ID:
            sprintf(str, "%i Па",(int)getAIN(reg_id) );
            break;
        case MAC_ADRESS_ID:
            WCHNET_GetMacAddr(MACAddr);
            sprintf(str,"%x%x%x%x%x%x",MACAddr[0],MACAddr[1],MACAddr[2],MACAddr[3],MACAddr[4],MACAddr[5]);
            break;
        case PROCESS_STATE_ID:
            switch( getProcessStateCode() )
            {
                 default:
                    strcpy(str,"Остановлен");
                    break;
                case CALIBRATE_CODE:
                    strcpy(str,"Калибровка");
                    break;
                case WORK_CODE:
                    strcpy(str,"Работа");
                    break;
            }
            break;
        case IP_ADRESS_DATA_ID:
        case IP_GATE_ID:
        case IP_SUBNETMASK_ID:
            switch (command)
            {
                case CMD_READ:
                    sprintf(str,"%03i.%03i.%03i.%03i",getReg8(reg_id),getReg8(reg_id+1),getReg8(reg_id+2),getReg8(reg_id+3));
                    break;
                case CMD_EDIT_READ:
                    sprintf(str,"%03i.%03i.%03i.%03i",edit_ip_addres[0],edit_ip_addres[1],edit_ip_addres[2],edit_ip_addres[3]);
                    break;
                default:
                    vIPDataEdit(reg_id,command);
            }
            break;
        case IP_PORT_ID:
            switch (command)
            {
                case CMD_READ:
                    sprintf(str,"%03i",getReg16(reg_id) );
                    break;
                case CMD_EDIT_READ:
                    sprintf(str,"%03i",edit_data_buffer_byte );
                     break;
                default:
                    vByteDataEdit(1,reg_id,command,2,999,0);
                    break;
            }
            break;
        case AC_VOLTAGE_ID:
            sprintf(str,"%i В",(uint16_t)getAIN(reg_id));
            break;
        case CURENT_TIME_ADDR:
            vTimeDataEdit(command,str);
            break;
        case CURENT_DATE_ADDR:
            vDateDataEdit(command,str);
            break;
        case VOLTAGE_MIN_ON_ID:
        case VOLTAGE_MIN_OFF_ID:
        case VOLTAGE_MAX_ON_ID:
        case VOLTAGE_MAX_OFF_ID:
            switch (command)
            {
                case CMD_READ:
                    sprintf(str,"%03i",getReg8(reg_id) );
                    break;
                case CMD_EDIT_READ:
                    sprintf(str,"%03i",edit_data_buffer_byte );
                    break;
               default:
                    if ( data_id == VOLTAGE_MIN_ON_ID)
                    {
                        max = getReg8(MenuRegMap[VOLTAGE_MIN_OFF_ID])-1;
                        min = 100;
                    }
                    else
                    if ( data_id == VOLTAGE_MIN_OFF_ID)
                    {
                        max = getReg8(MenuRegMap[VOLTAGE_MAX_OFF_ID])-1;
                        min = getReg8(MenuRegMap[VOLTAGE_MIN_ON_ID])+1;
                    }
                    else

                    if ( data_id == VOLTAGE_MAX_OFF_ID)
                    {
                        min = getReg8(MenuRegMap[VOLTAGE_MIN_OFF_ID])+1;
                        max = getReg8(MenuRegMap[VOLTAGE_MAX_ON_ID])-1;
                    }
                    else
                    {
                        min = getReg8(MenuRegMap[VOLTAGE_MAX_OFF_ID])+1;
                        max =255;
                    }
                    vByteDataEdit(0,reg_id,command,2,max,min);
                    break;
            }
            break;
        case ZERO_CALIBRATE_ID:
            if (command == CMD_READ )
            {
              if ( SelectEditFlag )
              {
                   if ((USER_GetProccesState() == USER_PROCCES_IDLE))
                               strcpy(str,"Старт?");
                           else
                               strcpy(str,"Заблок");
              }
            }
            else if (command == CMD_START_EDIT)
            {
                if ((USER_GetProccesState() == USER_PROCCES_IDLE))
                {
                  CalibrateZeroStart();
                }
                res = 1;
            }
            else
                start_edit_flag = 0;
            break;
        case DEVICE_RESET_ID :
               switch (command )
               {
                   case CMD_EDIT_READ:
                       break;
                   case CMD_READ:
                       if ( SelectEditFlag )
                           strcpy(str,"     Перегрузить?    ");
                       break;
                   case CMD_START_EDIT:
                        NVIC_SystemReset();
                        break;
                  default:
                        start_edit_flag = 0;
                        break;
               }
               break;
           case CONTRAST_ID:
           case MB_RTU_ADDR_ID :
           case MOD_BUS_TIMEOUT_ID:
               switch (command)
               {
                   case CMD_READ:
                       sprintf(str,"%02i",getReg8(reg_id) );
                       break;
                   case CMD_EDIT_READ:
                       if ( data_id == CONTRAST_ID ) setReg8(reg_id,edit_data_buffer_byte);
                       sprintf(str,"%02i",edit_data_buffer_byte );
                       break;
                   default:
                       vByteDataEdit(0,reg_id,command,1,99,1);
                       break;
               }
        break;
    case CONTROL_MODE_ID:
        *len = 0;
         switch (command)
         {
            case CMD_EDIT_READ:
                 strcpy(str, ControlModeStrig[ edit_data_buffer_byte ] );
                 break;
            case CMD_READ:
                 strcpy(str, ControlModeStrig[ getReg8( reg_id)] );
                 break;
             default:
                vByteDataEdit(0,reg_id,command,0,MKV_MB_TCP, MKV_MB_DIN );
                break;
         }
         break;
    case PROTOCOL_ID:
        *len = 0;
        switch (command)
        {
            case CMD_EDIT_READ:
                strcpy(str, ControlModeStrig[ edit_data_buffer_byte ] );
                break;
            case CMD_READ:
                strcpy(str, ControlModeStrig[ getReg8( reg_id)] );
                break;
            default:
                if ( getReg8(MB_PROTOCOL_TYPE)== MKV_MB_DIN)
                {
                    vByteDataEdit(0,reg_id,command,0,MKV_MB_TCP, MKV_MB_RTU);
                }
                break;
        }
        break;
    case HOURE_COUNTER_ID:
        sprintf(str,"%000000i часов %00i минут",vRTC_TASK_GetHoure(), vRTC_TASK_GetMinute( ));
        break;
    case SENS_COUNT_ID:
        *len = 0;
        switch (command)
        {
             case CMD_READ:
                 strcpy(str,SENSOR_COUNT_STRING[getReg8(reg_id)]);
                 break;
             case CMD_EDIT_READ:
                 strcpy(str,SENSOR_COUNT_STRING[edit_data_buffer_byte ]);
                 break;
            default:
                vByteDataEdit(0,reg_id,command,0,6,0);
                break;
         }
        break;
    default:
        break;
    }

    }
    return (res);
}

static void vDraw( xScreenObjet * pScreenDraw)
{
    MENU_ClearScreen();
    u8 x,y,offset;
    u8 len,edit_index,box_len;
    u8 str[50];
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
            vGetData(pScreenDraw[i].DataID,str,CMD_READ,&edit_index,&box_len);
            len = u8g2_GetUTF8Width(&u8g2,str);
            u8g2_DrawUTF8(&u8g2,128-len,y,str);
            break;
        case WRITE_DATA:
            x = pScreenDraw[i].x;
            y = pScreenDraw[i].y;
            u8g2_DrawUTF8(&u8g2,x,y,pScreenDraw[i].pStringParametr);
            vGetData(pScreenDraw[i].DataID,str,edit_data[i]==3 ? CMD_EDIT_READ : CMD_READ,&edit_index,&box_len);
            len = u8g2_GetUTF8Width(&u8g2,str);
            if (pScreenDraw[i].x_data!=0)
              offset = pScreenDraw[i].x_data;
            else
               offset = 0;

            if (( menu_mode >0) && (edit_data[i]>=2))
            {
                if ((edit_data[i]==2) && (SelectEditFlag))
                {
                   if ( blink_counter <= 5)
                   {
                       u8g2_DrawBox( &u8g2, 127-len-offset, y-10 , len , 11 );
                       u8g2_SetDrawColor(&u8g2,0);
                       u8g2_SetFontMode(&u8g2,1);
                   }
                }
                if (edit_data[i]==3)
                {
                    if (box_len == 1)
                       u8g2_DrawBox( &u8g2, 127-6*(edit_index+1)-offset, y-10 , 6 , 11 );
                    else
                        u8g2_DrawBox( &u8g2, 127-len-offset, y-10 , len , 11 );
                    u8g2_SetDrawColor(&u8g2,2);
                    u8g2_SetFontMode(&u8g2,1);
                }
            }
            u8g2_DrawUTF8(&u8g2,127-len-offset,y,str);
            u8g2_SetDrawColor(&u8g2,1);
            u8g2_SetFontMode(&u8g2,0);
            break;
        }
        if (pScreenDraw[i].last) break;
    }
}

void MENU_DrawString(u8 x, u8 y, char const * str)
{
    u8g2_DrawUTF8(&u8g2,x,y,str);
}


void MENU_ClearScreen()
{
    u8g2_ClearBuffer(&u8g2);
}
