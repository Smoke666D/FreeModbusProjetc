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


static TaskHandle_t pADCTaskHandle;
static TaskHandle_t  pTaskToNotifykHandle;
static uint16_t ADC_VALUE[DC_CHANNEL];
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

float getAIN( AIN_CHANNEL_t channel)
{
    switch (channel)
    {
        case  SENS1:
            return ADC_VALUE[0]*K;
        case SENS2:
            return ADC_VALUE[1]*K;
        case DC24:
            return ADC_VALUE[2]*COOF_24V*K;
        case DCAIN1:
            return ADC_VALUE[3]*K;
        case DCAIN2:
            return ADC_VALUE[4]*K;
        case DCAIN3:
            return ADC_VALUE[5]*K;
       case DCAIN4:
            return ADC_VALUE[6]*K;
        case AC220:
            return AC_220_VALUE;
    }
    return 0;

}


static void ADC2_Event()
{
    ADC2_Buffer[conversion_number * conversion_channel ] = Get_ConversionVal(ADC_2,HAL_ADC_DATA(ADC_2));
    printf( "ADC2 ch= %04d\r\n", ADC_val[0] );
    conversion_channel++;

    if (++conversion_channel  >= DC_CHANNEL)
    {
        conversion_channel  = 0;

       if (++conversion_number >= DC_CONVERSION_NUMBER)
       {
           conversion_number = 0;
           static portBASE_TYPE xHigherPriorityTaskWoken;
           xHigherPriorityTaskWoken = pdFALSE;
           xTaskNotifyIndexedFromISR(pADCTaskHandle, 2, ADC2_DATA_READY, eSetValueWithoutOverwrite, &xHigherPriorityTaskWoken  );
          // xEventGroupSetBitsFromISR( xADCEventGroupHandle, ADC2_DATA_READY, &xHigherPriorityTaskWoken );
           portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
       }
       else
       {
           ADC_SoftwareStartConvCmd(ADC2, ENABLE);
       }
    }
    ADC_RegularChannelConfig(ADC2,   ADC2_CHANNEL[conversion_channel], 1, ADC_SampleTime_239Cycles5);
}

void ADC1_Init()
{
    DMA_INIT_t init;
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
    HAL_DMA_Enable(DMA1_CH1);
    HAL_ADC_InitIT(ADC_2, ADC_ExternalTrigConv_None , 5, 0, &ADC2_Event );
    ADC_RegularChannelConfig(ADC2, ADC2_CHANNEL[0], 1, ADC_SampleTime_239Cycles5);
    ADC_TempSensorVrefintCmd(ENABLE);
}


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
            switch (ADC_TASK_FSM)
            {
                case STATE_INIT:
                    TIM_Cmd(TIM8, ENABLE);
                    ADC_SoftwareStartConvCmd(ADC2, ENABLE);
                    ADC_TASK_FSM = STATE_WHAIT_TO_RAEDY;
                    break;
                case STATE_WHAIT_TO_RAEDY:
                    ADC_TASK_FSM = STATE_RUN;
                    break;
                case STATE_RUN:

                    xTaskNotifyWaitIndexed(2, 0, ADC1_DATA_READY | ADC2_DATA_READY, &ulNotifiedValue,portMAX_DELAY );
                  //  bits =  xEventGroupWaitBits(xADCEventGroupHandle,ADC1_DATA_READY | ADC2_DATA_READY, pdTRUE, pdFALSE, portMAX_DELAY );
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
                   }
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
                       }
                   }
                   break;
            }
    }
}







