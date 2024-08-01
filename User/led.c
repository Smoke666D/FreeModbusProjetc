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

static TaskHandle_t LCDTaskHandle;
static u8g2_t             u8g2                             = { 0U };
static uint8_t            LCD_Buffer[LCD_DATA_BUFFER_SIZE] = { 0U };

typedef struct
{
    vu16 LCD_REG;
    vu16 LCD_RAM;
} LCD_TypeDef;

/* A17, Bank1, sector1 */
#define LCD_C1_BASE        ((u32)(0x60000000 | 0x0003FFFE))
#define LCD_C2_BASE        ((u32)(0x70000000 | 0x0003FFFE))


#define LCD_C1             ((LCD_TypeDef *) LCD_C1_BASE)
#define LCD_C2             ((LCD_TypeDef *) LCD_C2_BASE)

static u8 DmaCS  =0;

#define LCD_DATA_SEND  0x02

static void vRedrawLCD(void );


TaskHandle_t * getLCDTaskHandle()
{
    return (&LCDTaskHandle);
}



void LCD_task(void *pvParameters)
{
    TaskFSM_t LCD_Task_FSM = STATE_INIT;
    while (1)
    {
        switch ( LCD_Task_FSM  )
        {
            case STATE_INIT:
                vDispalayON();
                LCD_Task_FSM  = STATE_WHAIT_TO_RAEDY;
                break;
            case STATE_WHAIT_TO_RAEDY:
                LCD_Task_FSM  = STATE_RUN;
                break;
            case STATE_RUN:
                vTaskDelay(100);
                vRedrawLCD();
                break;
        }
    }


}
void callback()
{
    if ( DmaCS == 0 )
    {
        HAL_DMA_Disable(DMA1_CH3);
        HAL_DMAMem2MemStart(DMA1_CH3,LCD_DATA_BUFFER_SIZE/2,  (u32) LCD_Buffer[LCD_DATA_BUFFER_SIZE/2], LCD_C2->LCD_RAM );
        DmaCS = 1;
    }
    else
    {
        HAL_DMA_Disable(DMA1_CH3);
        static portBASE_TYPE xHigherPriorityTaskWoken;
        xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyIndexedFromISR(LCDTaskHandle, 2, LCD_DATA_SEND, eSetValueWithOverwrite, &xHigherPriorityTaskWoken  );
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
        DmaCS = 0;

    }
}


static void vRedrawLCD(void )
{
    uint32_t ulNotifiedValue;
    for (u16 i = 0; i < 64;i++)
    {
        memcpy(&LCD_Buffer[i*8],u8g2.tile_buf_ptr[i*16],8 );
        memcpy(&LCD_Buffer[LCD_DATA_BUFFER_SIZE/2+ i*8],u8g2.tile_buf_ptr[i*16+8],8 );
    }
    HAL_DMAMem2MemStart(DMA1_CH3,LCD_DATA_BUFFER_SIZE/2,  (u32) LCD_Buffer, LCD_C1->LCD_RAM );
    xTaskNotifyWaitIndexed(2, 0, LCD_DATA_SEND, &ulNotifiedValue,portMAX_DELAY );
}

void vLCDInit()
{

    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
    FSMC_NORSRAMTimingInitTypeDef  writeTiming;

    readWriteTiming.FSMC_AddressSetupTime = 0x40;
    readWriteTiming.FSMC_AddressHoldTime = 0x00;
    readWriteTiming.FSMC_DataSetupTime = 0x40;
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_B;

    writeTiming.FSMC_AddressSetupTime = 0x40;
    writeTiming.FSMC_AddressHoldTime = 0x00;
    writeTiming.FSMC_DataSetupTime = 0x40;
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_B;

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;

    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
        FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
        FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
        FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
        FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
        FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;

        FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

        FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);

    HAL_SetBit(LCDRST_Port,LCDRST_Pin);
    u8g2_Setup_ks0108_128x64_f(&u8g2, U8G2_R0, 0U, 0U );
    HAL_DMA1InitMemToMemHwordIT(DMA1_CH3,dma_High,1,5,&callback);
    memset(LCD_Buffer,0xFF,LCD_DATA_BUFFER_SIZE);

}

void vDispalayON()
{
    LCD_C1->LCD_REG = 0x3F;
    LCD_C1->LCD_REG = 0x40;
    LCD_C2->LCD_REG = 0x3F;
    LCD_C2->LCD_REG = 0x40;
}

void vSetADDR( u8 addr)
{

}

void vSetPage( u8 page)
{

}

void vDispalyStartLine( u8 line)
{

}
u8 vStatusRead()
{
    return 0;
}
void WriteDispalay( u8 * data, u8 size)
{

}

void ReadDispalay( u8 * data, u8 size)
{

}
