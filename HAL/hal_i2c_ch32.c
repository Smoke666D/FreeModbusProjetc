/*
 * hal_i2c_ch32.c
 *
 *  Created on: 1 авг. 2024 г.
 *      Author: i.dymov
 */
#include "hal_i2c.h"

#if MCU == CH32V2

#include "hw_lib_eeprom.h"
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


EEPOROM *   pEEPROM             __SECTION(RAM_SECTION_CCMRAM);

static inline void EEPROM_I2C_ENABLE()          { pEEPROM->dev->CTLR1 |= CTLR1_PE_Set;    }
static inline void EEPROM_I2C_DISABLE()         { pEEPROM->dev->CTLR1 &= CTLR1_PE_Reset;   }
static inline void EEPROM_I2C_STOP()            { pEEPROM->dev->CTLR1 |= CTLR1_STOP_Set;  }
static inline void EEPROM_I2C_START()           { pEEPROM->dev->CTLR1 |= CTLR1_START_Set; }
static inline void I2C_IT_ENABLE( u16 DATA )    { pEEPROM->dev->CTLR2  |= DATA;           }
static inline void I2C_IT_DISABLE( u16 DATA )   { pEEPROM->dev->CTLR2 &= (uint16_t)~DATA; }
static inline void EEPROM_I2C_SEND( u8 DATA)    {pEEPROM->dev->DATAR = DATA;}
static inline void  EERPOM_I2C_SEND_ADDR_TRANS( u8 DATA)   { pEEPROM->dev->DATAR = ( DATA & OADDR1_ADD0_Reset);}
static inline void  EERPOM_I2C_SEND_ADDR_RECIEVE( u8 DATA)  { pEEPROM->dev->DATAR = DATA | OADDR1_ADD0_Set;}
static inline void EEPROM_I2C_ACK_ENABLE()      { pEEPROM->dev->CTLR1 |= CTLR1_ACK_Set;}
static inline void EEPROM_I2C_ACK_DISABLE()     { pEEPROM->dev->CTLR1 &= CTLR1_ACK_Reset;}
#define            EEPROM_I2C_READ_DATA()       pEEPROM->dev->DATAR

#if I2C1_ENABLE == 1
void   I2C1_EV_IRQHandler(void)  __attribute__((interrupt()));  /* USB HP and CAN1 TX */
void   I2C1_ER_IRQHandler(void)  __attribute__((interrupt()));/* USB LP and CAN1RX0 */
#endif
#if I2C2_ENABLE == 1
void   I2C2_EV_IRQHandler(void)  __attribute__((interrupt()));        /* CAN1 RX1 */
void   I2C2_ER_IRQHandler(void)  __attribute__((interrupt()));     /* CAN1 SCE */
#endif



static EERPOM_ERROR_CODE_t I2C_Master_ReviceIT(  u8 DevAdrees, u16 data_addres,  u8 * data, u16 data_size, u32 timeout,uint8_t TNI  );
static EERPOM_ERROR_CODE_t I2C_Master_TransmitIT(  u8 DevAdrees, u16 data_addres, u8 * data, u16 data_size, u32 timeout,uint8_t TNI  );
static EERPOM_ERROR_CODE_t I2C_Master_TransmitFast( u8 DevAdrees, u16 data_addres,  u8 * data, u16 data_size  );


void InitI2CDMA( I2C_NAME_t i2c, uint8_t prior, uint8_t subprior)
{
    I2C_TypeDef *  I2Cx = i2c;
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
   uint16_t  freqrange = 0;
   uint16_t result = 0x04;
   uint32_t pclk1 = 8000000;
   RCC_ClocksTypeDef rcc_clocks;;
   RCC_GetClocksFreq(&rcc_clocks);
   pclk1 = rcc_clocks.PCLK1_Frequency;
   I2Cx->CTLR2 = (uint16_t)(pclk1 / 1000000);;
   I2Cx->CTLR1 &= CTLR1_PE_Reset;
   result = (uint16_t)(pclk1 / ( 400000 * 25));
   result |= I2C_DutyCycle_16_9;
   if((result & CKCFGR_CCR_Set) == 0)
   {
       result |= (uint16_t)0x0001;
   }
   I2Cx->RTR = (uint16_t)(((freqrange * (uint16_t)300) / (uint16_t)1000) + (uint16_t)1);
   I2Cx->CKCFGR = (uint16_t)(result | CKCFGR_FS_Set);
   I2Cx->CTLR1 |= CTLR1_PE_Set;
   I2Cx->CTLR1 |= (uint16_t)((uint32_t)I2C_Mode_I2C | I2C_Ack_Enable);
   I2Cx->OADDR1 = I2C_AcknowledgedAddress_7bit ;
   pEEPROM =xGetEEPROM();
   pEEPROM->dev = i2c;
   pEEPROM->I2C_Master_Recive_func =    I2C_Master_ReviceIT;
   pEEPROM->I2C_Master_Transmit_func =  I2C_Master_TransmitIT;
   pEEPROM->I2C_Master_Transmit_func_fast = I2C_Master_TransmitFast;
   if (i2c == I2C_1)
   {

	   PFIC_IRQ_ENABLE_PG1(I2C1_EV_IRQn ,prior,subprior);
	   PFIC_IRQ_ENABLE_PG1(I2C1_ER_IRQn ,prior,subprior);
   }
   else
   {
       PFIC_IRQ_ENABLE_PG1(I2C2_EV_IRQn ,prior,subprior);
       PFIC_IRQ_ENABLE_PG1(I2C2_ER_IRQn ,prior,subprior);

   }
   I2C_DualAddressCmd(pEEPROM->dev,DISABLE);
   EEPROM_I2C_ENABLE();
}


static EERPOM_ERROR_CODE_t I2C_Master_ReviceIT( u8 DevAdrees, u16 data_addres,  u8 * data, u16 data_size, u32 timeout,uint8_t TNI  )
{
	EERPOM_ERROR_CODE_t res = EEPROM_ACCESS_ERROR;
	pEEPROM->ucTaskNatificationIndex = TNI;
	pEEPROM->Index          = 0;
    pEEPROM->DataLength     = data_size;
	pEEPROM->ReciveBuffer   = data;
	pEEPROM->DevAdrres      = DevAdrees;
	pEEPROM->data_address   = data_addres;
	pEEPROM->NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
	pEEPROM->I2C_State = I2C_MASTER_RECIVE_START;
	EEPROM_I2C_ENABLE();
	while(  pEEPROM->dev->STAR2 & STAR2_BUSY_FLAG  );
	EEPROM_I2C_ACK_ENABLE();
	EEPROM_I2C_START();
	I2C_IT_ENABLE( I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR );
	uint32_t exit_code = ulTaskNotifyTakeIndexed( pEEPROM->ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
	I2C_IT_DISABLE( I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR);
	EEPROM_I2C_DISABLE();
	res = (exit_code == 0x01  )? (EEPROM_OK) : (EEPROM_WRITE_ERROR) ;
    return (res);
}

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



static EERPOM_ERROR_CODE_t I2C_Master_TransmitIT(  u8 DevAdrees, u16 data_addres,  u8 * data, u16 data_size, u32 timeout,uint8_t TNI  )
{
	EERPOM_ERROR_CODE_t res = EEPROM_ACCESS_ERROR;
	uint32_t exit_code;
	pEEPROM->ucTaskNatificationIndex = TNI;
	pEEPROM->Index          = 0;
	pEEPROM->DataLength     = data_size;
    pEEPROM->ReciveBuffer   = data;
	pEEPROM->DevAdrres      = DevAdrees;
    pEEPROM->data_address   = data_addres;
    pEEPROM->NotifyTaskHeandle = xTaskGetCurrentTaskHandle();
    pEEPROM->I2C_State = I2C_MASTER_TRANSMIT_START;
	EEPROM_I2C_ENABLE();
	while(  pEEPROM->dev->STAR2 & STAR2_BUSY_FLAG  );
	I2C_IT_ENABLE( I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR );
	EEPROM_I2C_START();
	exit_code = ulTaskNotifyTakeIndexed( pEEPROM->ucTaskNatificationIndex, 0xFFFFFFFF, timeout );
	I2C_IT_DISABLE( I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR);
	EEPROM_I2C_DISABLE();
	res = (exit_code > 0  )? (EEPROM_OK) : (EEPROM_READ_ERROR);
	return (res) ;
}


static void I2C_FSM()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;


#if MCU == CH32V2
        uint16_t int_flags =   pEEPROM->dev->STAR1;
        switch(pEEPROM->I2C_State)
        {
             case  I2C_MASTER_RECIVE_START:
                 if (int_flags &  STAR1_SB_FLAG)
                 {
                     EERPOM_I2C_SEND_ADDR_TRANS( pEEPROM->DevAdrres );
                     pEEPROM->I2C_State = I2C_MASTER_RECIVE_WRITE_ADDR;
                 }
                 break;
              case I2C_MASTER_RECIVE_WRITE_ADDR:
                 if (int_flags & STAR1_ADDR_FLAG)
                 {
                       int_flags = pEEPROM->dev->STAR2;
                       EEPROM_I2C_SEND( (pEEPROM->data_address >>  8) & 0x1F );
                       pEEPROM->I2C_State = I2C_MASTER_RECIVE_WRITE_ADDR2;
                 }
                 break;
              case  I2C_MASTER_RECIVE_WRITE_ADDR2:
                  if (int_flags & STAR1_TXE_FLAG)
                  {
                      EEPROM_I2C_SEND( pEEPROM->data_address & 0xFF );
                      pEEPROM->I2C_State = I2C_MASTER_RECIVE_DA_WRITE;
                  }
                  break;
              case I2C_MASTER_RECIVE_DA_WRITE:
                  if (int_flags & STAR1_TXE_FLAG)
                  {
                      EEPROM_I2C_START();
                      pEEPROM->I2C_State = I2C_MASTER_RECIVE_ADDR;
                   }
                   break;
              case I2C_MASTER_RECIVE_ADDR:
                  if (int_flags & STAR1_SB_FLAG)
                  {
                      EERPOM_I2C_SEND_ADDR_RECIEVE( pEEPROM->DevAdrres);
                      pEEPROM->I2C_State = I2C_MASTER_RECIVE_MODE;
                  }
                  break;
             case I2C_MASTER_RECIVE_MODE:
                  if (int_flags & STAR1_ADDR_FLAG)
                  {
                      int_flags = pEEPROM->dev->STAR2;
                      pEEPROM->I2C_State = I2C_MASTER_RECIEVE;
                   }
                   break;
            case I2C_MASTER_RECIEVE:
                   if (int_flags & 0x0040)
                   {
                       if (pEEPROM->Index < (pEEPROM->DataLength-1))
                       {
                            pEEPROM->ReciveBuffer[pEEPROM->Index++] = EEPROM_I2C_READ_DATA();
                       }
                       else
                       {
                            EEPROM_I2C_ACK_DISABLE();
                            pEEPROM->ReciveBuffer[pEEPROM->Index++] = EEPROM_I2C_READ_DATA();
                            EEPROM_I2C_STOP();
                            xTaskNotifyIndexedFromISR(pEEPROM->NotifyTaskHeandle, pEEPROM->ucTaskNatificationIndex, 0x01, eSetValueWithOverwrite, &xHigherPriorityTaskWoken  );
                            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
                            pEEPROM->I2C_State = I2C_IDLE;
                        }
                     }
                     break;
                 case  I2C_MASTER_TRANSMIT_START:

                    if (int_flags & STAR1_SB_FLAG)
                    {
                        EERPOM_I2C_SEND_ADDR_TRANS( pEEPROM->DevAdrres );
                        pEEPROM->I2C_State = I2C_MASTER_TRANSMIT_ADDR;
                    }
                     break;
                 case I2C_MASTER_TRANSMIT_ADDR:
                    if  ( int_flags & 0x0400)
                     {
                        I2C_ClearFlag( pEEPROM->dev,I2C_FLAG_AF);
                        EEPROM_I2C_START();
                         pEEPROM->I2C_State = I2C_MASTER_TRANSMIT_START;
                      }
                     else

                    if (int_flags & STAR1_ADDR_FLAG)
                    {
                        int_flags = pEEPROM->dev->STAR2;
                        EEPROM_I2C_SEND( (pEEPROM->data_address >>  8) & 0x1F );
                        pEEPROM->I2C_State = I2C_MASTER_TRANSMIT_ADDR2;
                    }
                     break;

                 case I2C_MASTER_TRANSMIT_ADDR2:
                     if (int_flags & STAR1_TXE_FLAG)
                     {
                         EEPROM_I2C_SEND( pEEPROM->data_address & 0xFF );
                         pEEPROM->I2C_State = I2C_MASTER_TRANSMIT_NEXT;
                     }

                     break;
                 case I2C_MASTER_TRANSMIT_NEXT:
                     if (int_flags & STAR1_TXE_FLAG)
                     {
                        if (pEEPROM->Index < pEEPROM->DataLength )
                        {
                           EEPROM_I2C_SEND( pEEPROM->ReciveBuffer[pEEPROM->Index] );
                           pEEPROM->Index++;
                        }
                        else
                             pEEPROM->I2C_State = I2C_MASTER_TRANSMIT_LAST;
                     }
                     break;
                 case  I2C_MASTER_TRANSMIT_LAST:
                     if (int_flags & STAR1_BTF_FLAG)
                     {
                         EEPROM_I2C_STOP();
                         xTaskNotifyIndexedFromISR(pEEPROM->NotifyTaskHeandle, pEEPROM->ucTaskNatificationIndex, 0x01, eSetValueWithOverwrite, &xHigherPriorityTaskWoken  );
                         portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
                         pEEPROM->I2C_State = I2C_IDLE;
                     }
                     break;
                 default:
                     break;
             }


#endif
}


#if I2C1_ENABLE == 1
void I2C1_EV_IRQHandler( void )
{
	I2C_FSM();
}
void I2C1_ER_IRQHandler ( void )
{
	I2C_FSM();
}
#endif
#if I2C2_ENABLE == 1
void  I2C2_EV_IRQHandler( void )
{
	I2C_FSM();
}
void I2C2_ER_IRQHandler ( void )
{
	I2C_FSM();
}
#endif

#endif
