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
static float PressSens[2]={0,0};
float AC_220_VALUE;
float AC_220_VALUE_CONTROL;
static uint16_t ADC2_Buffer[DC_CHANNEL];
static int16_t  ADC1_DMABuffer[AC_CONVERION_NUMBER*ADC_CHANNEL];
uint8_t ADC2_CHANNEL[DC_CHANNEL] = {  ADC_CH_2, ADC_CH_5,ADC_CH_6,ADC_CH_7,ADC_CH_14,ADC_CH_15};
#define ADC1_CH_COUNT 2
#define ADC1_PRIOR 1
#define ADC1_SUB_PRIOR 0
static int16_t sens_press=0;
static int16_t sens_press1=0;
int16_t GetConversional(ADC_Conversionl_Buf_t * pBuf);
int16_t GetConversionali2c(ADC_Conversionl_Buf_t * pBuf);
#define DC_24_BufferSize    3
#define DC_AIN_BufferSize  10
#define Sens_BufferSize_MAX 400
u16 sensor_timer[] = { 4,20,40,80,120,200,400};

#define DAC_CAL_POINT 10

POINT_t const DACCAL[DAC_CAL_POINT]={ {0,0},
                   {300,1.126},
                   {600,2.273},
                   {900,3.420},
                   {1200,4.567},
                   {1500,5.714},
                   {1800,6.85},
                   {2100,8.0},
                   {2400,9.14},
                   {2700,10.29}
};

#define K10 10000
const uint16_t B57164CalPoint[11][2] = {
                                    {50,K10*0.33363},
                                    {45,K10*0.41026},
                                    {40,K10*0.5074},
                                    {35,K10*0.63268},
                                    {30,K10*0.7942},
                                    {25,K10},
                                    {20,K10*1.2683},
                                    {15,K10*1.6204},
                                    {10,K10*2.086},
                                    {5,K10*2.7119},
                                    {0,K10*3.5563},


};

ADC_Conversionl_Buf_t DataBuffer[DC_CHANNEL+4];


int16_t SenseBuffer1[ Sens_BufferSize_MAX ];
int16_t SenseBuffer2[ Sens_BufferSize_MAX ];
int16_t DC24Buffer[ DC_24_BufferSize];
int16_t AIN1Buffer[ DC_AIN_BufferSize];
int16_t AIN2Buffer[ DC_AIN_BufferSize];
int16_t AIN3Buffer[ DC_AIN_BufferSize];
int16_t AIN4Buffer[ DC_AIN_BufferSize];
int16_t AIN5Buffer[ DC_AIN_BufferSize];
int16_t SensTemoBuffer[ DC_AIN_BufferSize];
int16_t SensTemoBuffer1[ DC_AIN_BufferSize];
/*
 *
 */

/*
 *
 */

void vSetCount( u16 coount)
{
    if (coount <= 7 )
    {
        saveReg16(SENSOR_COUNT,coount );
        DataBuffer[0].ConversionalSize = sensor_timer[coount];
        DataBuffer[1].ConversionalSize = sensor_timer[coount];
    }
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


u8 i2cdata[2][8] ={0};

#define DELTA  ((MAX_VDD_VALUE -MIN_VDD_VALUE )/(MIN_BAR_VALUE -  MAX_BAR_VALUE))


#define AIN1_ERROR 0x01
#define AIN2_ERROR 0x02
#define AIN3_ERROR 0x04

static u8 const SENSOR_ERROR_MASK[]={AIN1_ERROR,AIN2_ERROR,AIN3_ERROR};
float getAINConver( u8 ch)
{
    uint32_t raw_data;
    float temp_data;
    u8 sensor_error = getReg8(SENSOR_ERROR);
    u8 type;
    switch(ch)
    {
    default:
        raw_data = GetConversional(&DataBuffer[3]);
        type = getReg8(AIN1_TYPE);
       break;
    case 1:
        type = getReg8(AIN2_TYPE);
        raw_data = GetConversional(&DataBuffer[4]);
        break;
    case 2:
        type = getReg8(AIN3_TYPE);
        raw_data = GetConversional(&DataBuffer[5]);
        break;
    }
    switch (type)
    {
        default:
            sensor_error &= ~SENSOR_ERROR_MASK[ch];
            temp_data =  raw_data*KK*COOF_10V;
            break;
        case 1:
            temp_data = raw_data*KK*COOF_10V;
            if ( temp_data < 2.0 )
                sensor_error |=SENSOR_ERROR_MASK[ch];
            else
                sensor_error &= ~SENSOR_ERROR_MASK[ch];
            break;
        case 2:
            temp_data = raw_data*KK*10.0;
            if (temp_data < 4.0)
                sensor_error |=SENSOR_ERROR_MASK[ch];
                        else
                            sensor_error &= ~SENSOR_ERROR_MASK[ch];
            break;
    }
   setReg8(SENSOR_ERROR,sensor_error );
   return (temp_data);
}



float getAIN( AIN_CHANNEL_t channel)
{
    u16 temp_data;
    float temp_float;
    switch (channel)
    {
        case SENS1:
            return   (PressSens[0]);
        case SENS2:
            return  ( PressSens[1]);
        case DC24:
             return  ((float)GetConversional(&DataBuffer[2])*KK*COOF_24V);
        case DCAIN1:
            return  getAINConver(0);
        case DCAIN2:
            return  getAINConver(1);
        case DCAIN3:
            return  getAINConver(2);
       case DCAIN4:
            temp_data = (u16)GetConversional(&DataBuffer[6]);
            temp_float =fGetAinCalData(AIN4,(float)(temp_data*K10)/(4095-temp_data));
            if (temp_float < -55.0 ) return (-55.0);
                        else
                        return (temp_float);
       case DCAIN5:
            temp_data = (u16)GetConversional(&DataBuffer[7]);
            temp_float = fGetAinCalData(AIN5,(float)(temp_data*K10)/(4095-temp_data));
            if (temp_float < -55.0 ) return (-55.0);
            else
            return (temp_float);
       case DIG_TEMP:
           return ((float)GetConversional(&DataBuffer[8])/256);
       case DIG_PRES:
           return (float)(sens_press);
       case DIG2_TEMP:
           return ((float)GetConversional(&DataBuffer[9])/256);
       case DIG2_PRES:
           return (float)(sens_press1);
        case AC220:
           return (AC_220_VALUE);
        case AC220_CONTROL:
           return (AC_220_VALUE_CONTROL);
    }
    return (0);
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




void ADC1_Init()
{

    POINT_t d[2];
    vAINInit();
    eAinCalDataConfig(AIN4,11);
    eAinCalDataConfig(AIN5,11);
    for (int i = 0;i<10;i++)
    {
          d[0].X = B57164CalPoint[i][1];
          d[0].Y = B57164CalPoint[i][0];
          d[1].X = B57164CalPoint[i+1][1];
          d[1].Y = B57164CalPoint[i+1][0];
          eSetAinCalPoint(AIN4,&d[0],i);
          eSetAinCalPoint(AIN5,&d[0],i);
    }
    vDacInit();
    eDacCalDataConfig(DAC1,DAC_CAL_POINT);
    eSetDacCalPoint(DAC1,  (POINT_t *)DACCAL, DAC_CAL_POINT );
    eDacCalDataConfig(DAC2,DAC_CAL_POINT);
    eSetDacCalPoint(DAC2,  (POINT_t *)DACCAL, DAC_CAL_POINT );
    eDacCalDataConfig(DAC3,DAC_CAL_POINT);
    eSetDacCalPoint(DAC3,  (POINT_t *)DACCAL, DAC_CAL_POINT );
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
    memset(SenseBuffer1,0,Sens_BufferSize_MAX*2);
    DataBuffer[0].ConversionalSize = 400;
    DataBuffer[0].pIndex = 0;
    DataBuffer[0].pBuff = SenseBuffer1;
    DataBuffer[0].offset =0;
    memset(SenseBuffer2,0,Sens_BufferSize_MAX*2);
    DataBuffer[1].ConversionalSize = 400;
    DataBuffer[1].pIndex = 0;
    DataBuffer[1].pBuff = SenseBuffer2;
    DataBuffer[1].offset =0;
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
    DataBuffer[8].ConversionalSize = DC_AIN_BufferSize;
    DataBuffer[8].pIndex = 0;
    DataBuffer[8].pBuff = SensTemoBuffer;
    DataBuffer[9].ConversionalSize = DC_AIN_BufferSize;
    DataBuffer[9].pIndex = 0;
    DataBuffer[9].pBuff = SensTemoBuffer1;
    DataBuffer[7].ConversionalSize = DC_AIN_BufferSize;
    DataBuffer[7].pIndex = 0;
    DataBuffer[7].pBuff = AIN5Buffer;

}

void vDataBufferInit()
{

    DataBuffer[0].ConversionalSize = sensor_timer[getReg8(SENSOR_COUNT )];
    DataBuffer[1].ConversionalSize = sensor_timer[getReg8(SENSOR_COUNT )];
    DataBuffer[0].offset =getRegi16(SENSOR1_ZERO);
    DataBuffer[1].offset =getRegi16(SENSOR2_ZERO);
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
    tempdata = tempdata - (pBuf->offset*pBuf->ConversionalSize);

    tempdata = tempdata/pBuf->ConversionalSize;
    return (int16_t)tempdata;
}


int16_t GetConversionali2c(ADC_Conversionl_Buf_t * pBuf)
{
    int32_t tempdata = 0;
    uint16_t index = pBuf-> pIndex;
    for (uint16_t i = 0; i < pBuf->ConversionalSize; i++)
    {
        tempdata+=  pBuf->pBuff[index];
        if (index == 0) index = (pBuf->ConversionalSize -1 );
        index--;

    }
    tempdata = tempdata - (pBuf->offset*pBuf->ConversionalSize);
    tempdata = tempdata/pBuf->ConversionalSize;
    return (int16_t)tempdata;
}

#define SENOR_MAX_DATA 1000
u32 sensor_data[2][SENOR_MAX_DATA];



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
                    xTaskNotifyWait( 0,  ADC2_DATA_READY | ADC1_DATA_READY, &ulNotifiedValue,1);
                    if (ulNotifiedValue & ADC2_DATA_READY)    //§¦§ã§Ý§Ú §á§â§Ú§Ý§Ö§ä§Ö§Ý§à §á§â§Ö§â§Ó§Ñ§ß§Ú§Ö §à§ä §¡§¸§± §à§Ò§â§Ñ§Ò§Ñ§ä§í§Ó§Ñ§ð§ë§Ö§Ô§à DC §Õ§Ñ§ß§ß§í§Ö
                    {
                        for (u8 i=0;i<DC_CHANNEL;i++)
                        {
                            AddBufferData(&DataBuffer[i+2],(int16_t)ADC2_Buffer[i]);   //§©§Ñ§Ü§Ú§Õ§í§Ó§Ñ§Ö§Þ §Ú§ç §Ó §Ò§å§æ§æ§Ö§â
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
                      for (int i = 0; i<AC_CONVERION_NUMBER*ADC_CHANNEL;i++)
                      {
                          ADC1_DMABuffer[i]= Get_ConversionVal(ADC_1, ADC1_DMABuffer[i]);
                      }
                      vDecNetural(&ADC1_DMABuffer[0]);
                      iMax=xADCMax((int16_t *)&ADC1_DMABuffer[0], &DF1);
                      vADCFindFreq((int16_t *)&ADC1_DMABuffer[0], &uCurPeriod,2,iMax);
                      if (uCurPeriod < AC_CONVERION_NUMBER-1)
                      {
                           uint16_t data1 = xADCRMS(&ADC1_DMABuffer[0],uCurPeriod,2);
                           AC_220_VALUE_CONTROL = (float)data1 * ( 401U * 3.3 / 4095U );
                           data1 = vRCFilter(data1, &old);
                           AC_220_VALUE = (float)data1 * ( 401U * 3.3 / 4095U );
                      }
                       HAL_DMA_SetCounter(DMA1_CH1, AC_CONVERION_NUMBER*2);
                       HAL_DMA_Enable(DMA1_CH1);
                       HAL_TiemrEneblae(TIMER3);
                    }
                    if (++ANALOG_DATA_FSM >=10)
                    {
                        ANALOG_DATA_FSM = 0;
                    }
                   break;
            }
    }
}

uint8_t SetI2CDataFSM(I2C_NAME_t I2C,u8 ad, u8 data, I2C_FSM_t * i2cfsm )
{
       switch (*i2cfsm)
       {
           case I2C_GET_BUSY:
               if ((HAL_I2C_GET_STAT2(I2C) & STAR2_BUSY_FLAG ) == 0)
               {
                   HAL_I2C_START(I2C);
                   *i2cfsm = I2C_EVT_5 ;
               }
               break;
           case I2C_EVT_5 :
               if (HAL_I2C_GET_STAT1(I2C) & STAR1_SB_FLAG )
               {
                   HAL_I2C_SEND_ADDR_TRANS( I2C, 0x6D<<1);
                   *i2cfsm =I2C_EVT_6   ;
               }
               break;
           case I2C_EVT_6   :
               if (HAL_I2C_GET_STAT1(I2C) & STAR1_ADDR_FLAG )
               {
                   HAL_I2C_GET_STAT2(I2C);
                   HAL_I2C_SEND(I2C, ad );
                   *i2cfsm =I2C_EVT8;
               }
               break;
           case I2C_EVT8 :
               if (HAL_I2C_GET_STAT1(I2C) & STAR1_TXE_FLAG  )
               {
                   HAL_I2C_SEND(I2C, data);
                   *i2cfsm = I2C_EVT_8_2;
               }
               break;
           case I2C_EVT_8_2:
               if (HAL_I2C_GET_STAT1(I2C) & (STAR1_TXE_FLAG |  STAR1_BTF_FLAG) )
               {
                   HAL_I2C_STOP(I2C);
                   return (1);
               }
               break;
     }
     return (0);
}


uint8_t GetI2CDataFSM(I2C_NAME_t I2C,u8 ad, u8 * temp, I2C_FSM_t * i2cfsm )
{

      switch (*i2cfsm)
      {
          case I2C_GET_BUSY:
              if ((HAL_I2C_GET_STAT2(I2C) & STAR2_BUSY_FLAG ) == 0)
              {
                  HAL_I2C_START(I2C);
                 *i2cfsm = I2C_EVT_5 ;
              }
              break;
         case I2C_EVT_5 :
             if (HAL_I2C_GET_STAT1(I2C) & STAR1_SB_FLAG )
             {
                 HAL_I2C_SEND_ADDR_TRANS( I2C, 0x6D<<1);
                *i2cfsm = I2C_EVT_6   ;
             }
             break;
         case I2C_EVT_6   :
             if (HAL_I2C_GET_STAT1(I2C) & STAR1_ADDR_FLAG )
             {
                 HAL_I2C_GET_STAT2(I2C);
                 HAL_I2C_SEND(I2C, ad );
                *i2cfsm = I2C_START_READ ;
             }
             break;
         case I2C_START_READ :
             if (HAL_I2C_GET_STAT1(I2C) & (STAR1_TXE_FLAG |  STAR1_BTF_FLAG) )
             {
                 HAL_I2C_START(I2C);
                 *i2cfsm = I2C_EVT_5_R;
             }
             break;
         case I2C_EVT_5_R:
             if (HAL_I2C_GET_STAT1(I2C) & STAR1_SB_FLAG )
             {
                 HAL_I2C_SEND_ADDR_RECIEVE(I2C, 0x6D<<1);
                 *i2cfsm =I2C_CHECK_ACK_R;
             }
             break;
         case I2C_CHECK_ACK_R:
             if (HAL_I2C_GET_STAT1(I2C) & STAR1_ADDR_FLAG )
             {
                 HAL_I2C_GET_STAT2(I2C);
                 *i2cfsm = I2C_EVT_6_1 ;
             }
             break;
         case I2C_EVT_6_1 :
             if (HAL_I2C_GET_STAT1(I2C) & STAR1_RXNE_FLAG )
             {
                 HAL_I2C_ACK_DISABLE(I2C);
                 *temp = HAL_I2C_GET_DATA(I2C);
                 HAL_I2C_STOP(I2C);
                 return (1);
             }
             break;
   }
   return 0;
}

#define SENSOR_TIME_OUT 50

static void vSensFSM(u8 channel , SENSOR_FSM_t  * SENS_FSM, I2C_FSM_t * fsm,  u16 * sens_press )
{
    I2C_NAME_t i2c = (channel)? I2C_1:I2C_2;
    u8 status;
    u8 index = channel ? 1: 0;
    switch (*SENS_FSM )
    {
         case SENSOR_START_CONVERSION :
             if (SetI2CDataFSM(i2c, 0x30, 0x0A, fsm) == 1)
             {
                  *SENS_FSM = SENSOR_GET_STATUS;
                  *fsm = I2C_GET_BUSY;
             }
             break;
         case SENSOR_GET_STATUS:
              if (GetI2CDataFSM(i2c, 0x30,&status,fsm) == 1)
              {
                  if ((status & 0x08) == 0 )
                  {
                      *SENS_FSM = SENSOR_GET_PERS_1;
                  }
                  *fsm = I2C_GET_BUSY;
               }
               break;
          case SENSOR_GET_PERS_1:
              if  (GetI2CDataFSM(i2c, 0x06, &i2cdata[index][0],fsm) == 1)
              {
                   *fsm  =I2C_GET_BUSY;
                   *SENS_FSM=SENSOR_GET_PERS_2;
              }
              break;
         case SENSOR_GET_PERS_2:
              if ( GetI2CDataFSM(i2c, 0x07, &i2cdata[index][1],fsm) == 1)
              {
                   *fsm = I2C_GET_BUSY;
                   *SENS_FSM = SENSOR_GET_PERS_3;
              }
              break;
        case SENSOR_GET_PERS_3:
             if  ( GetI2CDataFSM(i2c, 0x08, &i2cdata[index][2], fsm) == 1)
             {
                   uint32_t temp_data = (u32)i2cdata[index][0]<<16 | (u32)i2cdata[index][1]<<8 | i2cdata[index][2];
                   if (temp_data > 0x800000)
                   {
                       *sens_press  = ((temp_data - 16777216)/GetSensCoof());
                   }
                   else
                   {
                       *sens_press  = temp_data/GetSensCoof();
                   }
                   AddBufferDataI2C(&DataBuffer[index], *sens_press  );
                   *SENS_FSM = SENSOR_GET_TEMP_1;
                   *fsm = I2C_GET_BUSY;

                }
                break;
            case SENSOR_GET_TEMP_1:
                if ( GetI2CDataFSM(i2c, 0x09, &i2cdata[index][3],fsm) == 1)
                {
                    *SENS_FSM = SENSOR_GET_TEMP_2;
                    *fsm = I2C_GET_BUSY;
                }
                break;
            case SENSOR_GET_TEMP_2:
                if ( GetI2CDataFSM(i2c, 0x0A, &i2cdata[index][4],fsm) == 1)
                {
                      int16_t temperature;
                      uint16_t sens_temp = i2cdata[index][3]*256 + i2cdata[index][4];
                      if (sens_temp & 0x8000)
                             temperature = (sens_temp -65536);
                      else
                          temperature = sens_temp;
                        AddBufferData(&DataBuffer[index?9:8],temperature);
                        *SENS_FSM = SENSOR_IDLE;
                        *fsm = I2C_GET_BUSY;
                }
                break;
            case SENSOR_IDLE:
                *fsm = I2C_GET_BUSY;;
                break;
        }

}
#define CALIB_COUNT 10
static u8 cla_zero_end = 0;
u8 calibration_zero_flag = 0;
u8 calibration_zero_count = 0;
u16 calib_data[2][CALIB_COUNT];

void CalibrateZeroStart()
{
    calibration_zero_flag = 1;
    cla_zero_end = 0;
}


u8 CalibrationZeroWhait()
{

    return (cla_zero_end);
}

void CalibrateZero()
{
  if (calibration_zero_flag)
  {
      if ( calibration_zero_count < 10 )
      {

          calib_data[0][calibration_zero_count] = sens_press;
          calib_data[1][calibration_zero_count] = sens_press1;
          calibration_zero_count++;
      }
      else
      {
          int32_t temp= 0;
          int32_t temp1 = 0;
          for (u8 i=0;i<CALIB_COUNT;i++)
          {
              temp = temp + calib_data[0][i];
              temp1 = temp1 + calib_data[1][i];
          }
          DataBuffer[0].offset    =  temp  / CALIB_COUNT;
          DataBuffer[1].offset    = temp1 / CALIB_COUNT;
          calibration_zero_flag = 0;
          calibration_zero_count = 0;
          setReg16(SENSOR1_ZERO, DataBuffer[0].offset );
          setReg16(SENSOR2_ZERO, DataBuffer[1].offset );
          cla_zero_end = 1;
      }
  }


}

void I2C_task(void *pvParameters)
{
   // u8 i2c1_error_counter = 0;
   // u8 i2c2_error_counter = 0;
    TickType_t xLastWakeTime;
    SENSOR_FSM_t SENS1_FSM,SENS2_FSM;

    HAL_ResetBit(I2C_EN_PORT, I2C_EN_PIN);
    vTaskDelay(1000);
    HAL_SetBit(I2C_EN_PORT, I2C_EN_PIN);
    vTaskDelay(1000);
    InitI2C();
    I2C_FSM_t fsm  = I2C_GET_BUSY;
    I2C_FSM_t fsm1 = I2C_GET_BUSY;
    u16 sensor_time_out;
    vDataBufferInit();
    while(1)
    {
        xLastWakeTime =  xTaskGetTickCount ();
        SENS1_FSM = SENSOR_START_CONVERSION;
        SENS2_FSM = SENSOR_START_CONVERSION;
        while (1)
        {
          /*  if ( (i2c2_error_counter == 1000) || (i2c1_error_counter  ==1000))
            {

                HAL_ResetBit(I2C_EN_PORT, I2C_EN_PIN);
                vTaskDelay(1000);
                HAL_SetBit(I2C_EN_PORT, I2C_EN_PIN);
                vTaskDelay(1000);
                I2C_SoftwareResetCmd(I2C1,ENABLE);
                I2C_SoftwareResetCmd(I2C2,ENABLE);
              //  vTaskDelay(1000);
               // InitI2C();
                I2C_SoftwareResetCmd(I2C1,DISABLE);
               I2C_SoftwareResetCmd(I2C2,DISABLE);
                i2c2_error_counter = 0;
                i2c1_error_counter = 0;
                SENS1_FSM = SENSOR_START_CONVERSION;
                        SENS2_FSM = SENSOR_START_CONVERSION;
                        printf("i2c_restart\r\n");
                        xLastWakeTime =  xTaskGetTickCount ();
            }
*/
            if ((SENS1_FSM ==SENSOR_IDLE) && (SENS2_FSM ==SENSOR_IDLE))
            {
                vTaskDelay(1);
            }
            sensor_time_out = xTaskGetTickCount () - xLastWakeTime;
            if (sensor_time_out  >25)
            {
                if (SENS1_FSM !=SENSOR_IDLE)
                {
                   // i2c2_error_counter++;
                    HAL_I2C_STOP(I2C_2);
                    fsm = I2C_GET_BUSY;
                }


                if (SENS2_FSM !=SENSOR_IDLE)
                {
                    //i2c1_error_counter++;
                    HAL_I2C_STOP(I2C_1);
                    fsm1 = I2C_GET_BUSY;
                }
                PressSens[0] = ((float)GetConversionali2c(&DataBuffer[0]));
                PressSens[1] = ((float)GetConversionali2c(&DataBuffer[1]));

                break;
            }
            else if (sensor_time_out > 5)
            {
                if (SENS1_FSM ==SENSOR_START_CONVERSION)
                {
                    SENS1_FSM = SENSOR_TIME_OUT;
                }

                if (SENS2_FSM ==SENSOR_START_CONVERSION)
                {
                    SENS2_FSM = SENSOR_TIME_OUT;

                }
            }
            vSensFSM(0,&SENS1_FSM,&fsm,  &sens_press );
            vSensFSM(1,&SENS2_FSM,&fsm1, &sens_press1);

     }
     CalibrateZero();
 }
}





