/*
 * din_dout_task.c
 *
 *  Created on: Aug 3, 2024
 *      Author: i.dymov
 */
#include "din_dout_task.h"
#include "hw_lib_din.h"
#include "hal_gpio.h"
#include "init.h"


static uint8_t  STATUS[KEY_COUNT];
static uint8_t  COUNTERS[KEY_COUNT];
/*
 *
 */
uint8_t fDinStateCallback (uint8_t i)
{
    switch ( i)
    {
        case INPUT_1:
            return HAL_GetBit( DIN_1_5_Port  ,DIN_1_Pin );
        case INPUT_2:
            return HAL_GetBit( DIN_1_5_Port  ,DIN_2_Pin );
        case INPUT_3:
            return HAL_GetBit( DIN_1_5_Port  ,DIN_3_Pin );
        case INPUT_4:
            return HAL_GetBit( DIN_1_5_Port  ,DIN_4_Pin );
        case INPUT_5:
            return HAL_GetBit( DIN_1_5_Port  ,DIN_5_Pin );
        default:
            return (RESET);
    }
}


/*
 *
 */
void  vSetDoutState( OUT_NAME_TYPE ucCh, u8 BitVal )
{
    switch (ucCh )
    {
           case OUT_1:
               if ( BitVal == RESET)
                     HAL_SetBit(DOUT1_Port,DOUT_1_Pin  );
                 else
                     HAL_ResetBit(DOUT1_Port,DOUT_1_Pin );
                break;
           case OUT_2:
               if ( BitVal == RESET)
                   HAL_SetBit(DOUT_Port,DOUT_2_Pin  );
               else
                  HAL_ResetBit(DOUT_Port,DOUT_2_Pin );
                               break;
           case OUT_3:
               if ( BitVal == RESET)
                   HAL_SetBit(DOUT_Port,DOUT_3_Pin  );
               else
                   HAL_ResetBit(DOUT_Port,DOUT_3_Pin );
               break;
           case OUT_4:
              if ( BitVal == RESET)
                   HAL_SetBit(DOUT_Port,DOUT_4_Pin  );
              else
                   HAL_ResetBit(DOUT_Port,DOUT_4_Pin );
              break;
           default:
               break;
       }
}

void vDIN_DOUT_Init()
{
    DoutCinfig_t  DOUT_CONFIG;
    DinConfig_t DIN_CONFIG;
    DIN_CONFIG.eInputType = DIN_CONFIG_NEGATIVE;
    DIN_CONFIG.ulHighCounter = DEF_H_FRONT;
    DIN_CONFIG.ulLowCounter  = DEF_L_FRONT;
    DIN_CONFIG.getPortCallback = &fDinStateCallback;
    eDinConfigWtihStruct(INPUT_1,&DIN_CONFIG);
    eDinConfigWtihStruct(INPUT_2,&DIN_CONFIG);
    eDinConfigWtihStruct(INPUT_3,&DIN_CONFIG);
    eDinConfigWtihStruct(INPUT_4,&DIN_CONFIG);
    eDinConfigWtihStruct(INPUT_5,&DIN_CONFIG);

    DOUT_CONFIG.setPortCallback =&vSetDoutState;
    DOUT_CONFIG.ucActiveLevel = DOUT_ACTIVE_CONFIG_NEGATIVE;
    eDOUTConfigWtihStruct( OUT_1, &DOUT_CONFIG);
    eDOUTConfigWtihStruct( OUT_2, &DOUT_CONFIG);
    eDOUTConfigWtihStruct( OUT_3, &DOUT_CONFIG);
    eDOUTConfigWtihStruct( OUT_4, &DOUT_CONFIG);
}


BitState_t fPortState (uint8_t i)
{
    switch (i)
    {
        case 0:
            return HAL_GetBit( KL_Port, KL1_Pin  );
        case 1:
            return HAL_GetBit( KL_Port, KL2_Pin  );
        case 2:
            return HAL_GetBit( KL_Port, KL3_Pin  );
        case 3:
            return HAL_GetBit( KL_Port, KL4_Pin  );
        case 4:
            return HAL_GetBit( KL_Port, KL5_Pin  );
        case 5:
            return HAL_GetBit( KL6_Port, KL6_Pin  );
        default:
            return 0;
    }
}




void vKeyboarInit()
{
    KeybaordStruct_t KeyboardInit;
    KeyboardInit.KEYBOARD_COUNT    = KEY_COUNT;
    KeyboardInit.COUNTERS          = COUNTERS;
    KeyboardInit.STATUS            = STATUS;
    KeyboardInit.REPEAT_TIME       = 20;
    KeyboardInit.KEYDOWN_HOLD_TIME = 100;
    KeyboardInit.KEYDOWN_DELAY     = 2;
    KeyboardInit.KEYBOARD_PERIOD   = 20;
    KeyboardInit.getPortCallback = &fPortState;
    eKeyboardInit(&KeyboardInit);
}

void vKeyboardTask(void  * argument )
{
    vKeyboarInit();

    for(;;)
    {
       vTaskDelay( HW_LIB_GetKeyboardPeriod() );
       HW_LIB_KeyboradFSM();
       vDinDoutProcess();
    }
}
