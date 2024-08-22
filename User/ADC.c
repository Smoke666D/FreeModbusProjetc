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
#include "hal_timers.h"
#include "hal_gpio.h"
#include "hal_i2c.h"

static TaskHandle_t pADCTaskHandle;
static TaskHandle_t pI2CTaskHandle;
float AC_220_VALUE;
static uint16_t ADC2_Buffer[DC_CHANNEL];
static int16_t  ADC1_DMABuffer[AC_CONVERION_NUMBER*ADC_CHANNEL];
uint8_t ADC2_CHANNEL[DC_CHANNEL] = {   ADC_CH_9, ADC_CH_2,ADC_CH_6,ADC_CH_7,ADC_CH_14,ADC_CH_15};
#define ADC1_CH_COUNT 2
#define ADC1_PRIOR 1
#define ADC1_SUB_PRIOR 0
static int16_t sens_press=0;

int16_t GetConversional(ADC_Conversionl_Buf_t * pBuf);
#define DC_24_BufferSize    3
#define DC_AIN_BufferSize  10
#define Sens_BufferSize 1000
ADC_Conversionl_Buf_t DataBuffer[DC_CHANNEL+2];


int16_t SenseBuffer1[ Sens_BufferSize];
int16_t SenseBuffer2[ Sens_BufferSize];
int16_t DC24Buffer[ DC_24_BufferSize];
int16_t AIN1Buffer[ DC_AIN_BufferSize];
int16_t AIN2Buffer[ DC_AIN_BufferSize];
int16_t AIN3Buffer[ DC_AIN_BufferSize];
int16_t AIN4Buffer[ DC_AIN_BufferSize];
int16_t SensTemoBuffer[ DC_AIN_BufferSize];
/*
 *
 */

/*
 *
 */

void vSetCount( u16 coount)
{
    if ((coount >1 ) && (coount <= Sens_BufferSize))
    DataBuffer[0].ConversionalSize = coount;
    DataBuffer[1].ConversionalSize = coount;
}
u16 uGetConversionCount()
{
    return DataBuffer[0].ConversionalSize;
}

TaskHandle_t * getADCTaskHandle()
{
    return (&pADCTaskHandle);
}

TaskHandle_t * getI2CTaskHandle()
{
    return (&pI2CTaskHandle);
}

static void ADC1_Event()
{
    HAL_TiemrDisable(TIMER3);
    HAL_DMA_Disable(DMA1_CH1);
    static portBASE_TYPE xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(pADCTaskHandle,  ADC1_DATA_READY, eSetBits, &xHigherPriorityTaskWoken  );
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

volatile u16 ADC_val[4];

static u8 conversion_channel;

#define MIN_BAR_VALUE -2500
#define MAX_BAR_VALUE  2500
#define MIN_VDD_VALUE  0.177
#define MAX_VDD_VALUE  2.7
#define VDD_RANGE    (MAX_VDD_VALUE - MIN_VDD_VALUE)

float Get_025HPDPN33_SensorData( float data)
{
    if ( (data - MIN_VDD_VALUE) >= (VDD_RANGE/2-MIN_VDD_VALUE))
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
    switch (channel)
    {
        case  SENS1:
            return  ((float)GetConversional(&DataBuffer[0]));
        case SENS2:
           return (Get_025HPDPN33_SensorData((float)GetConversional(&DataBuffer[1])*KK));
        case DC24:
             return  ((float)GetConversional(&DataBuffer[2])*KK*COOF_24V);

        case DCAIN1:
            return  ((float)GetConversional(&DataBuffer[3])*KK*COOF_10V);

        case DCAIN2:
            return  ((float)GetConversional(&DataBuffer[4])*KK*COOF_10V);
        case DCAIN3:
            return  ((float)GetConversional(&DataBuffer[5])*KK*COOF_10V);

       case DCAIN4:
            return ((float)GetConversional(&DataBuffer[6])*KK);
       case  DIG_TEMP:
           return ((float)GetConversional(&DataBuffer[7])/256);
       case DIG_PRES:

               return (float)(sens_press);

        case AC220:
            return (AC_220_VALUE);
    }


}


static void ADC2_Event()
{
    ADC2_Buffer[conversion_channel ] = Get_ConversionVal(ADC_2,HAL_ADC_DATA(ADC_2));
    conversion_channel++;
    if (conversion_channel  >= DC_CHANNEL)
    {
        conversion_channel  = 0;
        ADC_RegularChannelConfig(ADC2,   ADC2_CHANNEL[0], 1, ADC_SampleTime_7Cycles5 );
        static portBASE_TYPE xHigherPriorityTaskWoken;
        xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(pADCTaskHandle, ADC2_DATA_READY, eSetBits, &xHigherPriorityTaskWoken  );
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
        return;

    }
    ADC_RegularChannelConfig(ADC2,   ADC2_CHANNEL[conversion_channel], 1, ADC_SampleTime_7Cycles5 );
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
    DMA_INIT_t init;

    uint8_t ADC1_CHANNEL[ADC1_CH_COUNT] = { ADC_CH_0,  ADC_CH_1 };
    HAL_ADC_ContiniusScanTrigCinvertionDMA( ADC_1, ADC1_CH_COUNT, ADC1_CHANNEL, ADC_ExternalTrigConv_T3_TRGO);
    init.stream = DMA1_CH1;
    init.direction = PTOM;
    init.mode  = DMA_Circular;
    init.paddr = (u32)&ADC1->RDATAR;
    init.memadr = (u32)ADC1_DMABuffer;
    init.dma_size = DMA_HWORD;
    init.bufsize = AC_CONVERION_NUMBER*2;
    init.prioroty = dma_Medium;
    HAL_DMAInitIT(init,  ADC1_PRIOR , ADC1_SUB_PRIOR, &ADC1_Event  );
    HAL_DMA_Enable(DMA1_CH1);
    HAL_ADC_InitIT(ADC_2, ADC_ExternalTrigConv_None , 1, 0, &ADC2_Event );
    ADC_RegularChannelConfig(ADC2, ADC2_CHANNEL[0], 1, ADC_SampleTime_239Cycles5);
    ADC_TempSensorVrefintCmd(ENABLE);
    memset(SenseBuffer1,0,Sens_BufferSize*2);
    DataBuffer[0].ConversionalSize = Sens_BufferSize;
    DataBuffer[0].pIndex = 0;
    DataBuffer[0].pBuff = SenseBuffer1;
    memset(SenseBuffer2,0,Sens_BufferSize*2);
    DataBuffer[1].ConversionalSize = Sens_BufferSize;
    DataBuffer[1].pIndex = 0;
    DataBuffer[1].pBuff = SenseBuffer2;
    DataBuffer[2].ConversionalSize = DC_24_BufferSize;
    DataBuffer[2].pIndex = 0;
    DataBuffer[2].pBuff = DC24Buffer;
    DataBuffer[3].ConversionalSize = DC_AIN_BufferSize;
    DataBuffer[3].pIndex = 0;
    DataBuffer[3].pBuff = AIN1Buffer;
    DataBuffer[4].ConversionalSize = DC_AIN_BufferSize;
    DataBuffer[4].pIndex = 0;
    DataBuffer[4].pBuff = AIN2Buffer;
    DataBuffer[5].ConversionalSize = DC_AIN_BufferSize;
    DataBuffer[5].pIndex = 0;
    DataBuffer[5].pBuff = AIN3Buffer;
    DataBuffer[6].ConversionalSize = DC_AIN_BufferSize;
    DataBuffer[6].pIndex = 0;
    DataBuffer[6].pBuff = AIN4Buffer;
    DataBuffer[7].ConversionalSize = DC_AIN_BufferSize;
    DataBuffer[7].pIndex = 0;
    DataBuffer[7].pBuff = SensTemoBuffer;
}

void AddBufferDataI2C( ADC_Conversionl_Buf_t * pBuf, int16_t data )
{
    pBuf->pBuff[pBuf->pIndex] = data;
    pBuf->pIndex++;
    if (pBuf-> pIndex >= pBuf->ConversionalSize)  pBuf->pIndex = 0;
}


void AddBufferData( ADC_Conversionl_Buf_t * pBuf, int16_t data )
{
    pBuf->pBuff[pBuf->pIndex] = data;
    pBuf->pIndex++;
    if (pBuf-> pIndex >= pBuf->ConversionalSize)  pBuf->pIndex = 0;
}

int16_t GetConversional(ADC_Conversionl_Buf_t * pBuf)
{
    int32_t tempdata = 0;
    uint16_t index = pBuf-> pIndex;
    for (uint16_t i = 0; i < pBuf->ConversionalSize; i++)
    {
        if (index == 0) index = (pBuf->ConversionalSize -1 );
        index--;
        tempdata+=  pBuf->pBuff[index];
    }
    tempdata/= pBuf->ConversionalSize;
    return (int16_t)tempdata;
}



#define SENOR_MAX_DATA 1000
u32 sensor_data[2][SENOR_MAX_DATA];


u8 adress[8]= {0x06,0x07,0x08,0x09,0x0A,0x30,0xA5,0xA6};
#define SICLE_I2C 100

#include "init.h"

void ADC_task(void *pvParameters)
{
    uint8_t ANALOG_DATA_FSM = 0;
    uint32_t ulNotifiedValue;
    TaskFSM_t ADC_TASK_FSM = STATE_INIT;
    TickType_t xLastWakeTime;
    int16_t iMax =0;
    uint16_t DF1;
    uint16_t old= 0;
    uint16_t uCurPeriod = AC_CONVERION_NUMBER-1;
    uint8_t DC_ConversionDoneFlasg = 0;
    uint8_t AC_ConversionDoneFlasg = 0;
    u16 initr = 0;

    uint8_t dc_conv_start = 5;

    xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
            vTaskDelayUntil( &xLastWakeTime,  1 );
            switch (ADC_TASK_FSM)
            {
                case STATE_INIT:
                    ADC_SoftwareStartConvCmd(ADC2, ENABLE);
                    HAL_TiemrEneblae(TIMER3);
                    ADC_TASK_FSM = STATE_WHAIT_TO_RAEDY;
                    break;
                case STATE_WHAIT_TO_RAEDY:
                    ADC_TASK_FSM = STATE_RUN;

                    break;
                case STATE_RUN:
                    xTaskNotifyWait( 0,  ADC2_DATA_READY | ADC1_DATA_READY, &ulNotifiedValue,0);
                    if (ulNotifiedValue & ADC2_DATA_READY)    //����ݧ� ���ڧݧ֧�֧ݧ� ���֧�ӧѧߧڧ� ��� ������ ��ҧ�ѧҧѧ��ӧѧ��֧ԧ� DC �էѧߧߧ��
                    {
                        for (u8 i=0;i<DC_CHANNEL;i++)
                        {
                            AddBufferData(&DataBuffer[i+1],(int16_t)ADC2_Buffer[i]);   //���ѧܧڧէ�ӧѧ֧� �ڧ� �� �ҧ���֧�
                        }
                        DC_ConversionDoneFlasg = 1;
                    }
                    if (DC_ConversionDoneFlasg)
                    {
                        if (dc_conv_start == ( ANALOG_DATA_FSM %10) )
                        {
                           if  (--dc_conv_start == 0) dc_conv_start = 9;
                           ADC_SoftwareStartConvCmd(ADC2, ENABLE);
                           DC_ConversionDoneFlasg = 0;
                        }
                    }
                    if (ulNotifiedValue & ADC1_DATA_READY)
                    {
                        AC_ConversionDoneFlasg = 1;
                    }

                    if (AC_ConversionDoneFlasg  == 1)
                    {
                       for (int i = 0; i<AC_CONVERION_NUMBER*ADC_CHANNEL;i++)
                       {
                          ADC1_DMABuffer[i]= Get_ConversionVal(ADC_1, ADC1_DMABuffer[i]);
                       }
                       vDecNetural(&ADC1_DMABuffer[0]);
                       iMax=xADCMax((int16_t *)&ADC1_DMABuffer[0], &DF1);
                       vADCFindFreq((int16_t *)&ADC1_DMABuffer[0], &uCurPeriod,2,iMax);
                       AC_ConversionDoneFlasg  = 2;
                    }
                    else
                    if (AC_ConversionDoneFlasg  == 2)
                    {
                      if ((uCurPeriod < AC_CONVERION_NUMBER-1) && (uCurPeriod!=0) )
                      {
                           uint16_t data1 = xADCRMS(&ADC1_DMABuffer[0],uCurPeriod,2);
                           data1 = vRCFilter(data1, &old);
                           AC_220_VALUE = (float)data1 * ( 401U * 3.3 / 4095U );
                      }
                       HAL_DMA_SetCounter(DMA1_CH1, AC_CONVERION_NUMBER*2);
                       HAL_DMA_Enable(DMA1_CH1);
                       HAL_TiemrEneblae(TIMER3);
                       AC_ConversionDoneFlasg  = 0;
                    }


                    if (++ANALOG_DATA_FSM >=10) ANALOG_DATA_FSM = 0;



                    // if (++initr > 0xFFF) initr =0;
                   //  DAC_SetChannel1Data(DAC_Align_12b_R, initr);
                   break;
            }
    }
}

#define SENSOR_TIME_OUT 100

void I2C_task(void *pvParameters)
{
    u8 status;
    u8 bufdata;
    u16 delay_counter=0;
    uint8_t sens_conversion_start;
    uint8_t ADC_FSM = 0;
    uint16_t counter_led = 0;
      u8 led_state = 0;
      vTaskDelay(1000);
                         printf("Start I2C OK %i\r\n",SICLE_I2C);

 while(1)
 {
     if (++counter_led >500)
                {
                    counter_led =0;
                    if (led_state == 0)
                    {
                        led_state = 1;
                        HAL_SetBit(CRACH_Port,  CRACH_Pin);
                    }
                    else
                    {
                        HAL_ResetBit(CRACH_Port, CRACH_Pin);
                       led_state = 0;
                    }
                }


     vTaskDelay(1);
     delay_counter++;

     if (HAL_GetI2CBusy( I2C_2 ) == 0 )
     {

         switch (ADC_FSM )
         {
             case 0:
                 printf("1\r\n");
                     bufdata = 0x0A;
                     I2C_Master_TransmitIT(I2C_2, 0x6D<<1 , 0x30, &bufdata,1,1,0);
                     ADC_FSM = 1;
                     delay_counter = 0;

                 break;
             case 1:
                 printf("2\r\n");
                sens_conversion_start = I2C_Master_ReviceIT(I2C_2,0x6D<<1, 0x30 ,&status,1,1,0);
                if (( sens_conversion_start != HAL_I2C_ERROR) && ((status & 0x08) == 0 ))
                 {
                     ADC_FSM = 2;
                 }
                 break;
             case 2:
                 printf("3\r\n");
                 sens_conversion_start = I2C_Master_ReviceIT(I2C_2,0x6D<<1, 0x06 ,&i2cdata[1][0],1,1,0);
                 if ( sens_conversion_start != HAL_I2C_ERROR)
                     ADC_FSM = 3;

                 break;
             case 3:
                 printf("4\r\n");
                 sens_conversion_start = I2C_Master_ReviceIT(I2C_2,0x6D<<1, 0x07 ,&i2cdata[1][1],1,1,0);
                 if ( sens_conversion_start != HAL_I2C_ERROR)
                                    ADC_FSM = 4;

                 break;
             case 4:
                 printf("5\r\n");
                 sens_conversion_start = I2C_Master_ReviceIT(I2C_2,0x6D<<1, 0x08 ,&i2cdata[1][2],1,1,0);
                 if ( sens_conversion_start != HAL_I2C_ERROR)
                 {

                     uint32_t temp_data = (u32)i2cdata[1][0]<<16 | (u32)i2cdata[1][1]<<8 | i2cdata[1][2];
                                         if (temp_data > 0x800000)
                                         {
                                             sens_press  = ((temp_data - 16777216)/GetSensCoof());
                                         }
                                         else
                                         {
                                             sens_press  = temp_data/GetSensCoof();
                                         }
                                         AddBufferDataI2C(&DataBuffer[0], sens_press  );

                                         ADC_FSM = 5;
                 }

                 break;

             case 5:

                 if (delay_counter >SENSOR_TIME_OUT) ADC_FSM = 0;
                 break;
            /* case 19:
                // sens_conversion_start = I2C_Master_ReviceIT(I2C_2,0x6D<<1, 0x30 ,&status,1,10,2);
                // if ( sens_conversion_start != HAL_I2C_ERROR)
               //  {
               //    if ((status & 0x08) == 0 ) ADC_FSM++;
               //  }
                 break;
             case 20:
               //  sens_conversion_start = I2C_Master_ReviceIT(I2C_2,0x6D<<1, 0x06 ,&i2cdata[1][0],2,10,0);
              //   if ( sens_conversion_start != HAL_I2C_ERROR) ADC_FSM++;
              //   else
              //   {
              //       ADC_FSM = 23;
              //   }
                 break;
             case 21:
                / sens_conversion_start = I2C_Master_ReviceIT(I2C_2,0x6D<<1, 0x07 ,&i2cdata[1][1],2,10,0);
                 if ( sens_conversion_start != HAL_I2C_ERROR) ADC_FSM++;
                  else
                  {
                     ADC_FSM = 23;
                  }
                 break;
             case 22:
                  sens_conversion_start = I2C_Master_ReviceIT(I2C_2,0x6D<<1, 0x08 ,&i2cdata[1][2],2,10,0);
                 if (sens_conversion_start == HAL_I2C_OK)
                 {
                     uint32_t temp_data = (u32)i2cdata[1][0]<<16 | (u32)i2cdata[1][1]<<8 | i2cdata[1][2];
                     if (temp_data > 0x800000)
                     {
                         sens_press  = ((temp_data - 16777216)/GetSensCoof());
                     }
                     else
                     {
                         sens_press  = temp_data/GetSensCoof();
                     }
                     AddBufferData(&DataBuffer[0], sens_press  );
                     ADC_FSM = 23;
                 }
                 else {
                     ADC_FSM = 25;
                }
                 break;
             case 23:
                 I2C_Master_ReviceIT(I2C_2,0x6D<<1, 0x09 ,&status,1,10,2);
                 ADC_FSM = 24;
                 break;
             case 24:
                 I2C_Master_ReviceIT(I2C_2,0x6D<<1, 0x0A ,&status,1,10,2);
                                  ADC_FSM = 25;
                 break;*/
             case 40:
                 ADC_FSM = 0;
                 break;
         }
         //if (++ADC_FSM >=SICLE_I2C) ADC_FSM = 0;

     }
     /*
                         {

                             case 0:
                                 //���ѧ���ܧѧ֧� ���֧�ҧ�ѧ٧�ӧѧߧڧ� �ߧ� ��ڧ���ӧ�� �էѧ��ڧܧ�
                                 bufdata = 0x0A;

                                 sens_conversion_start  = I2C_Master_TransmitIT(I2C_2, 0x6D<<1 , 0x30, &bufdata,1,10,2);
                                 break;
                             case 1:

                             case 3:
                             case 4:
                                 break;
                             case 5:
                                 if (sens_conversion_start == HAL_I2C_OK)
                                 {
                                     sens_conversion_start = I2C_Master_ReviceIT(I2C_2,0x6D<<1, 0x30 ,&status,1,10,2);
                                 }
                                 break;
                             case 6:
                                 if (sens_conversion_start == HAL_I2C_OK)
                                 {
                                     if  (status & 0x08)
                                         sens_conversion_start = I2C_Master_ReviceIT(I2C_2,0x6D<<1, 0x30 ,&status,1,10,2);
                                 }
                                 break;
                             case 7:
                                 if (sens_conversion_start == HAL_I2C_OK)
                                 {
                                     if  (status & 0x08)
                                         sens_conversion_start = I2C_Master_ReviceIT(I2C_2,0x6D<<1, 0x30 ,&status,1,10,2);
                                 }
                                 break;
                             case 8:
                                 if (sens_conversion_start == HAL_I2C_OK)
                                 {
                                 if ((status & 0x08) == 0 )
                                 {
                                     for (u8 i = 0; i<3; i++)
                                     {
                                         sens_conversion_start = I2C_Master_ReviceIT(I2C_2,0x6D<<1, adress[i] ,&i2cdata[1][i],2,10,2);
                                         if ( sens_conversion_start == HAL_I2C_ERROR) break;

                                     }
                                     if (sens_conversion_start == HAL_I2C_OK)
                                     {
                                     uint32_t temp_data = (u32)i2cdata[1][0]<<16 | (u32)i2cdata[1][1]<<8 | i2cdata[1][2];
                                     if (temp_data > 0x800000)
                                     {
                                          sens_press  = ((temp_data - 16777216)/GetSensCoof());
                                     }
                                     else
                                     {
                                         sens_press  = temp_data/GetSensCoof();
                                     }
                                     AddBufferData(&DataBuffer[0], sens_press  );
                                     }
                                 }
                                 }
                                 break;

                             default:

                                 break;
                         }
                        */
 }
}





