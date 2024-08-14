/*
 * led.c
 *
 *  Created on: Jul 15, 2024
 *      Author: i.dymov
 */

#include "led.h"
#include "hal_dma.h"
#include "init.h"
#include "hal_gpio.h"
#include "hal_timers.h"
#include "menu.h"
#define FONT_TYPE4          ( u8g2_font_6x13_t_cyrillic )


static TaskHandle_t LCDTaskHandle;
u8g2_t             u8g2                             = { 0U };


/* A17, Bank1, sector1 */


#define RESET_ENABLE HAL_ResetBit( LDCDATA_2_3_E_REW_CD_LED_Port , LCDnE_Pin )
#define SET_ENABLE   HAL_SetBit( LDCDATA_2_3_E_REW_CD_LED_Port , LCDnE_Pin )
#define SEND_COMMAND HAL_ResetBit(  LCDDATA_0_1_n_Port , LCDDni_Pin  )
#define SEND_DATA    HAL_SetBit(  LCDDATA_0_1_n_Port , LCDDni_Pin  )
#define READ_DATA   HAL_SetBit( LDCDATA_2_3_E_REW_CD_LED_Port,LCDnRW_Pin )
#define WRITE_DATA   HAL_ResetBit( LDCDATA_2_3_E_REW_CD_LED_Port,LCDnRW_Pin )
#define CS1_ENABLE   HAL_ResetBit( LDCDATA_2_3_E_REW_CD_LED_Port,LCDCS_Pin )
#define CS2_ENABLE   HAL_SetBit( LDCDATA_2_3_E_REW_CD_LED_Port,LCDCS_Pin )


#define LCD_DATA_SEND  0x02
void WriteCommand( u8 command );
void WriteDispalay( u8 * data, u16 size);




TaskHandle_t * getLCDTaskHandle()
{
    return (&LCDTaskHandle);
}

#define SCREEN_BUFFER_SIZE  1024
static u8 screen_buufer[SCREEN_BUFFER_SIZE ];



void LCD_task(void *pvParameters)
{
    TaskFSM_t LCD_Task_FSM = STATE_INIT;
    while (1)
    {
        switch ( LCD_Task_FSM  )
        {
            case STATE_INIT:
                RESET_ENABLE;
                CS1_ENABLE;
                HAL_ResetBit(LCDRST_Port,LCDRST_Pin);
                vTaskDelay(10);
                HAL_SetBit(LCDRST_Port,LCDRST_Pin);
                vTaskDelay(10);
                CS2_ENABLE;
                HAL_ResetBit(LCDRST_Port,LCDRST_Pin);
                vTaskDelay(10);
                HAL_SetBit(LCDRST_Port,LCDRST_Pin);
                vTaskDelay(10);
                CS1_ENABLE;
                WriteCommand(0x3F);
                vTaskDelay(1);
                CS2_ENABLE;
                WriteCommand(0x3F);
                vTaskDelay(1);
                //memset(screen_buufer,0,SCREEN_BUFFER_SIZE);

                LCD_Task_FSM  = STATE_WHAIT_TO_RAEDY;
                break;
            case STATE_WHAIT_TO_RAEDY:
                LCD_Task_FSM  = STATE_RUN;
                break;
            case STATE_RUN:
                ulTaskNotifyTakeIndexed( 0, pdTRUE, portMAX_DELAY );
                memcpy(screen_buufer,u8g2.tile_buf_ptr,SCREEN_BUFFER_SIZE );
                for (u8 i =0;i<8;i++)
                {
                     CS1_ENABLE;
                     WriteCommand( 0x040  );
                     WriteCommand( 0x0b8 | i );
                     CS2_ENABLE;
                     WriteCommand( 0x040  );
                     WriteCommand( 0x0b8 | i );
                     WriteDispalay(&screen_buufer[i*128],128);
                }

               break;
       }
    }


}


uint8_t led_sycle;


typedef struct
{
   u8 command;
   u8 * data;
   u16 size;
   u16 index;
   u16 tile;
} LCD_DATA_t;

LCD_DATA_t lcd_data;
TaskHandle_t NotifyTaskHeandle;

void vSetData( u8 data)
{
  u16 bufdata = GPIO_ReadOutputData(GPIOD) & 0x3FFC;
  bufdata|= ((u16)(data & 0x03))<<14 | ((data & 0x0C)>>2);
  GPIO_Write(GPIOD ,bufdata);
  bufdata =  GPIO_ReadOutputData(GPIOE) & 0xF87F;
  bufdata = bufdata | ((data & 0xF0)<<3);
  GPIO_Write(GPIOE,bufdata);
}

void LEDCall()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    switch (led_sycle)
    {
        case 0:
            WRITE_DATA;
            if (lcd_data.command)
            {
                SEND_COMMAND;
                vSetData ( lcd_data.command);
                lcd_data.size = 0;
            }
            else
            {
                SEND_DATA;
                if (lcd_data.tile < 64)
                    CS2_ENABLE;
                else {
                    CS1_ENABLE;
                }
                vSetData( lcd_data.data[lcd_data.index]) ;
            }
            break;
        case 1:
            SET_ENABLE;
            break;
        case 2:
            RESET_ENABLE;
            break;
        case 3:
            if (++lcd_data.tile>=128)
            {
                lcd_data.tile = 0;
            }
            if (++lcd_data.index >= lcd_data.size)
             {
                  HAL_TiemrDisable(TIMER5);
                  xTaskNotifyIndexedFromISR(LCDTaskHandle, 2, 0x01, eSetValueWithOverwrite, &xHigherPriorityTaskWoken  );
                  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
             }
            break;

    }
   if (++led_sycle>3) led_sycle = 0;
}

void vLCDInit()
{
    HAL_TIMER_InitIt(TIMER5,300000,1,&LEDCall,0,1);
    HAL_ResetBit( LDCDATA_2_3_E_REW_CD_LED_Port , LCDnRW_Pin);
    u8g2_Setup_ks0108_128x64_f(&u8g2, U8G2_R0, 0U, 0U );
    u8g2_SetFont( &u8g2, u8g2_font_6x13_t_cyrillic);
    u8g2_ClearBuffer(&u8g2);
}



void WriteDispalay( u8 * data, u16 size)
{
    lcd_data.command = 0;
    lcd_data.index = 0;
    lcd_data.data = data;
    lcd_data.size = size;
    lcd_data.tile =0;
    HAL_TiemrEneblae(TIMER5);
    NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
    ulTaskNotifyTakeIndexed(2, 0xFFFFFFFF, portMAX_DELAY );
}


void WriteCommand( u8 command )
{
    lcd_data.command = command;
    HAL_TiemrEneblae(TIMER5);
    NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
    ulTaskNotifyTakeIndexed(2, 0xFFFFFFFF, portMAX_DELAY );

}
