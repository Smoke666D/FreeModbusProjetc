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
static u8 * SensorTypeStrig[]= {"0-10","2-10","4-20"};
static xScreenType * pMenu;
static u8 journal_index =0;
static uint16_t curr_edit_data_id = 0;
static u8 cur_edit_index = 0;

static uint8_t start_edit_flag =0;
static uint16_t edit_data_buffer_byte;
static const uint32_t coof[]={1,10,100,1000};
static const float coof_float[]={0.01,0.1,1.0,10.0,100.0,1000.0,10000.0};
static HAL_DateConfig_T date;
static u8 edit_ip_addres[4];

#define  DateFormatString  "%02i.%02i.%02i"
#define  TimeFormatString  "%02i:%02i:%02i"

static void vDraw( xScreenObjet * pScreenDraw);
void vGetData(u16 data_id, u8 * str, DATA_VIEW_COMMAND_t command, u8 * index, u8 * len);


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
         }
         if (pscreen & ~COMMNAD_MASK)
             pCurrMenu = GetID(pscreen);
     }
}



static void vSelect( u8 direction)
{
    uint8_t index;
    for (index =0; index < MAX_STRING_NUMBER;index ++)
    {
        if (edit_data[index ] == 2)
        {
            edit_data[index ] = 1;
            break;
        }
    }
   for (uint8_t i = 0; i < MAX_STRING_NUMBER; i++)
   {
        if (direction==0)
        {
          if (index == 0) index = (MAX_STRING_NUMBER-1);
              else
          index--;
        }
        else
        {
          if (++index >= MAX_STRING_NUMBER) index = 0;
        }
        if (edit_data[index ] == 1)
        {
           edit_data[index ] = 2;
           break;
        }
    }
}



void vSetEdit()
{
    for (u8 i =0; i < MAX_STRING_NUMBER;i ++)
    {
        if (edit_data[i ] == 2)
        {
              edit_data[i] = 3;
              curr_edit_data_id = pMenu[pCurrMenu].pScreenCurObjets[i].DataID;
              vGetData( curr_edit_data_id , 0,CMD_START_EDIT ,0,0);
              break;
        }
    }
    SelectEditFlag = 1;
}

void vSetCommnad( DATA_VIEW_COMMAND_t cmd )
{
    for (u8 i =0; i < MAX_STRING_NUMBER;i ++)
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
                      case 0:
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
                                           menu_mode = 3;
                                           vSetEdit();
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
                               default: ; break;
                           }
                          break;
                     case 3:
                         switch ( TempEvent.KeyCode )
                         {
                            case EXIT_KEY: vSetCommnad(CMD_EXIT_EDIT); break;
                            case ENTER_KEY: vSetCommnad(CMD_SAVE_EDIT); break;
                            case UP_KEY: vGetData( curr_edit_data_id, 0,CMD_DEC,0,0); break;
                            case DOWN_KEY: vGetData( curr_edit_data_id, 0,CMD_INC,0,0); break;
                            case RIGTH_KEY: vGetData( curr_edit_data_id, 0,CMD_PREV_EDIT,0,0); break;
                            case LEFT_KEY: vGetData( curr_edit_data_id, 0,CMD_NEXT_EDIT,0,0); break;
                         }
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
             if (++cur_edit_index >=max_index) cur_edit_index = 0;
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

void vFloatDataEdit( u16 data_id, DATA_VIEW_COMMAND_t command ,u8 max_index , u8 min_index, float max_data, float min_data )
{
    u8 temp_index;
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
                 edit_data_buffer_float = edit_data_buffer_float+ coof_float [temp_index];
             else
                 edit_data_buffer_float = max_data;
             break;
       case CMD_DEC:
            temp_index = cur_edit_index;
            if (temp_index > min_index)  temp_index--;
             if (  (edit_data_buffer_float - min_data) >=  coof_float[temp_index] )
                 edit_data_buffer_float = edit_data_buffer_float-coof_float[temp_index];
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


u16 getDataModelID( u16 MENU_ID)
{
    switch (MENU_ID)
    {
        case SENS_1_RAW_ID:         return (SENS1);
        case SENS_2_RAW_ID:         return (SENS2);
        case COOF_P_ID:             return (COOF_P);
        case COOF_I_ID:             return (COOF_I);
        case COOF_P_1_ID:           return (COOF_P1);
        case COOF_I_1_ID:           return (COOF_I1);
        case COOF_P_2_ID:           return (COOF_P2);
        case COOF_I_2_ID:           return (COOF_I2);
        case COOF_P_3_ID:           return (COOF_P3);
        case COOF_I_3_ID:           return (COOF_I3);
        case VOLTAGE_MIN_ON_ID:     return (LOW_VOLTAGE_ON);
        case VOLTAGE_MIN_OFF_ID:    return (LOW_VOLTAGE_OFF);
        case VOLTAGE_MAX_ON_ID:     return (HIGH_VOLTAGE_ON);
        case VOLTAGE_MAX_OFF_ID:    return (HIGH_VOLTAGE_OFF);
        case CONTROL_MODE_ID:       return (CONTROL_TYPE);
        case PROTOCOL_ID:           return (MB_PROTOCOL_TYPE);
        case IP_ADRESS_DATA_ID:     return (IP_1);
        case IP_GATE_ID:            return (GATE_1);
        case IP_SUBNETMASK_ID:      return (MASK_1);
        case FILTER_HIGH_ID:        return (FILTER_HIGH);
        case FILTER_LOW_ID:         return (FILTER_LOW);
        case SETTING1_ID:           return (SETTING1);
        case SETTING2_ID:           return (SETTING2);
        case KOOFKPS_ID:            return (KOOFKPS);
        case MB_RTU_ADDR_ID:        return (MB_RTU_ADDR);
        case MOD_BUS_TIMEOUT_ID:    return (MOD_BUS_TIMEOUT);
        case CONTRAST_ID:           return (CONTRAST);
        case FAN_START_TIMEOUT_ID:  return (FAN_START_TIMEOUT);
        case AFTER_ZONE_SETTING_ID: return (AFTER_ZONE_SETTING);
        case CDV_CH_COUNT_ID:       return (CDV_BP_CH_COUNT);
        case MEASERING_UNIT_ID:     return (MEASERING_UNIT);
        case OFFSET2_ID:            return (OFFSET_CH2);
        case PRIOR_SENSOR_ID:       return (PRIOR_SENSOR );
        case CLEAN_TIMER_ID:        return (CLEAN_TIMER);
        case ZERO_POINT_TIMEOUT_ID: return (ZERO_POINT_TIMEOUT);
        case KK_SENSOR_TYPE_ID:     return (KK_SENSOR_TYPE);
        case CO2_SENSOR_TYPE_ID:    return (CO2_SENSOR_TYPE);
        case H_SENSOR_TYPE_ID:      return (H_SENSOR_TYPE);
        default: return 0;
    }
}

#define FILTER_ERROR  0x01
#define SETTING_ERROR 0x02
#define LOW_VOLTAGE_ERROR 0x04
#define HIGH_VOLTAGE_ERROR 0x08


static u8 error_shif = 0;
static u8 const *  ErrorString[]={"HEPA Фильтр засорен","Невозможно","Низкое напряжение","Высокое напряжение"};
static u8 const *  ViewErrorString[]={"HEPA Фильтр засорен","Невоз. поддер. устав!","Низкое напряжение","Высокое напряжение"};
static u8 const *  SettingTilteStirnf[]={"1/9 ","1/21 ","1/18 "};
static u8 const *  Setting2TilteStirnf[]={"2/9 ","2/21 ","2/18 "};
static u8 const *  VoltageTilteStirnf[] ={"5/9 ","20/21 ","17/18 "};
static u8 const *  CalTilteStirnf[] ={"7/9 ","21/21 ","20/20 "};

void vSetTitle(u16 data_id, u8 * str, u8 dev_type)
{
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
void vSetCDV_PB(u16 data_id, u8 * str, DATA_VIEW_COMMAND_t command,  u8 * len, u16 reg_id)
{
    switch (data_id)
    {

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
                      vByteDataEdit(0,reg_id,command,0,1,0);
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

    }
}


void vGetData(u16 data_id, u8 * str, DATA_VIEW_COMMAND_t command, u8 * index, u8 * len)
{
    u8 MACAddr[6];
    u8 temp_byte,temp_state = 0;
    int16_t temp_int;
    u16 max,min;
    if (index!=0) *index = cur_edit_index;
    if (len!=0)   *len = 1;
    u16 reg_id = getDataModelID(data_id);
    if (( data_id >=TITLE_FIRST) &&  ( data_id <=TITLE_LAST))
    {
        vSetTitle(data_id,str,getReg8(DEVICE_TYPE));
    }
    else
    if ((data_id>= CDV_BP_FIRST) && ( data_id<=CDV_BP_LAST))
    {
        vSetCDV_PB(data_id,str,command,len,reg_id);
    }
    else
    switch (data_id)
    {

        case JOURNAL_TIME_ID:
            sprintf(str,TimeFormatString,time.hours,time.minutes,time.seconds);
            break;
        case JOURNAL_DATE_ID:
            sprintf(str,DateFormatString,date.date,date.month,date.year);
            break;
        case JOURNAL_INFO1_ID:
             strcpy(str,ErrorString[error_flag]);
            break;
        case JOURNAL_INFO2_ID:
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
             break;
        case JURNAL_RECORD_ID:
            sprintf(str,"%02i/%02i",journal_index+1, getReg16(RECORD_COUNT));
            vGetRecord(journal_index,&error_flag,&time,&date);
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
            if (temp_byte == 0)
                sprintf(str,"");
            else
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
        case COOF_P_1_ID:
        case COOF_I_1_ID:
        case COOF_P_2_ID:
        case COOF_I_2_ID:
        case COOF_P_3_ID:
        case COOF_I_3_ID:
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
                   vFloatDataEdit(reg_id, command,4,2,999.99,-999.99);
                   break;
            }
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
        case SENS_1_RAW_ID:
        case SENS_2_RAW_ID:
            sprintf(str, "%i Па",(int)getAIN(reg_id) );
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
        case SETTING_ID:
            sprintf(str,"%04i м^3/ч", USER_GetSetting());
            break;
        case FACT_RASH_ID:
            temp_int = USER_GetFact(&temp_state);
            if (temp_state)
                sprintf(str,"%04i м^3/ч", temp_int);
            else {
                sprintf(str,"---- м^3/ч");
            }
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
        case MODE_STATE_ID:
            if (getReg8(MODE ))
                strcpy(str,"2 (Доп.)");
            else
                strcpy(str,"1 (Основной)");
            break;
        case MAC_ADRESS_ID:
            WCHNET_GetMacAddr(MACAddr);
            sprintf(str,"%x%x%x%x%x%x",MACAddr[0],MACAddr[1],MACAddr[2],MACAddr[3],MACAddr[4],MACAddr[5]);
            break;
        case PROCESS_STATE_ID:
            switch( USER_GetProccesState() )
            {
                case USER_PROCCES_IDLE:
                case USER_PROCESS_ALARM:
                    strcpy(str,"Остановлен");
                    break;
                case USER_PEOCESS_WORK_TIME_OUT:
                case USER_PEOCESS_ZERO_CALIB:
                    strcpy(str,"Калибровка");
                    break;
                case USER_RROCCES_WORK:
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
                    sprintf(str,"%03i",getReg16(IP_PORT) );
                    break;
                case CMD_EDIT_READ:
                    sprintf(str,"%03i",edit_data_buffer_byte );
                     break;
                default:
                    vByteDataEdit(1,IP_PORT,command,3,999,0);
                    break;
            }
            break;
        case AC_VOLTAGE_ID:
            sprintf(str,"%i В",(uint16_t)getAIN(AC220));
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
                        max = getReg8(getDataModelID(VOLTAGE_MIN_OFF_ID))-1;
                        min = 100;
                    }
                    else
                    if ( data_id == VOLTAGE_MIN_OFF_ID)
                    {
                        max = getReg8(getDataModelID(VOLTAGE_MAX_OFF_ID))-1;
                        min = getReg8(getDataModelID(VOLTAGE_MIN_ON_ID))+1;
                    }
                    else

                    if ( data_id == VOLTAGE_MAX_OFF_ID)
                    {
                        min = getReg8(getDataModelID(VOLTAGE_MIN_OFF_ID))+1;
                        max = getReg8(getDataModelID(VOLTAGE_MAX_ON_ID))-1;
                    }
                    else
                    {
                        min = getReg8(getDataModelID(VOLTAGE_MAX_OFF_ID))+1;
                        max =255;
                    }
                    vByteDataEdit(0,reg_id,command,2,max,min);
                    break;
        }
        break;
    case JOURNAL_COUNT_ID:
        sprintf(str,"%02i",getReg16(RECORD_COUNT) );
        break;
    case ZERO_CALIBRATE_ID:
        switch (command )
        {
            case CMD_EDIT_READ:
                   strcpy(str,"");
                   break;
            case CMD_READ:
                       if ( SelectEditFlag )
                       {
                           if ((USER_GetProccesState() == USER_PROCCES_IDLE))
                               strcpy(str,"Старт?");
                           else
                               strcpy(str,"Заблок");

                       }
                        else
                            strcpy(str,"");
                        break;
                    case CMD_SAVE_EDIT:
                    case CMD_START_EDIT:
                        if ((USER_GetProccesState() == USER_PROCCES_IDLE))
                        {
                         CalibrateZeroStart();

                        }
                        start_edit_flag = 0;
                        menu_mode = 0;
                        SelectEditFlag = 0;
                         break;
                   default:
                       start_edit_flag = 0;
                       menu_mode = 0;
                    break;
                }

        break;

    case JOURNAL_RESET_ID:
        switch (command )
        {
            case CMD_EDIT_READ:
                strcpy(str,"");
                break;
            case CMD_READ:
                if ( SelectEditFlag )
                    strcpy(str,"     Сбросить журнал?   ");
                else
                    strcpy(str,"");
                break;
            case CMD_SAVE_EDIT:
            case CMD_START_EDIT:
                 saveReg16(RECORD_INDEX, 0);
                 saveReg16(RECORD_COUNT, 0);
                 start_edit_flag = 0;
                 menu_mode = 0;
                 SelectEditFlag = 0;
                 break;
           default:
               start_edit_flag = 0;
               menu_mode = 0;
            break;
        }
        break;
    case MB_RTU_ADDR_ID :
    case MOD_BUS_TIMEOUT_ID:
    case CONTRAST_ID:
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
                vByteDataEdit(0,reg_id,command,2,100,1);
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
                 strcpy(str,SENSOR_COUNT_STRING[getReg8(SENSOR_COUNT)]);
                 break;
             case CMD_EDIT_READ:
                 strcpy(str,SENSOR_COUNT_STRING[edit_data_buffer_byte ]);
                 break;
            default:
                vByteDataEdit(0,SENSOR_COUNT,command,0,6,0);
                break;
         }
        break;
    default:
        if (str!=0)
        strcpy(str,"0,0");
        break;
    }
}

static void vDraw( xScreenObjet * pScreenDraw)
{
    MENU_ClearScreen();
    u8 x,y;
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
            //x = pScreenDraw[i].x_data;
            if (( menu_mode >0) && (edit_data[i]>=2))
            {
                if ((edit_data[i]==2) && (SelectEditFlag))
                {
                   if ( blink_counter <= 5)
                   {
                       u8g2_DrawBox( &u8g2, 128-len, y-10 , len , 11 );
                       u8g2_SetDrawColor(&u8g2,0);
                       u8g2_SetFontMode(&u8g2,1);
                   }
                }
                if (edit_data[i]==3)
                {
                    if (box_len == 1)
                       u8g2_DrawBox( &u8g2, 128-6*(edit_index+1), y-10 , 6 , 11 );
                    else
                        u8g2_DrawBox( &u8g2, 128-len, y-10 , len , 11 );
                    u8g2_SetDrawColor(&u8g2,2);
                    u8g2_SetFontMode(&u8g2,1);
                }
            }
            u8g2_DrawUTF8(&u8g2,128-len,y,str);
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
