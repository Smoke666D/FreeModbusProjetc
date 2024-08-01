/*
 * hal_gpio.c
 *
 *  Created on: 9 апр. 2024 г.
 *      Author: i.dymov
 */

#include "hal_gpio.h"

#if MCU == CH32V2 || MCU==CH32V3

#if MCU == CH32V2
    #include "ch32v20x_rcc.h"
    #include "ch32v20x_gpio.h"
#endif
#if MCU == CH32V3
    #include "ch32v30x_rcc.h"
    #include "ch32v30x_gpio.h"
#endif
#include "string.h"

static void HAL_InitRCC(PortName_t PORT );

#if MCU == CH32V2

void HAL_InitGPIO(GPIO_TypeDef *GPIOx,    uint16_t GPIO_Pin, GPIOSpeed_TypeDef GPIO_Speed,
    GPIOMode_TypeDef GPIO_Mode)
{
    uint32_t currentmode = 0x00, currentpin = 0x00, pinpos = 0x00, pos = 0x00;
    uint32_t tmpreg = 0x00, pinmask = 0x00;

    currentmode = ((uint32_t)GPIO_Mode) & ((uint32_t)0x0F);

    if((((uint32_t)GPIO_Mode) & ((uint32_t)0x10)) != 0x00)
    {
        currentmode |= (uint32_t)GPIO_Speed;
    }
#if defined(PORT_C_ENABLE) || defined(DEFAULT_C)
#if defined (CH32V20x_D6)

    if((GPIOx == GPIOC) && (((*(uint32_t *) 0x40022030) & 0x0F000000) == 0)){
        GPIO_Pin = GPIO_Pin >> 13;
    }
#endif
#endif
    if(((uint32_t)GPIO_Pin & ((uint32_t)0x00FF)) != 0x00)
    {
        tmpreg = GPIOx->CFGLR;
        for(pinpos = 0x00; pinpos < 0x08; pinpos++)
        {
            pos = ((uint32_t)0x01) << pinpos;
            currentpin = (GPIO_Pin) & pos;
            if(currentpin == pos)
            {
                pos = pinpos << 2;
                pinmask = ((uint32_t)0x0F) << pos;
                tmpreg &= ~pinmask;
                tmpreg |= (currentmode << pos);
                if(GPIO_Mode == GPIO_Mode_IPD)
                {
                    GPIOx->BCR = (((uint32_t)0x01) << pinpos);
                }
                else
                {
                    if(GPIO_Mode == GPIO_Mode_IPU)
                    {
                        GPIOx->BSHR = (((uint32_t)0x01) << pinpos);
                    }
                }
            }
        }
        GPIOx->CFGLR = tmpreg;
    }

    if(GPIO_Pin > 0x00FF)
    {
        tmpreg = GPIOx->CFGHR;

        for(pinpos = 0x00; pinpos < 0x08; pinpos++)
        {
            pos = (((uint32_t)0x01) << (pinpos + 0x08));
            currentpin = ((GPIO_Pin) & pos);
            if(currentpin == pos)
            {
                pos = pinpos << 2;
                pinmask = ((uint32_t)0x0F) << pos;
                tmpreg &= ~pinmask;
                tmpreg |= (currentmode << pos);
                if(GPIO_Mode == GPIO_Mode_IPD)
                {
                    GPIOx->BCR = (((uint32_t)0x01) << (pinpos + 0x08));
                }
                if(GPIO_Mode == GPIO_Mode_IPU)
                {
                    GPIOx->BSHR = (((uint32_t)0x01) << (pinpos + 0x08));
                }
            }
        }
        GPIOx->CFGHR = tmpreg;
    }
}

#endif
/*
 * API Функция иницилазиации тактирования порта. Сохраняет флаг, что бы не было повторонй инициализации
 */
/*
 * Инициализация прота в режиме выхода
 */
void HAL_InitGpioOut( PortName_t PORT, uint16_t Pin  )
{

#if MCU == CH32V2
    HAL_InitGPIO( PORT, Pin,GPIO_Speed_2MHz, GPIO_Mode_Out_PP );
#else
    GPIO_InitTypeDef GPIO_InitStructure={0};
    GPIO_InitStructure.GPIO_Pin = Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(PORT, &GPIO_InitStructure);
#endif
}

/*
 * Инициализация порта в режиме аналогового входа
 */
void HAL_InitGpioAIN(PortName_t PORT, uint16_t Pin )
{

#if MCU == CH32V2
    HAL_InitGPIO( PORT, Pin,GPIO_Speed_50MHz, GPIO_Mode_AIN );
#else
    GPIO_InitTypeDef GPIO_InitStructure={0};
    GPIO_InitStructure.GPIO_Pin = Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(PORT, &GPIO_InitStructure);
#endif
}

/*
 * Инициализация порта в режиме входа с пулап
 */
void HAL_InitGpioInPUP(PortName_t PORT, uint16_t Pin)
{

#if MCU == CH32V2
    HAL_InitGPIO( PORT, Pin,GPIO_Speed_50MHz, GPIO_Mode_IPU );
#endif
}



/*
 * Инициализация порта в режиме входа
 */
void HAL_InitGpioIn(PortName_t PORT, uint16_t Pin)
{

#if MCU == CH32V2
    HAL_InitGPIO( PORT, Pin,GPIO_Speed_50MHz,GPIO_Mode_IN_FLOATING );
#else
    GPIO_InitTypeDef GPIO_InitStructure={0};
    GPIO_InitStructure.GPIO_Pin = Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(PORT, &GPIO_InitStructure);
#endif
}



/*
 * Иницализация порта в режиме альтернативной функции
 */
void HAL_InitGpioAF(PortName_t PORT, uint16_t Pin, uint32_t AF ,  GPIO_MODE_t mode )
{

    if (AF != 0 )
    {
        RCC->APB2PCENR |= RCC_APB2Periph_AFIO;
        GPIO_PinRemapConfig(AF,ENABLE);
    }
#if MCU == CH32V2
    HAL_InitGPIO( PORT, Pin,GPIO_Speed_50MHz,mode );
#else
    GPIO_InitTypeDef GPIO_InitStructure={0};
    GPIO_InitStructure.GPIO_Pin = Pin;
    GPIO_InitStructure.GPIO_Mode = mode;
    GPIO_Init(PORT, &GPIO_InitStructure);
#endif

}
/*
 * Получить бит
 */
BitState_t HAL_GetBit( PortName_t  port, uint16_t pin)
{
#ifdef PORT_C_ENABLE
    return ( GPIO_ReadInputDataBit(port, pin));
#else
    return (( port->INDR & pin))? (uint8_t)Bit_SET : (uint8_t)Bit_RESET;
#endif
}

/*
 * Функция установки бита порта
 */
void HAL_SetBit(  PortName_t  port, uint16_t pin )
{
#ifdef PORT_C_ENABLE
    GPIO_SetBits(port, pin);
#else
     port->BSHR = pin;
#endif
}

/*
 * Получить бит порта
 */
BitState_t xHAL_GetOutBit( PortName_t  port, uint16_t pin)
{
#ifdef PORT_C_ENABLE
    return ( GPIO_ReadOutputDataBit(port, pin));
#else
    return ( (port->OUTDR & pin) )? (uint8_t)Bit_SET :  (uint8_t)Bit_RESET;
#endif
}

/*
 * Функция сброса бита порта
 */
void HAL_ResetBit(  PortName_t  port, uint16_t pin )
{
#ifdef PORT_C_ENABLE
    GPIO_ResetBits(port, pin);
#else
    port->BCR = pin;
#endif
}
/*
 * Функция инициализации тактирования указанного порта
 */
static void HAL_InitRCC(PortName_t PORT )
{
    if (PORT ==  PORT_A)
        RCC->APB2PCENR |= RCC_APB2Periph_GPIOA;
    else
    if (PORT ==  PORT_B)
        RCC->APB2PCENR |= RCC_APB2Periph_GPIOB;
    else
    if (PORT ==  PORT_C)
        RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;
#if MCU == CH32V3
    else
    if (PORT ==  PORT_D)
        RCC->APB2PCENR |= RCC_APB2Periph_GPIOD;
    else
        RCC->APB2PCENR |= RCC_APB2Periph_GPIOE;
#endif
}


#if MCU == CH32V3
#define GPIO_PORT_COUNT  5
static const uint32_t PERIPH_MASK[GPIO_PORT_COUNT] = { RCC_APB2Periph_GPIOA,RCC_APB2Periph_GPIOB,RCC_APB2Periph_GPIOC,RCC_APB2Periph_GPIOD,RCC_APB2Periph_GPIOE};
#endif
#if MCU == CH32V2
#define GPIO_PORT_COUNT  3
static const uint32_t PERIPH_MASK[GPIO_PORT_COUNT] = { RCC_APB2Periph_GPIOA,RCC_APB2Periph_GPIOB,RCC_APB2Periph_GPIOC};
#endif

 void HAL_InitGPO()
 {
     for (u8 i=0;i< GPIO_PORT_COUNT;i++)
     {
         RCC->APB2PCENR |= PERIPH_MASK[i];
         RCC->APB2PRSTR |= PERIPH_MASK[i];
         RCC->APB2PRSTR &= ~PERIPH_MASK[i];

     }
 }


#endif
