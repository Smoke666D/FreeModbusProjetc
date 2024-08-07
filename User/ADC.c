/*
 * ADC.C
 *
 *  Created on: Feb 5, 2024
 *      Author: i.dymov
 */
#include "AC_ConvertLib.h"
#include "system_init.h"
#include "hal_adc.h"
#include "ADC.H"
#include "main.h"
#include "mb_task.h"
#include "hw_lib_adc.h"

static TaskHandle_t pADCTaskHandle;
static TaskHandle_t  pTaskToNotifykHandle;
static uint16_t ADC_VALUE[DC_CHANNEL];
static float adc_float[DC_CHANNEL];
float AC_220_VALUE;
static uint16_t ADC2_Buffer[DC_CONVERSION_NUMBER*DC_CHANNEL];
static uint16_t DC_Buffer[DC_CHANNEL];
static int16_t  ADC1_DMABuffer[AC_CONVERION_NUMBER*ADC_CHANNEL];
static EventGroupHandle_t xADCEventGroupHandle;
uint8_t ADC2_CHANNEL[DC_CHANNEL] = { ADC_CH_0,  ADC_CH_1, ADC_CH_2,ADC_CH_6,ADC_CH_7,ADC_CH_14,ADC_CH_15};
static uint8_t ADC_2_FSM  = 0;
static uint8_t DC_cnversion =0;
#define ADC1_CH_COUNT 2
#define ADC1_PRIOR 2
#define ADC1_SUB_PRIOR 0
/*
 *
 */

/*
 *
 */





TaskHandle_t * getADCTaskHandle()
{
    return (&pADCTaskHandle);
}

static void ADC1_Event()
{
    static portBASE_TYPE xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyIndexedFromISR(pADCTaskHandle, 2, ADC1_DATA_READY, eSetValueWithoutOverwrite, &xHigherPriorityTaskWoken  );
   // xEventGroupSetBitsFromISR( xADCEventGroupHandle, ADC1_DATA_READY, &xHigherPriorityTaskWoken );
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

volatile u16 ADC_val[4];
static u8 conversion_number;
static u8 conversion_channel;

#define MIN_BAR_VALUE -0.025
#define MAX_BAR_VALUE  0.025
#define MIN_VDD_VALUE  0.2
#define MAX_VDD_VALUE  2.7
#define VDD_RANGE    (MAX_VDD_VALUE - MIN_VDD_VALUE)

float Get_025HPDPN33_SensorData( float data)
{
    if ( (data - MIN_VDD_VALUE) >- (VDD_RANGE/2))
    {
        return (data - MIN_VDD_VALUE - VDD_RANGE/2)* (MAX_BAR_VALUE/(VDD_RANGE/2));
    }
    else
    {
        return (VDD_RANGE/2 - (data - MIN_VDD_VALUE))*(MAX_BAR_VALUE/(VDD_RANGE/2))*-1;
    }

}
u8 i2cdata[2][8] ={0};

#define DELTA  ((MAX_VDD_VALUE -MIN_VDD_VALUE )/(MIN_BAR_VALUE -  MAX_BAR_VALUE))

float getAIN( AIN_CHANNEL_t channel)
{
    uint32_t temp_data = 0;
    switch (channel)
    {
        case  SENS1:
            return adc_float[0]*KK;//Get_025HPDPN33_SensorData( adc_float[0]*KK);
        case SENS2:
            return adc_float[1]*KK;//Get_025HPDPN33_SensorData( adc_float[1]*KK);
        case DC24:
            return adc_float[2]*KK*COOF_24V;
        case DCAIN1:
            return adc_float[3]*KK;
        case DCAIN2:
            return adc_float[4]*KK;
        case DCAIN3:
            return adc_float[5]*KK;
       case DCAIN4:
            return adc_float[6]*KK;
       case  DIG_TEMP:
             temp_data = (i2cdata[1][3]<<8 | i2cdata[1][4]);
             if (temp_data & 0x8000)
                 return  (float)((temp_data - 0x8000)/256.0);
             else


             return  (float)(temp_data/256.0);
       case DIG_PRES:
           temp_data = (u32)i2cdata[1][0]<<16 | (u32)i2cdata[1][1]<<8 | i2cdata[1][2];
           if (temp_data & 0x800000)
               return (float)(temp_data - 0x800000)/(float)GetSensCoof();
           else
            return (float)( temp_data/(float)GetSensCoof());
        case AC220:
            return AC_220_VALUE;
    }


}


static void ADC2_Event()
{
    u8 index =  conversion_number*DC_CHANNEL + conversion_channel;

    ADC2_Buffer[index ] = Get_ConversionVal(ADC_2,HAL_ADC_DATA(ADC_2));
    if (conversion_number >= DC_CONVERSION_NUMBER)
    {
        conversion_number = 0;
        conversion_channel = 0;
        static portBASE_TYPE xHigherPriorityTaskWoken;
        xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(pADCTaskHandle, ADC2_DATA_READY, eSetValueWithoutOverwrite, &xHigherPriorityTaskWoken  );
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
        return;
    }
    if (++conversion_channel  >= DC_CHANNEL)
    {
        conversion_channel  = 0;
        conversion_number++;
    }
    ADC_RegularChannelConfig(ADC2,   ADC2_CHANNEL[conversion_channel], 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(ADC2, ENABLE);
}

#define DAC_CAL_POINT 10

POINT_t DACCAL[DAC_CAL_POINT]={ {0,0},
                   {10,1.126},
                   {20,2.273},
                   {30,3.420},
                   {40,4.567},
                   {50,5.714},
                   {60,6.85},
                   {70,8.0},
                   {80,9.14},
                   {90,10.29}
};

void ADC1_Init()
{
    vDacInit();
    eDacCalDataConfig(DAC1,DAC_CAL_POINT);
    eSetDacCalPoint(DAC1,  DACCAL, DAC_CAL_POINT );
    eDacCalDataConfig(DAC2,DAC_CAL_POINT);
    eSetDacCalPoint(DAC2,  DACCAL, DAC_CAL_POINT );
    eDacCalDataConfig(DAC3,DAC_CAL_POINT);



    eSetDacCalPoint(DAC3,  DACCAL, DAC_CAL_POINT );

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB1Periph_I2C2,ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB1Periph_I2C2,DISABLE);
    RCC_APB2PeriphResetCmd(RCC_APB1Periph_I2C1,ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB1Periph_I2C1,DISABLE);
    I2C_InitTypeDef  I2C_InitTSturcture = {0};
    I2C_InitTSturcture.I2C_ClockSpeed = 120000;
    I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
    I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2C_InitTSturcture.I2C_OwnAddress1 = 0;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &I2C_InitTSturcture);
    I2C_Init(I2C2, &I2C_InitTSturcture);
    I2C_Cmd(I2C1, ENABLE);
    I2C_Cmd(I2C2, ENABLE);
  //  I2C_GenerateSTOP(I2C1, ENABLE);
   // I2C_GenerateSTOP(I2C2, ENABLE);
   /* DMA_INIT_t init;
    uint8_t ADC1_CHANNEL[ADC1_CH_COUNT] = { ADC_CH_8,  ADC_CH_9 };
    HAL_ADC_ContiniusScanTrigCinvertionDMA( ADC_1, ADC1_CH_COUNT, ADC1_CHANNEL, ADC_ExternalTrigConv_Ext_IT11_TIM8_TRGO);
    init.stream = DMA1_CH1;
    init.direction = PTOM;
    init.mode  = DMA_Circular;
    init.paddr = (u32)&ADC1->RDATAR;
    init.memadr = (u32)ADC1_DMABuffer;
    init.dma_size = DMA_HWORD;
    init.bufsize = AC_CONVERION_NUMBER*2;
    init.prioroty = dma_Medium;
    HAL_DMAInitIT(init,  ADC1_PRIOR , ADC1_SUB_PRIOR, &ADC1_Event  );
    HAL_DMA_Enable(DMA1_CH1);*/
    HAL_ADC_InitIT(ADC_2, ADC_ExternalTrigConv_None , 1, 0, &ADC2_Event );
    ADC_RegularChannelConfig(ADC2, ADC2_CHANNEL[0], 1, ADC_SampleTime_239Cycles5);
    ADC_TempSensorVrefintCmd(ENABLE);
}


void SetI2CData(I2C_TypeDef * i2c,u8 ad, u8 data)
{
    while(I2C_GetFlagStatus( i2c, I2C_FLAG_BUSY) != RESET)
        ;
    I2C_GenerateSTART(I2C2, ENABLE);

    while(!I2C_CheckEvent( i2c, I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress( i2c, 0x6D<<1, I2C_Direction_Transmitter);

    while(!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;


    I2C_SendData(i2c, (u8)(ad & 0x00FF));
    while(!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;



    if(I2C_GetFlagStatus(i2c, I2C_FLAG_TXE) != RESET)
    {
        I2C_SendData(i2c, data);
    }

    while(!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C_GenerateSTOP(i2c, ENABLE);
}


uint8_t GetI2CData(I2C_TypeDef * i2c,u8 ad)
{
    u8 temp;
    while( I2C_GetFlagStatus( i2c, I2C_FLAG_BUSY ) != RESET );

    I2C_GenerateSTART(i2c, ENABLE);

    while( !I2C_CheckEvent( i2c, I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( i2c, 0x6D<<1, I2C_Direction_Transmitter );

    while( !I2C_CheckEvent( i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );

    I2C_SendData(i2c, (u8)(ad & 0x00FF));
    while(!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTART(i2c, ENABLE);

    while(!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress(i2c, 0x6D<<1, I2C_Direction_Receiver);

    while(!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        ;
    while(I2C_GetFlagStatus(i2c, I2C_FLAG_RXNE) == RESET)
        I2C_AcknowledgeConfig(i2c, DISABLE);

    temp = I2C_ReceiveData(i2c);
    I2C_GenerateSTOP(i2c, ENABLE);
    return temp;
}

u8 adress[8]= {0x06,0x07,0x08,0x09,0x0A,0x30,0xA5,0xA6};

void ADC_task(void *pvParameters)
{
    uint32_t ulNotifiedValue;
    TaskFSM_t ADC_TASK_FSM = STATE_INIT;

    int16_t iMax =0;
    uint16_t DF1;
    uint16_t old= 0;
    uint16_t uCurPeriod = AC_CONVERION_NUMBER-1;

    for (;;)
    {
        vTaskDelay(10);
            switch (ADC_TASK_FSM)
            {
                case STATE_INIT:
                    //TIM_Cmd(TIM8, ENABLE);
                    ADC_SoftwareStartConvCmd(ADC2, ENABLE);
                    ADC_TASK_FSM = STATE_WHAIT_TO_RAEDY;
                    break;
                case STATE_WHAIT_TO_RAEDY:
                    ADC_TASK_FSM = STATE_RUN;
                    SetI2CData(I2C2,0x30,0x1B);
                    break;
                case STATE_RUN:

                    xTaskNotifyWait( 0,  ADC2_DATA_READY, &ulNotifiedValue,portMAX_DELAY );
                  /*  bits =  xEventGroupWaitBits(xADCEventGroupHandle,ADC1_DATA_READY | ADC2_DATA_READY, pdTRUE, pdFALSE, portMAX_DELAY );
                    if (ulNotifiedValue & ADC1_DATA_READY)
                    {
                        printf( "ADC1 DMA\r\n");
                        xEventGroupClearBits(xADCEventGroupHandle, ADC1_DATA_READY);
                        for (int i = 0; i<AC_CONVERION_NUMBER*ADC_CHANNEL;i++)
                        {
                            ADC1_DMABuffer[i]= Get_ConversionVal(ADC_1, ADC1_DMABuffer[i]);
                        }
                        vDecNetural(&ADC1_DMABuffer[0]);
                        iMax=xADCMax((int16_t *)&ADC1_DMABuffer[0], &DF1);
                        vADCFindFreq((int16_t *)&ADC1_DMABuffer[0], &uCurPeriod,2,iMax);
                        if ((uCurPeriod < AC_CONVERION_NUMBER-1) && (uCurPeriod!=0) )
                        {
                              uint16_t data1 = xADCRMS(&ADC1_DMABuffer[0],uCurPeriod,2);
                              data1 = vRCFilter(data1, &old);
                              AC_220_VALUE = (float)data1 * ( 401U * 3.3 / 4095U );
                                             //int8SetRegister(V220,(uint8_t)data1 );
                         }
                        ADC_SoftwareStartConvCmd(ADC2, ENABLE);
                   }*/
                   if (ulNotifiedValue & ADC2_DATA_READY)
                   {
                       for (u8 i=0;i<DC_CHANNEL;i++)
                       {
                           u32 adc_buffer_data = 0;
                           for (u8 k=0;k <DC_CONVERSION_NUMBER;k++)
                           {
                               adc_buffer_data= adc_buffer_data + ADC2_Buffer[k*DC_CHANNEL + i];
                           }
                           ADC_VALUE[i] = adc_buffer_data/DC_CONVERSION_NUMBER;
                           adc_float[i] = ADC_VALUE[i];
                       }
                       for (u8 i = 0; i<8; i++)
                       {
                          //i2cdata[0][i] =  GetI2CData(I2C1,adress[i]);
                         // vTaskDelay(1);
                           i2cdata[1][i] =  GetI2CData(I2C2,adress[i]);
                           vTaskDelay(1);
                       }

                       ADC_SoftwareStartConvCmd(ADC2, ENABLE);
                   }
                   break;
            }
    }
}







