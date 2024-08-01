/*
 * hw_lib_keyboard.c
 *
 *  Created on: 14 мая 2024 г.
 *      Author: i.dymov
 */
#include "hw_lib_keyboard.h"

static QueueHandle_t    pKeyboardQueue;
static KeybaordStruct_t Keyboard;


QueueHandle_t * xKeyboardQueue( void )
{
  return  (&pKeyboardQueue);
}



uint16_t HW_LIB_GetKeyboardPeriod()
{
    return Keyboard.KEYBOARD_PERIOD;
}

/*
 * Функция инициаизации драйвера
 */
KEYBOARD_INIT_CODES eKeyboardInit( KeybaordStruct_t * kis )
{
    KEYBOARD_INIT_CODES res = KEYBORAD_PARAM_ERROR;
    Keyboard.KEYBOARD_COUNT      = kis->KEYBOARD_COUNT;
    Keyboard.REPEAT_TIME         = kis->REPEAT_TIME;
    Keyboard.KEYDOWN_HOLD_TIME   = kis->KEYDOWN_HOLD_TIME;
    Keyboard.KEYDOWN_DELAY       = kis->KEYDOWN_DELAY;
    Keyboard.getPortCallback     = kis->getPortCallback;
    Keyboard.KEYBOARD_PERIOD     = kis->KEYBOARD_PERIOD;
    Keyboard.COUNTERS = kis->COUNTERS;
    memset(Keyboard.COUNTERS,0, Keyboard.KEYBOARD_COUNT);
    Keyboard.STATUS   = kis->STATUS;
    memset(Keyboard.COUNTERS,0, Keyboard.KEYBOARD_COUNT);
    res = KEYBOARD_INIT_OK;
    xQueueReset( pKeyboardQueue );
    return (res);
}

static void vPostState( uint8_t key, uint8_t state)
{
    KeyEvent      TEvent;
    TEvent.KeyCode = key;
    TEvent.Status  = state;
    Keyboard.COUNTERS[key]    = 0U;
    xQueueSend( pKeyboardQueue, &TEvent, portMAX_DELAY );
}

void HW_LIB_KeyboradFSM()
{

    for ( uint8_t i = 0U; i < Keyboard.KEYBOARD_COUNT; i++ )                                          /* РЎС‡РёС‚С‹РІР°РµРј С‚РµРєСѓС‰РµРµ СЃРѕСЃС‚РѕСЏРЅРёРµ РїРѕСЂС‚РѕРІ РєР»Р°РІРёР°С‚СѓСЂС‹ */
        {
           if (Keyboard.getPortCallback(i) == KEY_OFF_STATE  )
           {
               if (Keyboard.STATUS[i])
               {
                   Keyboard.STATUS[i]      = KEY_OFF; /*РЎРѕСЃС‚РѕСЏРЅРёРё РєР»Р°РІРёС€Рё Р’Р«РљР›*/
                   vPostState( i , BRAKECODE);
               }
           }
           else
           {
              Keyboard.COUNTERS[i]++;
              if ( Keyboard.STATUS[i] == KEY_OFF )
              {
                     if ( Keyboard.COUNTERS[i] >=  Keyboard.KEYDOWN_DELAY )
                     {
                           Keyboard.STATUS[i]      = KEY_ON;
                           vPostState( i , MAKECODE);
                     }
               }
               else
               {
                   if  ( ( ( Keyboard.COUNTERS[i] >=  Keyboard.KEYDOWN_HOLD_TIME ) && (Keyboard.STATUS[i]==KEY_ON        )) ||
                             ( ( Keyboard.COUNTERS[i] >=  Keyboard.REPEAT_TIME )   && (Keyboard.STATUS[i]==KEY_ON_REPEAT ))     )
                    {
                            Keyboard.STATUS[i]      = KEY_ON_REPEAT;
                            vPostState( i , MAKECODE);
                    }
               }
            }
         }
}
