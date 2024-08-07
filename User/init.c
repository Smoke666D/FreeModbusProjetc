/*
 * init.c
 *
 *  Created on: Jul 8, 2024
 *      Author: i.dymov
 */
#include "init.h"
#include "adc.h"
#include "hal_gpio.h"
#include "hal_timers.h"
#include "EEPROM_25C.h"
#include "din_dout_task.h"




static void MX_GPIO_Init(void);

void vInit_DeviceConfig( void )
{


     MX_GPIO_Init();
     RCC_APB1PeriphClockCmd(  RCC_APB1Periph_SPI2, ENABLE );
   // vLCDInit();
    ADC1_Init();
    // InitEEPROM(HAL_SPI2,DMA1_CH4,DMA1_CH5);
   //  HW_TIMER_TimerInit(TIMER8,4000000,10);
    // HW_TIMER_SelectOutTrigger(TIMER8,TIM_TRGOSource_Update);


 //    SPI_SSOutputCmd( SPI2, ENABLE );
     HAL_TIMER_PWMTimersInit(TIMER9,20000,1000,TIM_CHANNEL_1 | TIM_CHANNEL_2 | TIM_CHANNEL_3);

     HAL_TIMER_SetPWMPulse(TIMER9,TIM_CHANNEL_1 | TIM_CHANNEL_2 | TIM_CHANNEL_3, 500 );
     HAL_TIMER_EnablePWMCH(TIMER9);
     HAL_TiemrEneblae(TIMER9);


      vDIN_DOUT_Init();
}


static void MX_GPIO_Init(void)
{
    HAL_InitGPO();
  //  HAL_InitGpioAF(  LCDDATA_4_7_Port  , LCDDATA4_Pin   | LCDDATA5_Pin | LCDDATA6_Pin | LCDDATA7_Pin , 0, GPIO_Mode_AF_PP);
  //  HAL_InitGpioAF(  LCDDATA_0_1_n_Port , LCDDATA1_Pin     | LCDDATA0_Pin| LCDDni_Pin  , 0, GPIO_Mode_AF_PP);
  //  HAL_InitGpioAF(  LCDRST_Port , LCDRST_Pin , 0, GPIO_Mode_AF_PP);
  //  HAL_InitGpioAF(  LDCDATA_2_3_E_REW_CD_LED_Port , LCDCS_Pin     | LCDLED_Pin| LCDnRW_Pin | LCDnE_Pin | LCDDATA3_Pin |LCDDATA2_Pin  , 0, GPIO_Mode_AF_PP);
    HAL_InitGpioAF( I2C1_Port , I2C1_SDA_Pin | I2C1_SCL_Pin   , 0 , GPIO_Mode_AF_OD );
    HAL_InitGpioAF( I2C2_Port , I2C2_SDA_Pin | I2C2_SCL_Pin   , 0 , GPIO_Mode_AF_OD );
    HAL_InitGpioAF(  SPI2_Port , SPI2_SCK_Pin   | SPI2_MOSI_Pin , 0, GPIO_Mode_AF_PP);
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   // HAL_InitGpioIn(  SPI2_Port , SPI2_MISO_Pin);
    HAL_InitGpioOut(SPI2_Port,   SPI2_NSS_Pin);
    HAL_InitGpioAF(  AOUT_Port , AOUT1_Pin   | AOUT2_Pin | AOUT3_Pin  ,GPIO_FullRemap_TIM9, GPIO_Mode_AF_PP );
    HAL_InitGpioOut(DOUT_Port,DOUT_1_Pin | DOUT_2_Pin | DOUT_3_Pin);
    HAL_InitGpioOut(ELED_Port,ELED1_Pin | ELED2_Pin);
    HAL_InitGpioIn( KL_Port ,KL1_Pin | KL2_Pin | KL3_Pin |  KL1_Pin | KL2_Pin | KL3_Pin );
    HAL_InitGpioIn(DIN_1_5_Port, DIN_1_Pin | DIN_2_Pin |DIN_3_Pin |DIN_4_Pin| DIN_5_Pin    );


 //  HAL_InitGpioAF(GPIOA,GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6,GPIO_Remap_ADC1_ETRGREG,GPIO_Mode_AIN);

}
