/*
 * hal_i2c_ch32.c
 *
 *  Created on: 1 авг. 2024 г.
 *      Author: i.dymov
 */
#include "hal_i2c.h"



#if MCU == CH32V2 || MCU == CH32V3
#include "ch32v30x_i2c.h"
#include "ch32v30x.h"

#include "hal_gpio.h"
#include "hal_irq.h"


static I2C_TypeDef * I2C[]={I2C1,I2C2};
/* I2C START mask */
#define CTLR1_START_Set          ((uint16_t)0x0100)
#define CTLR1_START_Reset        ((uint16_t)0xFEFF)

/* I2C ADD0 mask */
#define OADDR1_ADD0_Set          ((uint16_t)0x0001)
#define OADDR1_ADD0_Reset        ((uint16_t)0xFFFE)
/* I2C SPE mask */
#define CTLR1_PE_Set             ((uint16_t)0x0001)
#define CTLR1_PE_Reset           ((uint16_t)0xFFFE)
/* I2C ACK mask */
#define CTLR1_ACK_Set            ((uint16_t)0x0400)
#define CTLR1_ACK_Reset          ((uint16_t)0xFBFF)

/* I2C ENDUAL mask */
#define OADDR2_ENDUAL_Set        ((uint16_t)0x0001)
#define OADDR2_ENDUAL_Reset      ((uint16_t)0xFFFE)
#define EEPROM_I2C_DUALADDR_DISABLE       I2C[pEEPROM->dev]->OADDR2 &= OADDR2_ENDUAL_Reset
/* I2C STOP mask */
#define CTLR1_STOP_Set           ((uint16_t)0x0200)
#define CTLR1_STOP_Reset         ((uint16_t)0xFDFF)
/* I2C FREQ mask */
#define CTLR2_FREQ_Reset         ((uint16_t)0xFFC0)
/* I2C F/S mask */
#define CKCFGR_FS_Set            ((uint16_t)0x8000)
/* I2C registers Masks */
#define CTLR1_CLEAR_Mask         ((uint16_t)0xFBF5)
/* I2C CCR mask */
#define CKCFGR_CCR_Set           ((uint16_t)0x0FFF)
/* I2C FLAG mask */
#define FLAG_Mask                ((uint32_t)0x00FFFFFF)




void HAL_I2C_ENABLE( I2C_NAME_t i2c )          { I2C[i2c]->CTLR1 |= CTLR1_PE_Set;    }
static inline void EEPROM_I2C_DISABLE(I2C_NAME_t i2c )         { I2C[i2c]->CTLR1 &= CTLR1_PE_Reset;   }
static inline void EEPROM_I2C_STOP(I2C_NAME_t i2c )            { I2C[i2c]->CTLR1 |= CTLR1_STOP_Set;  }
static inline void EEPROM_I2C_START(I2C_NAME_t i2c )           { I2C[i2c]->CTLR1 |= CTLR1_START_Set; }
static inline void I2C_IT_ENABLE(I2C_NAME_t i2c ,u16 DATA )    { I2C[i2c]->CTLR2  |= DATA;           }
static inline void I2C_IT_DISABLE(I2C_NAME_t i2c , u16 DATA )   {I2C[i2c]->CTLR2 &= (uint16_t)~DATA; }
static inline void EEPROM_I2C_SEND(I2C_NAME_t i2c , u8 DATA)    {I2C[i2c]->DATAR = DATA;}
static inline void  EERPOM_I2C_SEND_ADDR_TRANS(I2C_NAME_t i2c ,u8 DATA)   { I2C[i2c]->DATAR = ( DATA & OADDR1_ADD0_Reset);}
static inline void  EERPOM_I2C_SEND_ADDR_RECIEVE(I2C_NAME_t i2c , u8 DATA)  { I2C[i2c]->DATAR = DATA | OADDR1_ADD0_Set;}
static inline void EEPROM_I2C_ACK_ENABLE(I2C_NAME_t i2c )      { I2C[i2c]->CTLR1 |= CTLR1_ACK_Set;}
static inline void EEPROM_I2C_ACK_DISABLE(I2C_NAME_t i2c )     { I2C[i2c]->CTLR1 &= CTLR1_ACK_Reset;}
#define            EEPROM_I2C_READ_DATA(i2c )      I2C[i2c]->DATAR

#if I2C1_ENABLE == 1
void   I2C1_EV_IRQHandler(void)  __attribute__((interrupt()));  /* USB HP and CAN1 TX */
void   I2C1_ER_IRQHandler(void)  __attribute__((interrupt()));/* USB LP and CAN1RX0 */
#endif
#if I2C2_ENABLE == 1
void   I2C2_EV_IRQHandler(void)  __attribute__((interrupt()));        /* CAN1 RX1 */
void   I2C2_ER_IRQHandler(void)  __attribute__((interrupt()));     /* CAN1 SCE */
#endif

static I2C_TypeDef_T I2CConfig[2];

//static EERPOM_ERROR_CODE_t I2C_Master_ReviceIT(  u8 DevAdrees, u16 data_addres,  u8 * data, u16 data_size, u32 timeout,uint8_t TNI  );
//static EERPOM_ERROR_CODE_t I2C_Master_TransmitIT(  u8 DevAdrees, u16 data_addres, u8 * data, u16 data_size, u32 timeout,uint8_t TNI  );
//static EERPOM_ERROR_CODE_t I2C_Master_TransmitFast( u8 DevAdrees, u16 data_addres,  u8 * data, u16 data_size  );


void HAL_I2C_InitIT( I2C_NAME_t i2c, HAL_I2C_InitTypeDef * I2C_InitStruct, uint8_t prior, uint8_t subprior)
{
    ;
#if I2C1_ENABLE == 1
    if ( i2c == I2C_1)
    {
        RCC->APB1PCENR |= RCC_APB1Periph_I2C1;
        RCC->APB1PRSTR |= RCC_APB1Periph_I2C1;
        RCC->APB1PRSTR &= ~RCC_APB1Periph_I2C1;
    }
#endif
#if I2C2_ENABLE == 1
    if ( i2c == I2C_2)
    {
        RCC->APB1PCENR |= RCC_APB1Periph_I2C2;
        RCC->APB1PRSTR |= RCC_APB1Periph_I2C2;
        RCC->APB1PRSTR &= ~RCC_APB1Periph_I2C2;

    }
#endif
    uint16_t tmpreg = 0, freqrange = 0;
    uint16_t result = 0x04;
    uint32_t pclk1 = 8000000;
    RCC_ClocksTypeDef rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);
    pclk1 = rcc_clocks.PCLK1_Frequency;
    freqrange = (uint16_t)(pclk1 / 1000000);
    I2C[i2c]->CTLR2 = freqrange;
    I2C[i2c]->CTLR1 &= CTLR1_PE_Reset;
    tmpreg = 0;

    if(I2C_InitStruct->I2C_ClockSpeed <= 100000)
    {
        result = (uint16_t)(pclk1 / (I2C_InitStruct->I2C_ClockSpeed << 1));
        if(result < 0x04)
        {
           result = 0x04;
        }
        tmpreg |= result;
        I2C[i2c]->RTR = freqrange + 1;
    }
    else
    {
        if(I2C_InitStruct->I2C_DutyCycle == I2C_DutyCycle_2)
        {
            result = (uint16_t)(pclk1 / (I2C_InitStruct->I2C_ClockSpeed * 3));
        }
        else
        {
            result = (uint16_t)(pclk1 / (I2C_InitStruct->I2C_ClockSpeed * 25));
            result |= I2C_DutyCycle_16_9;
        }
        if((result & CKCFGR_CCR_Set) == 0)
        {
            result |= (uint16_t)0x0001;
        }
        tmpreg |= (uint16_t)(result | CKCFGR_FS_Set);
        I2C[i2c]->RTR = (uint16_t)(((freqrange * (uint16_t)300) / (uint16_t)1000) + (uint16_t)1);
    }
    I2C[i2c]->CKCFGR = tmpreg;
    I2C[i2c]->CTLR1 |= CTLR1_PE_Set;
    tmpreg = I2C[i2c]->CTLR1;
    tmpreg &= CTLR1_CLEAR_Mask;
    tmpreg |= (uint16_t)((uint32_t)I2C_InitStruct->I2C_Mode | I2C_InitStruct->I2C_Ack);
    I2C[i2c]->CTLR1 = tmpreg;
    I2C[i2c]->OADDR1 = (I2C_InitStruct->I2C_AcknowledgedAddress | I2C_InitStruct->I2C_OwnAddress1);


  // pEEPROM->I2C_Master_Recive_func =    I2C_Master_ReviceIT;
 //  pEEPROM->I2C_Master_Transmit_func =  I2C_Master_TransmitIT;
 //  pEEPROM->I2C_Master_Transmit_func_fast = I2C_Master_TransmitFast;
  /*if (i2c == I2C_1)
   {
	   PFIC_IRQ_ENABLE_PG2(I2C1_EV_IRQn ,prior,subprior);
	   PFIC_IRQ_ENABLE_PG2(I2C1_ER_IRQn ,prior,subprior);
   }
   else
   {
       PFIC_IRQ_ENABLE_PG2(I2C2_EV_IRQn ,prior,subprior);
       PFIC_IRQ_ENABLE_PG2(I2C2_ER_IRQn ,prior,subprior);

   }*/
}


uint16_t HAL_I2C_GET_STAT1(I2C_NAME_t i2c)
{
    return (I2C[i2c]->STAR1);
}

uint16_t HAL_I2C_GET_STAT2(I2C_NAME_t i2c)
{
    return (I2C[i2c]->STAR2);
}


 void HAL_I2C_SEND(I2C_NAME_t i2c , u8 DATA)    {I2C[i2c]->DATAR = DATA;}
 void HAL_I2C_SEND_ADDR_TRANS(I2C_NAME_t i2c ,u8 DATA)   { I2C[i2c]->DATAR = ( DATA & OADDR1_ADD0_Reset);}
 void HAL_I2C_SEND_ADDR_RECIEVE(I2C_NAME_t i2c , u8 DATA)  { I2C[i2c]->DATAR = DATA | OADDR1_ADD0_Set;}
 void HAL_I2C_ACK_ENABLE(I2C_NAME_t i2c )      { I2C[i2c]->CTLR1 |= CTLR1_ACK_Set;}
 void HAL_I2C_ACK_DISABLE(I2C_NAME_t i2c )     { I2C[i2c]->CTLR1 &= CTLR1_ACK_Reset;}
 void HAL_I2C_STOP(I2C_NAME_t i2c )            { I2C[i2c]->CTLR1 |= CTLR1_STOP_Set;  }
 void HAL_I2C_START(I2C_NAME_t i2c )           { I2C[i2c]->CTLR1 |= CTLR1_START_Set; }


uint16_t HAL_GetI2CBusy( I2C_NAME_t i2c)
{
    u16 data;
    HAL_I2C_ENABLE(i2c);
    data = I2C[i2c]->STAR2 & STAR2_BUSY_FLAG;
    EEPROM_I2C_DISABLE(i2c);
    return (data );
}

EERPOM_ERROR_CODE_t I2C_Master_ReviceIT( I2C_NAME_t i2c, u8 DevAdrees, u16 data_addres,  u8 * data, u16 data_size, u32 timeout,uint8_t TNI  )
{
	EERPOM_ERROR_CODE_t res = EEPROM_ACCESS_ERROR;
	I2CConfig[i2c].ucTaskNatificationIndex = TNI;
	I2CConfig[i2c].Index          = 0;
	I2CConfig[i2c].DataLength     = data_size;
	I2CConfig[i2c].ReciveBuffer   = data;
	I2CConfig[i2c].DevAdrres      = DevAdrees;
	I2CConfig[i2c].data_address   = data_addres;
	I2CConfig[i2c].NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
	I2CConfig[i2c].I2C_State = I2C_MASTER_RECIVE_START;
	HAL_I2C_ENABLE(i2c);;
while(  I2C[i2c]->STAR2 & STAR2_BUSY_FLAG  );
	EEPROM_I2C_ACK_ENABLE(i2c);
	EEPROM_I2C_START(i2c);
	I2C_IT_ENABLE(i2c, I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR );
	uint32_t exit_code = ulTaskNotifyTakeIndexed( I2CConfig[i2c].ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
	I2C_IT_DISABLE( i2c,I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR);
	EEPROM_I2C_DISABLE(i2c);
	res = (exit_code == 0x01  )? (EEPROM_OK) : (EEPROM_WRITE_ERROR) ;
    return (res);
}



I2C_ERROR_CODE_t I2C_Master_TransmitIT( I2C_NAME_t i2c,  u8 DevAdrees, u16 data_addres,  u8 * data, u16 data_size, u32 timeout,uint8_t TNI  )
{
    I2C_ERROR_CODE_t res;
	uint32_t exit_code;
	I2CConfig[i2c].ucTaskNatificationIndex = TNI;
	I2CConfig[i2c].Index          = 0;
	I2CConfig[i2c].DataLength     = data_size;
	I2CConfig[i2c].ReciveBuffer   = data;
	I2CConfig[i2c].DevAdrres      = DevAdrees;
	I2CConfig[i2c].data_address   = data_addres;
	I2CConfig[i2c].NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
	I2CConfig[i2c].I2C_State = I2C_MASTER_TRANSMIT_START;
	HAL_I2C_ENABLE(i2c);
	while( I2C[i2c]->STAR2 & STAR2_BUSY_FLAG  );
	I2C_IT_ENABLE( i2c, I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR );
	EEPROM_I2C_START(i2c);
	exit_code = ulTaskNotifyTakeIndexed( I2CConfig[i2c].ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
	I2C_IT_DISABLE( i2c,I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR);
	EEPROM_I2C_DISABLE(i2c);
	res = (exit_code == 1  )? (HAL_I2C_OK) : (HAL_I2C_ERROR);
	return (res) ;
}



/*
static EERPOM_ERROR_CODE_t I2C_Master_TransmitFast( u8 DevAdrees, u16 data_addres,  u8 * data, u16 data_size  )
{
   EEPROM_I2C_ENABLE();
   while(  pEEPROM->dev->STAR2 & STAR2_BUSY_FLAG  );
   EEPROM_I2C_START();
   while( !( pEEPROM->dev->STAR1 & STAR1_SB_FLAG )) ;

   EERPOM_I2C_SEND_ADDR_TRANS( pEEPROM->DevAdrres );
   while( !( pEEPROM->dev->STAR1 & STAR1_ADDR_FLAG ) );
   u16 int_flags = pEEPROM->dev->STAR2;
   EEPROM_I2C_SEND(  (u8)((data_addres >>  8) & 0x1F ) );
   while( !( pEEPROM->dev->STAR1 & STAR1_TXE_FLAG ) );
   EEPROM_I2C_SEND(  data_addres & 0xFF );
   while( !( pEEPROM->dev->STAR1 & STAR1_TXE_FLAG ) );
   for (u8 i = 0;i<data_size;i++)
   {
       EEPROM_I2C_SEND( data[i]);
       while( !( pEEPROM->dev->STAR1 & STAR1_TXE_FLAG ) );
   }
   EEPROM_I2C_STOP();
   return (EEPROM_OK);
}


*/

static void I2C_FSM( I2C_NAME_t i2c )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    uint16_t int_flags =   I2C[i2c]->STAR1;
     switch(I2CConfig[i2c].I2C_State)
        {
             case  I2C_MASTER_RECIVE_START:
                 if (int_flags &  STAR1_SB_FLAG)
                 {

                     EERPOM_I2C_SEND_ADDR_TRANS( i2c,I2CConfig[i2c].DevAdrres );
                     I2CConfig[i2c].I2C_State = I2C_MASTER_RECIVE_WRITE_ADDR;
                 }
                 break;
              case I2C_MASTER_RECIVE_WRITE_ADDR:


                 if (int_flags & STAR1_ADDR_FLAG)
                 {
                       int_flags = I2C[i2c]->STAR2;
                       EEPROM_I2C_SEND( i2c, I2CConfig[i2c].data_address  );
                       I2CConfig[i2c].I2C_State = I2C_MASTER_RECIVE_DA_WRITE;
                 }
                 break;
              case I2C_MASTER_RECIVE_DA_WRITE:
                  if (int_flags & STAR1_TXE_FLAG)
                  {
                      EEPROM_I2C_START(i2c);
                      I2CConfig[i2c].I2C_State = I2C_MASTER_RECIVE_ADDR;
                   }
                   break;
              case I2C_MASTER_RECIVE_ADDR:
                  if (int_flags & STAR1_SB_FLAG)
                  {
                      EERPOM_I2C_SEND_ADDR_RECIEVE(i2c, I2CConfig[i2c].DevAdrres);
                      I2CConfig[i2c].I2C_State = I2C_MASTER_RECIVE_MODE;
                  }
                  break;
             case I2C_MASTER_RECIVE_MODE:
                  if (int_flags & STAR1_ADDR_FLAG)
                  {
                      int_flags = I2C[i2c]->STAR2;
                      I2CConfig[i2c].I2C_State = I2C_MASTER_RECIEVE;
                   }
                   break;
            case I2C_MASTER_RECIEVE:
                   if (int_flags & 0x0040)
                   {
                       if (I2CConfig[i2c].Index < (I2CConfig[i2c].DataLength-1))
                       {
                           I2CConfig[i2c].ReciveBuffer[ I2CConfig[i2c].Index++] = EEPROM_I2C_READ_DATA(i2c );
                       }
                       else
                       {
                            EEPROM_I2C_ACK_DISABLE(i2c);
                            I2CConfig[i2c].ReciveBuffer[ I2CConfig[i2c].Index++] = EEPROM_I2C_READ_DATA(i2c);
                            EEPROM_I2C_STOP(i2c);
                            xTaskNotifyIndexedFromISR(I2CConfig[i2c].NotifyTaskHeandle, I2CConfig[i2c].ucTaskNatificationIndex, 0x01, eSetValueWithOverwrite, &xHigherPriorityTaskWoken  );
                            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
                            I2CConfig[i2c].I2C_State = I2C_IDLE;
                        }
                     }
                     break;
                 case  I2C_MASTER_TRANSMIT_START:
                    if (int_flags & STAR1_SB_FLAG)
                    {
                        EERPOM_I2C_SEND_ADDR_TRANS( i2c,I2CConfig[i2c].DevAdrres);
                        I2CConfig[i2c].I2C_State = I2C_MASTER_TRANSMIT_ADDR;
                    }
                     break;
                 case I2C_MASTER_TRANSMIT_ADDR:

                    if (int_flags & STAR1_ADDR_FLAG)
                    {

                        int_flags = I2C[i2c]->STAR2;
                        EEPROM_I2C_SEND( i2c, I2CConfig[i2c].data_address  );
                        I2CConfig[i2c].I2C_State = I2C_MASTER_TRANSMIT_NEXT;
                    }
                     break;
                 case I2C_MASTER_TRANSMIT_NEXT:
                     if (int_flags & STAR1_TXE_FLAG)
                     {
                        if (I2CConfig[i2c].Index < I2CConfig[i2c].DataLength )
                        {

                           EEPROM_I2C_SEND(i2c, I2CConfig[i2c].ReciveBuffer[I2CConfig[i2c].Index] );
                           I2CConfig[i2c].Index++;
                        }
                        else
                            I2CConfig[i2c].I2C_State = I2C_MASTER_TRANSMIT_LAST;
                     }
                     break;
                 case  I2C_MASTER_TRANSMIT_LAST:
                     if (int_flags & STAR1_BTF_FLAG)
                     {
                         EEPROM_I2C_STOP(i2c);

                         xTaskNotifyIndexedFromISR(I2CConfig[i2c].NotifyTaskHeandle, I2CConfig[i2c].ucTaskNatificationIndex, 0x01, eSetValueWithOverwrite, &xHigherPriorityTaskWoken  );
                         portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
                         I2CConfig[i2c].I2C_State = I2C_IDLE;
                     }
                     break;
                 default:
                     break;
             }



}

void I2C_ERROR( I2C_NAME_t i2c)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    EEPROM_I2C_STOP(i2c);

                            xTaskNotifyIndexedFromISR(I2CConfig[i2c].NotifyTaskHeandle, I2CConfig[i2c].ucTaskNatificationIndex, 0x02, eSetValueWithOverwrite, &xHigherPriorityTaskWoken  );
                            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
                            I2CConfig[i2c].I2C_State = I2C_IDLE;

}

#if I2C1_ENABLE == 1
void I2C1_EV_IRQHandler( void )
{
	I2C_FSM( I2C_1);
}
void I2C1_ER_IRQHandler ( void )
{
    I2C_ERROR(I2C_1 );
}
#endif
#if I2C2_ENABLE == 1
void  I2C2_EV_IRQHandler( void )
{
	I2C_FSM(I2C_2);
}
void I2C2_ER_IRQHandler ( void )
{
    I2C_ERROR(I2C_2);
}
#endif

#endif
