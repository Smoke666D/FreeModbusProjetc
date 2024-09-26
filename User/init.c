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
#include "hal_dac.h"
#include "hal_i2c.h"
#include "led.h"

static void MX_GPIO_Init(void);

void vInit_DeviceConfig( void )
{
     HAL_I2C_InitTypeDef  I2C_InitTSturcture = {0};
     MX_GPIO_Init();
     RCC_APB1PeriphClockCmd(  RCC_APB1Periph_SPI2, ENABLE );
     vLCDInit(TIMER5);
     ADC1_Init();
     HW_TIMER_TimerInit(TIMER3,1945945,25);
     HW_TIMER_SelectOutTrigger(TIMER3,TIM_TRGOSource_Update);
     HAL_TIMER_PWMTimersInit(TIMER9,60000,3000,TIM_CHANNEL_1 | TIM_CHANNEL_2 | TIM_CHANNEL_3);
     HAL_TIMER_SetPWMPulse(TIMER9,TIM_CHANNEL_1 | TIM_CHANNEL_2 | TIM_CHANNEL_3, 500 );
     HAL_TIMER_EnablePWMCH(TIMER9);
     HAL_TiemrEneblae(TIMER9);
     I2C_InitTSturcture.I2C_ClockSpeed = 200000;

     I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
     I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_16_9;
     I2C_InitTSturcture.I2C_OwnAddress1 = 0;
     I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
     I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
     HAL_I2C_InitIT(I2C_1, &I2C_InitTSturcture,1,1);
     HAL_I2C_InitIT(I2C_2, &I2C_InitTSturcture, 1,1);
     HAL_InitGpioAF (  I2C2_Port , I2C2_SDA_Pin   | I2C2_SCL_Pin  , 0 , GPIO_Mode_AF_OD );
     HAL_InitGpioAF (  I2C1_Port , I2C1_SDA_Pin   | I2C1_SCL_Pin  , 0 , GPIO_Mode_AF_OD );
     HAL_I2C_ENABLE( I2C_1 );
     HAL_I2C_ENABLE( I2C_2 );
     HAL_DAC_InitTypeDef init;
     init.channel = HAL_DAC1;
     init.DAC_Trigger = DAC_Trigger_None;
     init.DAC_WaveGeneration = DAC_WaveGeneration_None;
     init.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
     init.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
     HAL_DAC_Init(&init);
     DAC_Cmd(DAC_Channel_1, ENABLE);
     DAC_SetChannel1Data(DAC_Align_12b_R, 1000);
     vDIN_DOUT_Init();
     InitEEPROM( HAL_SPI2 );

}


static void MX_GPIO_Init(void)
{
    HAL_InitGPO();
    HAL_InitGpioAF( DAC0_Port, DAC0_Pin, 0, GPIO_Mode_AF_OD );
    HAL_InitGpioAIN( AC_SENSE_PORT, AC_SENS1_| AC_SENS2_);
    HAL_InitGpioAIN( AIN5_6_PORT , AIN5_Pin | AIN6_Pin );
    HAL_InitGpioAIN( AIN3_4_PORT , AIN3_Pin | AIN4_Pin );
    HAL_InitGpioAIN( SEN1_POW_AIN1_2_PORT  , POWER_CONTROL | AIN2_Pin | AIN1_Pin );
    HAL_InitGpioOut(  LCDDATA_4_7_Port  ,  LCDDATA4_Pin | LCDDATA5_Pin | LCDDATA6_Pin | LCDDATA7_Pin );
    HAL_InitGpioOut(  LCDDATA_0_1_n_Port , LCDDATA1_Pin | LCDDATA0_Pin );
    HAL_InitGpioOut(  LCDRST_Port , LCDRST_Pin );
    HAL_InitGpioOut(  LDCDATA_2_3_E_REW_CD_LED_Port , LCDDATA3_Pin | LCDDATA2_Pin );
    HAL_InitGpioOut(  LDCDATA_2_3_E_REW_CD_LED_Port , LCDnRW_Pin | LCDCS_Pin | LCDLED_Pin );
    HAL_InitGpioOut(  LDCDATA_2_3_E_REW_CD_LED_Port , LCDnE_Pin );
    HAL_InitGpioOut(  LCDDATA_0_1_n_Port , LCDDni_Pin  );
    HAL_InitGpioAF (  SPI2_Port , SPI2_SCK_Pin   | SPI2_MOSI_Pin  , 0 , GPIO_Mode_AF_PP );
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init( GPIOB, &GPIO_InitStructure);
    HAL_InitGpioOut( SPI2_Port,  SPI2_NSS_Pin);
    HAL_InitGpioOut( CRACH_Port, CRACH_Pin);
    HAL_InitGpioOut( LDCDATA_2_3_E_REW_CD_LED_Port,   LCDLED_Pin);
    HAL_InitGpioAF(  AOUT_Port, AOUT1_Pin   | AOUT2_Pin | AOUT3_Pin , GPIO_FullRemap_TIM9, GPIO_Mode_AF_PP );
    HAL_InitGpioOut( DOUT_Port, DOUT_1_Pin | DOUT_2_Pin | DOUT_3_Pin);
    HAL_InitGpioOut( ELED_Port,ELED1_Pin | ELED2_Pin);
    HAL_InitGpioIn( KL_Port ,KL1_Pin | KL2_Pin |  KL3_Pin | KL4_Pin | KL5_Pin  );
    HAL_InitGpioIn( KL6_Port ,KL6_Pin   );
    HAL_InitGpioIn( DIN_1_5_Port, DIN_1_Pin | DIN_2_Pin |DIN_3_Pin |DIN_4_Pin| DIN_5_Pin    );
    DAC_SetChannel1Data( DAC_Align_12b_R, 0x00 );
    HAL_SetBit(LDCDATA_2_3_E_REW_CD_LED_Port,  LCDLED_Pin);
    HAL_InitGpioOut(RS485_EN_Port,  RS485_EN_Pin);
    HAL_InitGpioAF(  RS485_Port  , RS485_RX_Pin   ,GPIO_FullRemap_USART4 ,   GPIO_Mode_IN_FLOATING );
    HAL_InitGpioAF(  RS485_Port ,  RS485_TX_Pin    ,GPIO_FullRemap_USART4 ,  GPIO_Mode_AF_PP );
    HAL_InitGpioAF(  UART_Port , TX1_Pin   ,0 ,  GPIO_Mode_AF_PP );

}
