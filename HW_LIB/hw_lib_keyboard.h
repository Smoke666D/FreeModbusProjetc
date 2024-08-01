/*
 * hw_lib_keyboard.h
 *
 *  Created on: 14 мая 2024 г.
 *      Author: i.dymov
 */

#ifndef HW_LIB_HW_LIB_KEYBOARD_H_
#define HW_LIB_HW_LIB_KEYBOARD_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "string.h"
#include "event_groups.h"
#include "hal_gpio.h"

/*------------------------ Define --------------------------------------*/
/* Константа определяющая количесвто клавиш, которые обрабатываем драйвер */

#define KEY_READY	         0x0001U
#define KEY_ON_MESSAGE     0x0001U
#define KEY_OFF_MESSAGE    0x0002U
#define KEY_ON_STATE       ( HAL_BIT_RESET  )
#define KEY_OFF_STATE      ( HAL_BIT_SET )
#define KEY_PEREOD         150U
#define SWITCHONDELAY      30U
#define DefaultDelay       600U
#define DefaultRepeatRate  400U
#define BRAKECODE          0x40U
#define MAKECODE           0x80U
#define KEY_OFF            0x00U
#define KEY_ON             0X01U
#define KEY_ON_REPEAT      0x02U

#define kl1_key            0x00U
#define kl2_key            0x01U
#define kl3_key            0x02U
#define kl4_key            0x03U
#define kl5_key            0x04U
#define kl6_key            0x05U
#define kl7_key            0x06U
#define kl8_key            0x07U
#define time_out           0xFFU

#define USE_KEY_TIME_OUT       0

#define  KEY_TIME_OUT      1000U


typedef struct
{
  uint8_t  KeyCode;
  uint8_t  Status;
} KeyEvent;



typedef struct
{
  uint8_t KEYBOARD_COUNT;
  uint8_t *STATUS;
  uint8_t *COUNTERS;
  uint16_t KEYBOARD_PERIOD;
  uint8_t REPEAT_TIME;
  uint8_t KEYDOWN_HOLD_TIME;
  uint8_t KEYDOWN_DELAY;
  BitState_t (*getPortCallback)(uint8_t);
} KeybaordStruct_t;


typedef enum
{
  KEYBOARD_INIT_OK = 0,
  KEYBORAD_PARAM_ERROR = 1,
} KEYBOARD_INIT_CODES;

/*---------------------------- Structures --------------------------------------*/



/*----------------------------- Functions ------------------------------------*/
KEYBOARD_INIT_CODES eKeyboardInit( KeybaordStruct_t * kis );
QueueHandle_t * xKeyboardQueue( void );
uint16_t HW_LIB_GetKeyboardPeriod();
void HW_LIB_KeyboradFSM();

#endif /* HW_LIB_HW_LIB_KEYBOARD_H_ */
