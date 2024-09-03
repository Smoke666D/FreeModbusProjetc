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
static int16_t sens_press1=0;
int16_t GetConversional(ADC_Conversionl_Buf_t * pBuf);
#define DC_24_BufferSize    3
#define DC_AIN_BufferSize  10
#define Sens_BufferSize_MAX 400
u16 sensor_timer[] = { 4,20,40,80,120,200,400};

#define DAC_CAL_POINT 10

POINT_t const DACCAL[DAC_CAL_POINT]={ {0,0},
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


ADC_Conversionl_Buf_t DataBuffer[DC_CHANNEL+3];


int16_t SenseBuffer1[ Sens_BufferSize_MAX ];
int16_t SenseBuffer2[ Sens_BufferSize_MAX ];
int16_t DC24Buffer[ DC_24_BufferSize];
int16_t AIN1Buffer[ DC_AIN_BufferSize];
int16_t AIN2Buffer[ DC_AIN_BufferSize];
int16_t AIN3Buffer[ DC_AIN_BufferSize];
int16_t AIN4Buffer[ DC_AIN_BufferSize];
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

float getAIN( AIN_CHANNEL_t channel)
{
    switch (channel)
    {
        case  SENS1:
            return  ((float)GetConversional(&DataBuffer[0]));
        case SENS2:
            return  ((float)GetConversional(&DataBuffer[1]));
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
       case DIG2_TEMP:
           return ((float)GetConversional(&DataBuffer[8])/256);

       case DIG2_PRES:
           return (float)(sens_press1);
        case AC220:
            return (AC_220_VALUE);
    }
}

uint8_t getACVoltage()
{
    return ((uint8_t)AC_220_VALUE);
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
    memset(SenseBuffer1,0,Sens_BufferSize_MAX*2);
    DataBuffer[0].ConversionalSize = sensor_timer[getReg16(SENSOR_COUNT )];
    DataBuffer[0].pIndex = 0;
    DataBuffer[0].pBuff = SenseBuffer1;
    memset(SenseBuffer2,0,Sens_BufferSize_MAX*2);
    DataBuffer[1].ConversionalSize = sensor_timer[getReg16(SENSOR_COUNT )];
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
    DataBuffer[8].ConversionalSize = DC_AIN_BufferSize;
     DataBuffer[8].pIndex = 0;
     DataBuffer[8].pBuff = SensTemoBuffer;
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
                            AddBufferData(&DataBuffer[i+1],(int16_t)ADC2_Buffer[i]);   //§©§Ñ§Ü§Ú§Õ§í§Ó§Ñ§Ö§Þ §Ú§ç §Ó §Ò§å§æ§æ§Ö§â
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

static void vSensFSM(u8 channel , SENSOR_FSM_t  * SENS_FSM, I2C_FSM_t * fsm, u8 * status, u16 * sens_press )
{
    I2C_NAME_t i2c = (channel)? I2C_1:I2C_2;
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
              if (GetI2CDataFSM(i2c, 0x30,status,fsm) == 1)
              {
                  if ((*status & 0x08) == 0 )
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
                      uint16_t sens_temp = i2cdata[1][3]*256 + i2cdata[1][4];
                      if (sens_temp & 0x8000)
                             temperature = (sens_temp -65536);
                      else
                          temperature = sens_temp;
                        AddBufferData(&DataBuffer[index?7:8],temperature);
                        *SENS_FSM = SENSOR_IDLE;
                        *fsm = I2C_GET_BUSY;
                }
                break;
            case SENSOR_IDLE:
                *fsm = I2C_GET_BUSY;;
                break;
        }

}


void I2C_task(void *pvParameters)
{
    TickType_t xLastWakeTime;
    u8 status,status1;
    SENSOR_FSM_t SENS1_FSM,SENS2_FSM;
    uint16_t counter_led = 0;
    u8 led_state = 0;
    vTaskDelay(1000);
    I2C_FSM_t fsm,fsm1;
    u16 sensor_time_out;
    printf("Start I2C OK 1.5\r\n");
 while(1)
 {
     if (++counter_led >20)
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
     xLastWakeTime =  xTaskGetTickCount ();
     SENS1_FSM = SENSOR_START_CONVERSION;
     SENS2_FSM = SENSOR_START_CONVERSION;
     fsm = 0;
     while (1)
     {
         if ((SENS1_FSM ==SENSOR_IDLE) && (SENS2_FSM ==SENSOR_IDLE))
         {
             vTaskDelay(1);
         }
         sensor_time_out = xTaskGetTickCount () - xLastWakeTime;
         if (sensor_time_out  >25)
         {
            if (SENS1_FSM !=SENSOR_IDLE)
            {
                 printf("i2c error %i %i\r\n",fsm,SENS1_FSM);
                 HAL_I2C_STOP(I2C_2);
                 fsm = I2C_GET_BUSY;
            }
            if (SENS2_FSM !=SENSOR_IDLE)
            {
                 printf("i21 error %i %i\r\n",fsm1,SENS2_FSM);
                 HAL_I2C_STOP(I2C_1);
                 fsm1 = I2C_GET_BUSY;
            }
            break;

         }
         else if (sensor_time_out > 5)
         {
             if (SENS1_FSM ==SENSOR_START_CONVERSION)
             {
                 SENS1_FSM = SENSOR_IDLE;
                 HAL_I2C_STOP(I2C_2);
             }
             if (SENS2_FSM ==SENSOR_START_CONVERSION)
             {
                 SENS2_FSM = SENSOR_IDLE;
                  HAL_I2C_STOP(I2C_1);
             }


         }
         vSensFSM(0,&SENS1_FSM,&fsm,&status, &sens_press);
         vSensFSM(1,&SENS2_FSM,&fsm1,&status1,&sens_press1);
     }
 }
}





