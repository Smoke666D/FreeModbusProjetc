/*
 * lib_menu.c
 *
 *  Created on: Jul 16, 2024
 *      Author: i.dymov
 */

#include "lib_menu.h"
#include "u8g2.h"
/**********************���֧�֧ާ֧ߧߧ��************************************************************************/
static xScreenObjet*     pCurDrawScreen   = NULL;
static u8g2_t*           u8g2             = NULL;
static uint8_t           Blink            = 0U;

void vDrawObject( xScreenObjet * pScreenObjects)
{
  char* TEXT      = NULL;
  uint8_t sTEXT[2]={0U};
  uint8_t  Insert    = 0U;
  char  Text[40U] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ',' ',' ',' '};
  uint8_t  i         = 0U;
  uint8_t  x_offset  = 0U;
  uint8_t  y_offset  = 0U;
  uint8_t  Redraw    = 0U;

  //�����ӧ֧��֧�, �ܧѧܧ�� ��ܧ�ѧ� �ߧ֧�ҧ��էڧާ� ��֧�֧�ڧ��ӧѧ��
  if ( pCurDrawScreen != pScreenObjects )  //����ݧ� ��ܧ�ѧ� �ڧ٧ާ֧ߧڧݧ��
  {
    pCurDrawScreen = pScreenObjects;
    Redraw         = 1U;
  }
  else                     //����ݧ� ���� �ا� ��ѧާ�� ��ܧ�ѧ�
  {
    for ( i=0U; i<MAX_SCREEN_OBJECT; i++ ) //�����ӧ֧��֧� �֧��� �ݧ� �ߧ� ��ܧ�ѧߧ� �էڧߧѧާڧ�֧�ܧڧ� ��ҧ�֧ܧ��
    {
      switch ( pScreenObjects[i].xType )
      {
        case HW_DATA:
        case INPUT_HW_DATA:
        case DATA_STRING:
          Redraw = 1U;
          break;
        default:
          break;
      }
      if ( ( pScreenObjects[i].last > 0U ) || ( Redraw != 0U ) )
      {
        break;
      }
    }
  }
  if ( Redraw > 0U )   //����ݧ� ��ܧ�ѧ� �ߧ�اߧ� ��֧�֧�ڧ��ӧ�ӧѧ��
  {
    u8g2_ClearBuffer( u8g2 );
    for ( i=0U; i<MAX_SCREEN_OBJECT; i++ )
    {
      Insert = 0U;
      switch ( pScreenObjects[i].xType )
      {
        //����ݧ� ��֧ܧ��ڧ� ��ҧ�֧ܧ� - �����ܧ�
        case H_LINE:
          u8g2_SetDrawColor( u8g2, pScreenObjects[i].ObjectParamert[1U] );
          u8g2_DrawLine( u8g2, pScreenObjects[i].x, pScreenObjects[i].y, pScreenObjects[i].Width, pScreenObjects[i].Height );
          break;
        case STRING:
          break;
        case INPUT_HW_DATA:
        case HW_DATA:
        case TEXT_STRING:
        case DATA_STRING:
          if ( ( pScreenObjects[i].xType == INPUT_HW_DATA ) || ( pScreenObjects[i].xType ==DATA_STRING ) )
          {
            if ( pScreenObjects[i].ObjectParamert[3U] > 0U )
            {
              Insert = 1U;
              Blink = ( Blink > 0U ) ? 0U : 1U;
            }
            if ( !Insert )
            {
              u8g2_SetDrawColor( u8g2, pScreenObjects[i].ObjectParamert[1U]?0U:1U );
            }
            else
            {
              u8g2_SetDrawColor( u8g2, Blink?0U:1U );
            }
            u8g2_DrawBox( u8g2, pScreenObjects[i].x, pScreenObjects[i].y, pScreenObjects[i].Width, pScreenObjects[i].Height );
          }
          u8g2_SetFontMode( u8g2, pScreenObjects[i].ObjectParamert[0U] );
          if ( !Insert )
          {
            u8g2_SetDrawColor( u8g2, pScreenObjects[i].ObjectParamert[1U] );
          }
          else
          {
            u8g2_SetDrawColor( u8g2, Blink );
          }
          switch ( pScreenObjects[i].xType )
          {
            case STRING:
              break;
            case TEXT_STRING:
            case DATA_STRING:
              TEXT = pScreenObjects[i].pStringParametr;
              break;
            case HW_DATA:
            case INPUT_HW_DATA:
              ( pScreenObjects[i].DataID > 0U ) ? pScreenObjects[i].GetDtaFunction( mREAD, &Text, pScreenObjects[i].DataID ) : pScreenObjects[i].GetDtaFunction( mREAD, &Text );
              TEXT = (char*)Text;
              break;
            default:
              break;
          }
          u8g2_SetFont( u8g2, FONT_TYPE4 );
          if ( pScreenObjects[i].xType != TEXT_STRING )
          {
            switch ( pScreenObjects[i].ObjectParamert[2U] )
            {
              case RIGTH_ALIGN:
                x_offset =  pScreenObjects[i].x + ( pScreenObjects[i].Width - u8g2_GetUTF8Width( u8g2, TEXT ) ) - 1U;
                break;
              case LEFT_ALIGN:
                x_offset =  pScreenObjects[i].x + 1U;
                break;
              default:
                x_offset =  pScreenObjects[i].x + ( pScreenObjects[i].Width - u8g2_GetUTF8Width( u8g2, TEXT ) ) / 2U;
                break;
            }
            y_offset =  pScreenObjects[i].y + ( pScreenObjects[i].Height / 2U ) + ( u8g2_GetAscent( u8g2 ) / 2U );
            u8g2_DrawUTF8( u8g2,x_offset, y_offset, TEXT );
          }
          else
          {
            if (TEXT[0]!='$')
            {
              u8g2_DrawUTF8( u8g2, pScreenObjects[i].x, pScreenObjects[i]. y, TEXT );
            }
            else
            {

              u8g2_SetFont( u8g2, FONT_TYPES );
              sTEXT[0] =GetNumber(TEXT[1])*100+GetNumber(TEXT[2])*10+GetNumber(TEXT[3]);
              u8g2_DrawStr( u8g2, pScreenObjects[i].x, pScreenObjects[i].y, ( const char* )sTEXT );
            }
          }
          break;
        default:
           break;
      }
      if ( pScreenObjects[i].last == LAST_OBJECT )
      {
          break;
      }
    }
  }
  return;
}
