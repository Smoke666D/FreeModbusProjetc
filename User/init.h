/*
 * init.h
 *
 *  Created on: Jul 8, 2024
 *      Author: i.dymov
 */

#ifndef USER_INIT_H_
#define USER_INIT_H_

#include "main.h"
#include "hal_config.h"

#define CRACH_Port                  PORT_C
#define CRACH_Pin                   GPIO_2
#define KL6_Pin                     GPIO_8
#define KL6_Port                    PORT_B
#define KL5_Pin                     GPIO_2
#define KL4_Pin                     GPIO_3
#define KL3_Pin                     GPIO_4
#define KL2_Pin                     GPIO_5
#define KL1_Pin                     GPIO_6
#define KL_Port                     PORT_E
#define AC_SENS1_                 GPIO_0
#define AC_SENS2_                 GPIO_1
#define AC_SENSE_PORT              PORT_B
#define POWER_CONTROL               GPIO_2
#define AIN1_Pin                    GPIO_6
#define AIN2_Pin                    GPIO_7
#define SEN1_POW_AIN1_2_PORT        PORT_A
#define AIN3_Pin                    GPIO_4
#define AIN4_Pin                    GPIO_5
#define AIN3_4_PORT                 PORT_C
#define AIN5_Pin                    GPIO_0
#define AIN6_Pin                    GPIO_1
#define AIN5_6_PORT                 PORT_A
#define DAC0_Pin                    GPIO_4
#define DAC0_Port                   PORT_A
#define DOUT_1_Pin                  GPIO_3
#define DOUT_2_Pin                  GPIO_4
#define DOUT_3_Pin                  GPIO_5
#define DOUT_Port                   PORT_B
#define DIN_1_Pin                   GPIO_11
#define DIN_2_Pin                   GPIO_12
#define DIN_3_Pin                   GPIO_13
#define DIN_4_Pin                   GPIO_14
#define DIN_5_Pin                   GPIO_15
#define DIN_1_5_Port                PORT_E
#define I2C2_SCL_Pin                GPIO_10
#define I2C2_SDA_Pin                GPIO_11
#define I2C2_Port                   PORT_B
#define SPI2_NSS_Pin                GPIO_12
#define SPI2_SCK_Pin                GPIO_13
#define SPI2_MISO_Pin               GPIO_14
#define SPI2_MOSI_Pin               GPIO_15
#define SPI2_Port                   PORT_B
#define CRASH_LED_Pin               Pin GPIO_Pin_8
#define CRASH_LED_Port              PORT_D
#define AOUT1_Pin                   GPIO_9
#define AOUT2_Pin                   GPIO_11
#define AOUT3_Pin                   GPIO_13
#define AOUT_Port                   PORT_D

#define RX1_Pin                     Pin GPIO_Pin_9
#define TX1_Pin                     Pin GPIO_Pin_10
#define UART_Port                   PORT_A
#define ELED2_Pin                   GPIO_10
#define ELED1_Pin                   GPIO_11
#define ELED_Port                   PORT_C



/*LCD PORT */
#define LCDDATA4_Pin                GPIO_7
#define LCDDATA5_Pin                GPIO_8
#define LCDDATA6_Pin                GPIO_9
#define LCDDATA7_Pin                GPIO_10
#define LCDDATA_4_7_Port            PORT_E
#define LCDDni_Pin                  GPIO_12
#define LCDDATA0_Pin                GPIO_14
#define LCDDATA1_Pin                GPIO_15
#define LCDDATA_0_1_n_Port          PORT_D
#define LCDRST_Pin                  GPIO_12
#define LCDRST_Port                 PORT_C
#define LCDDATA2_Pin                GPIO_0
#define LCDDATA3_Pin                GPIO_1
#define LCDnE_Pin                   GPIO_2
#define LCDnRW_Pin                  GPIO_5
#define LCDLED_Pin                  GPIO_6
#define LCDCS_Pin                   GPIO_7
#define LDCDATA_2_3_E_REW_CD_LED_Port PORT_D



#define I2C1_SCL_Pin                GPIO_6
#define I2C1_SDA_Pin                GPIO_7
#define I2C1_Port                   PORT_B

#define RS485_EN_Pin                GPIO_9
#define RS485_EN_Port               PORT_B
#define RS485_TX_Pin                GPIO_0
#define RS485_RX_Pin                GPIO_1
#define RS485_Port                  PORT_E

#define DEF_H_FRONT 10U
#define DEF_L_FRONT 10U
#define RPM_CHANNEL_COUNT     0
#define DIN_CHANNEL_COUNT     5
#define DIN_COUNT            ( RPM_CHANNEL_COUNT + DIN_CHANNEL_COUNT )
#define DOUT_COUNT           3


#define RTC_IT_PRIOR    1
#define RTC_IT_SUBPRIOR 3
#define UART2_IT_PRIOR    1
#define UART2_IT_SUBPRIOR 3

void vInit_DeviceConfig( void );

#endif /* USER_INIT_H_ */
