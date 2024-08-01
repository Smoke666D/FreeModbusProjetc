/*
 * lib_menu.h
 *
 *  Created on: Jul 16, 2024
 *      Author: i.dymov
 */

#ifndef HW_LIB_LIB_MENU_H_
#define HW_LIB_LIB_MENU_H_

#include "main.h"

/******************************************************************************************************************/
#define   SET_PARAMETR_SCREEN 0U
#define   CENTER_ALIGN        1U
#define   RIGTH_ALIGN         2U
#define   LEFT_ALIGN          3U
#define   NO_ALIGN            0U

#define   LAST_OBJECT         1U
#define MAX_SCREEN_COUNT    3U
#define MAX_SCREEN_OBJECT   25U
#define XRESULURION         128U
#define YRESOLUTION         64U
#define FONT_TYPE           (u8g2_font_6x12_t_cyrillic) // ( u8g2_font_6x13_t_cyrillic )
#define FONT_X_SIZE         6U
#define FONT_TYPE_NUMBER    ( u8g2_font_5x8_mf )
#define FONT_TYPE4          ( u8g2_font_6x13_t_cyrillic )
#define FONT_TYPES          ( u8g2_font_6x13O_tf )
#define FONT_X_SIZE4        6U
#define FONT_TYPE_NUMBER4   ( u8g2_font_5x8_mf )
#define MAX_KEY_PRESS


#define MAX_SCREEN_OBJECT   25U

typedef enum
{
  ICON,
  TEXT_STRING,
  STRING,
  H_LINE,
  NEGATIVE_STRING,
  DATA_STRING,
  INPUT_DATA_STRING,
  HW_DATA,
  INPUT_HW_DATA,
} OBJECT_TYPE;

typedef enum
{
  FLAG_RESET,
  FLAG_SET,
} FLAG;

typedef enum
{
  mREAD,
  mSAVE,
  mESC,
  mINC,
  mDEC,
} DATA_COMMNAD_TYPE;


typedef struct __packed
{
  uint8_t     last;
  uint8_t     x;
  uint8_t     y;
  uint8_t     Width;
  uint8_t     Height;
  OBJECT_TYPE xType;
  uint8_t*    ObjectParamert;
  char*       pStringParametr;
  void        ( *GetDtaFunction )();
  uint16_t    DataID;
} xScreenObjet;

typedef struct __packed
{
  const xScreenObjet*  pScreenCurObjets;
   void*         pUpScreenSet;
   void*         pDownScreenSet;
  uint8_t       pCurrIndex;
  uint8_t       pMaxIndex;
 // void          ( *pFunc )( void*, char );
} xScreenType;

typedef struct __packed
{
  xScreenType*  pHomeMenu;               /* §µ§Ü§Ñ§Ù§Ñ§ä§Ö§Ý§î §ß§Ñ §Þ§Ñ§ã§ã§Ú§Ó §ï§Ü§â§Ñ§ß§à§Ó §Ó§Ö§â§ç§ß§Ö§Ô§à §å§â§à§Ó§ß§ñ. */
  uint8_t       pMaxIndex;               /* §ª§ß§Õ§Ö§Ü§ã §Õ§à§Þ§Ñ§ê§ß§Ö§Ô§à §ï§Ü§â§Ñ§ß§Ñ §Ó §Þ§Ñ§Ñ§ã§Ú§Ó§Ö §ï§Ü§â§Ñ§ß§à§Ó §Ó§Ö§â§ç§ß§Ö§Ô§à §å§â§à§Ó§ß§ñ */
  uint8_t       pCurrIndex;              /* §ª§ß§Õ§Ö§Ü§ã§Ö §ä§Ö§Ü§å§ë§Ö§Ô§à §ï§Ü§â§Ñ§ß§Ñ */
  void          (* pFunc)(void *,char);  /* §¶§å§ß§Ü§è§Ú§ñ §à§Ò§â§Ñ§Ò§à§ä§Ü§Ú §ß§Ñ§Ø§Ñ§ä§Ú§Û */
} xScreenSetObject;

#endif /* HW_LIB_LIB_MENU_H_ */
