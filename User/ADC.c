/*
 * ADC.C
 *
 *  Created on: Feb 5, 2024
 *      Author: i.dymov
 */
#include "AC_ConvertLib.h"
#include "ADC.H"
#include "main.h"

static s16 Calibrattion_Val = 0;
static s16 Calibrattion_Val2 = 0;
static uint16_t ADC2_Buffer[DC_CONVERSION_NUMBER*DC_CHANNEL];
static uint16_t DC_Buffer[DC_CHANNEL];
static int16_t  ADC1_DMABuffer[AC_CONVERION_NUMBER*ADC_CHANNEL];
static EventGroupHandle_t xADCEventGroupHandle;
static uint8_t ADC_2_FSM  = 0;
static uint8_t DC_cnversion =0;


void DMA_Tx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure={0};
    NVIC_InitTypeDef NVIC_InitStructure={0};
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );

    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular ;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init( DMA_CHx, &DMA_InitStructure );

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_ITConfig( DMA1_Channel1, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE, ENABLE );
}


void TIM1_PWM_In( void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_InternalClockConfig(TIM3);
    TIM_TimeBaseInitStructure.TIM_Period = SystemCoreClock /2/ 1000000;
    TIM_TimeBaseInitStructure.TIM_Prescaler =25;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
}

/* ADC JL mask */
#define ISQR_JL_Set1                      ((uint32_t)0x00300000)
//#define ISQR_JL_Reset                    ((uint32_t)0xFFCFFFFF)
#define ISQR_JSQ_Set1                     ((uint32_t)0x0000001F)

void ADC_ChConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t Rank)
{
    uint32_t tmpreg1 = 0, tmpreg2 = 0, tmpreg3 = 0;



    tmpreg1 = ADCx->ISQR;
    tmpreg3 = (tmpreg1 & ISQR_JL_Set1) >> 20;
    tmpreg2 = ISQR_JSQ_Set1 << (5 * (uint8_t)((Rank + 3) - (tmpreg3 + 1)));
    tmpreg1 &= ~tmpreg2;
    tmpreg2 = (uint32_t)ADC_Channel << (5 * (uint8_t)((Rank + 3) - (tmpreg3 + 1)));
    tmpreg1 |= tmpreg2;
    ADCx->ISQR = tmpreg1;
}



void ADC_TestInit()
{

    ADC_InitTypeDef  ADC_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    //§±§à§Ý§å§é§Ñ§Ö§Þ §å§Ü§Ñ§Ù§Ñ§Ö§ä§Ö§Ý§î §ß§Ñ §æ§Ý§Ñ§Ô§Ú §â§Ñ§Ò§à§ä§í §ã §¡§¸§±
    xADCEventGroupHandle = xGetADCEvent();

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1  , ENABLE );
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2  , ENABLE );
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode =ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv =ADC_ExternalTrigConv_T3_TRGO;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 2;
    ADC_InitStructure.ADC_OutputBuffer = ADC_OutputBuffer_Disable;
    ADC_InitStructure.ADC_Pga = ADC_Pga_1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1,   ADC_Channel_1, 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1,   ADC_Channel_0, 2, ADC_SampleTime_239Cycles5);
    ADC_ExternalTrigConvCmd(ADC1,ENABLE);

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE); //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    Calibrattion_Val = Get_CalibrationValue(ADC1);
    ADC_BufferCmd(ADC1, ENABLE);
    ADC_TempSensorVrefintCmd(ENABLE);

    DMA_Tx_Init( DMA1_Channel1, (u32)&ADC1->RDATAR, (u32)ADC1_DMABuffer,AC_CONVERION_NUMBER*2 );
    DMA_Cmd( DMA1_Channel1, ENABLE );
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    TIM1_PWM_In();


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    ADC_DeInit(ADC2);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode =DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None ;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 4;

    ADC_Init(ADC2, &ADC_InitStructure);

    ADC_InjectedSequencerLengthConfig(ADC2, 4);
    ADC_InjectedChannelConfig(ADC2, ADC_Channel_TempSensor, 1, ADC_SampleTime_239Cycles5);
    ADC_InjectedChannelConfig(ADC2, ADC_Channel_6, 2, ADC_SampleTime_239Cycles5);
    ADC_InjectedChannelConfig(ADC2, ADC_Channel_7, 3, ADC_SampleTime_239Cycles5);
    ADC_InjectedChannelConfig(ADC2, ADC_Channel_8, 4, ADC_SampleTime_239Cycles5);

    ADC_ExternalTrigInjectedConvConfig(ADC2,ADC_ExternalTrigInjecConv_None );

   // ADC_ExternalTrigConvCmd(ADC2,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    ADC_ITConfig(ADC2, ADC_IT_JEOC, ENABLE);
    ADC_Cmd(ADC2, ENABLE);

    ADC_BufferCmd(ADC2, DISABLE); //disable buffer
    ADC_ResetCalibration(ADC2);
    while(ADC_GetResetCalibrationStatus(ADC2));
    ADC_StartCalibration(ADC2);
    while(ADC_GetCalibrationStatus(ADC2));
    Calibrattion_Val2 = Get_CalibrationValue(ADC2);
    ADC_BufferCmd(ADC2, ENABLE);
    ADC_TempSensorVrefintCmd(ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}


u16 Get_ConversionVal(s16 val)
{
    if((val+Calibrattion_Val)<0|| val==0) return 0;
    if((Calibrattion_Val+val)>4095||val==4095) return 4095;
    return (val+Calibrattion_Val);
}

u16 Get_ConversionVal2(s16 val)
{
    if((val+Calibrattion_Val2)<0|| val==0) return 0;
    if((Calibrattion_Val2+val)>4095||val==4095) return 4095;
    return (val+Calibrattion_Val2);
}


void ADC1_2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel1_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));


/*********************************************************************
 * @fn      ADC1_2_IRQHandler
 *
 * @brief   ADC1_2 Interrupt Service Function.
 *
 * @return  none
 */

volatile u16 ADC_val[4];


void ADC1_2_IRQHandler()
{

    if(ADC_GetITStatus(ADC2, ADC_IT_JEOC))
    {

        ADC_ClearITPendingBit(ADC2, ADC_IT_JEOC);
        ADC_val[0] = ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_1);
        ADC_val[1] = ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_2);
        ADC_val[2] = ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_3);
        ADC_val[3]=  ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_4);
        ADC2_Buffer[ ADC_2_FSM*12 +  DC_cnversion  *4]  = Get_ConversionVal2( ADC_val[0]);
        ADC2_Buffer[ADC_2_FSM*12 + DC_cnversion  *4 +1]  = Get_ConversionVal2( ADC_val[1]);
        ADC2_Buffer[ADC_2_FSM*12 + DC_cnversion  *4 +2]  = Get_ConversionVal2( ADC_val[2]);
        ADC2_Buffer[ADC_2_FSM*12 + DC_cnversion  *4 +3 ]  = Get_ConversionVal2( ADC_val[3]);


        if (++DC_cnversion >=DC_CONVERSION_NUMBER)
        {
            DC_cnversion = 0;
            switch( ADC_2_FSM )
            {
                          case 0:
                              ADC_2_FSM = 1;
                              ADC_ChConfig(ADC2,ADC_Channel_9,  1);
                              ADC_ChConfig(ADC2, ADC_Channel_10, 2);
                              ADC_ChConfig(ADC2, ADC_Channel_11, 3);
                              ADC_ChConfig(ADC2, ADC_Channel_12, 4);
                            // xEventGroupSetBitsFromISR( xADCEventGroupHandle, ADC2_DATA_READY, &xHigherPriorityTaskWoken );
                             // ADC2_Buffer[]
                              break;
                         case 1:
                              ADC_2_FSM = 0;
                             ADC_ChConfig(ADC2, ADC_Channel_TempSensor, 1);
                             ADC_ChConfig(ADC2, ADC_Channel_6, 2);
                             ADC_ChConfig(ADC2, ADC_Channel_7, 3);
                              ADC_ChConfig(ADC2, ADC_Channel_8, 4);
                              static portBASE_TYPE xHigherPriorityTaskWoken;
                                                           xHigherPriorityTaskWoken = pdFALSE;
                                                           xEventGroupSetBitsFromISR( xADCEventGroupHandle, ADC2_DATA_READY, &xHigherPriorityTaskWoken );

                                                           portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

                              break;
                    }




        }



    }
}



void DMA1_Channel1_IRQHandler()
{
    //printf( "DMA Ready");
    if(DMA_GetITStatus(DMA1_IT_TC1)==SET )
    {
        static portBASE_TYPE xHigherPriorityTaskWoken;
        TIM_Cmd(TIM3, DISABLE);  //§°§ã§ä§Ñ§ß§Ó§Ñ§Ý§Ú§Ó§Ñ§Ö§Þ §ä§Ñ§Ü§ä§Ú§â§à§Ó§Ñ§ß§Ú§Ö §¡§¸§±
        DMA_ClearITPendingBit(DMA1_IT_GL1); //§³§Ò§â§Ñ§ã§í§Ó§Ñ§Ö§Þ §æ§Ý§Ñ§Ô
        xHigherPriorityTaskWoken = pdFALSE;
        xEventGroupSetBitsFromISR( xADCEventGroupHandle, ADC1_DATA_READY, &xHigherPriorityTaskWoken );
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    }
    return;
}




void ADC_task(void *pvParameters)
{
  u8 ttt=0;
    int16_t iMax =0;
    uint16_t DF1;
    uint16_t old= 0;
    uint16_t uCurPeriod = AC_CONVERION_NUMBER-1;
    ADC_TestInit();
    for (;;)
    {
            EventBits_t bits =  xEventGroupWaitBits(xADCEventGroupHandle,ADC1_DATA_READY | ADC2_DATA_READY, pdTRUE, pdFALSE, portMAX_DELAY );
            if (bits & ADC1_DATA_READY)
            {

                 xEventGroupClearBits(xADCEventGroupHandle, ADC1_DATA_READY);
                 for (int i = 0; i<AC_CONVERION_NUMBER*ADC_CHANNEL;i++)
                 {
                     ADC1_DMABuffer[i]= Get_ConversionVal(ADC1_DMABuffer[i]);
                 }
                 vDecNetural(&ADC1_DMABuffer[0]);
                 iMax=xADCMax((int16_t *)&ADC1_DMABuffer[0], &DF1);
                 vADCFindFreq((int16_t *)&ADC1_DMABuffer[0], &uCurPeriod,2,iMax);
                 if ((uCurPeriod < AC_CONVERION_NUMBER-1) && (uCurPeriod!=0) )
                 {
                           uint16_t data1 = xADCRMS(&ADC1_DMABuffer[0],uCurPeriod,2);
                           data1 = vRCFilter(data1, &old);

                           data1 = (float)data1 * ( 401U * 3.3 / 4095U );
                           //int8SetRegister(V220,(uint8_t)data1 );
                 }
                 TIM_Cmd(TIM3, ENABLE); //§©§Ñ§á§å§ã§Ü§Ñ§Ö §à§é§Ö§â§Ö§ß§à§Ö §á§â§Ö§à§Ò§â§Ñ§Ù§à§Ó§Ñ§ß§Ú§Ö
               //  printf("ADC_START_2\r\n");
                 if (++ttt >= 10)
                 {
                     ttt=0;
                     ADC_SoftwareStartInjectedConvCmd(ADC2, ENABLE); //§©§Ñ§á§å§ã§Ü§Ñ§Ö§Þ §á§â§Ö§à§Ò§â§Ñ§Ù§Ó§à§Ñ§ß§Ú§Ö §ß§Ñ §¡§¸§±2
                 }

            }
            if (bits & ADC2_DATA_READY)
            {
                uint32_t u32Buffer;

                printf("ADC_END\r\n");
                for (int j = 0; j < DC_CHANNEL; j++)
                {
                    for (int k=0; k< DC_CONVERSION_NUMBER;k++)
                    {
                        u32Buffer = ADC2_Buffer[k*DC_CHANNEL + j];
                    }
                    DC_Buffer[j] = u32Buffer /DC_CONVERSION_NUMBER;
                    printf( "JADC2 ch=%02d %04d\r\n", j,DC_Buffer[j] );
                }

            }
    }
}







