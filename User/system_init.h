/*
 * system_init.h
 *
 *  Created on: Jul 8, 2024
 *      Author: i.dymov 444
 */

#ifndef USER_SYSTEM_INIT_H_
#define USER_SYSTEM_INIT_H_

#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "list.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "portmacro.h"
#include "stdbool.h"
#include "init.h"
#include "adc.h"
#include "data_model.h"
#include "led.h"
#include "din_dout_task.h"
#include "mb_task.h"
#include "menu.h"
#include "hal_spi.h"
#include "hal_rtc.h"
#include "rtc_task.h"
#include "EEPROM_25C.h"
#include "user_process.h"

#define __SECTION(x)

#define LCD_TASK_PRIOR        7
#define LCD_STK_SIZE        512
#define KEYBAORD_TASK_PRIO     5
#define KEYBAORD_STK_SIZE  128
#define SERIAL_TASK_PRIO     5
#define SERIAL_STK_SIZE  128
#define MBTCP_TASK_PRIO     5
#define MBTCP_STK_SIZE      700
#define MBRTU_TASK_PRIO     5
#define MBRTU_STK_SIZE      512
#define WCHNET_TASK_PRIO     8
#define WCHNET_STK_SIZE     700
#define ADC_TASK_PRIO     6
#define ADC_STK_SIZE      256
#define DEFAULT_TASK_PRIOR          5
#define DEFAULT_TASK_STACK_SIZE         256U
#define USER_TASK_PRIO     5
#define USER_STK_SIZE      256
#define I2C_TASK_PRIO     6
#define I2C_STK_SIZE      256

typedef enum
{
  DEV_FMCH = 0,
  DEV_CDV  = 1,
  DEV_BP   = 2,
} DEVICE_TYPE_t;


typedef enum
{
    STATE_INIT,
    STATE_IDLE,
    STATE_RUN,
    STATE_WHAIT_TO_RAEDY,
    STATE_SAVE_DATA,
} TaskFSM_t;

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize );

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize );
void vSYStaskInit ( void );
void vSYSeventInit ( void );
void vSYSqueueInit ( void );
DEVICE_TYPE_t SystemInitGetDevType();
#endif /* USER_SYSTEM_INIT_H_ */
