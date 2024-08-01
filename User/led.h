/*
 * led.h
 *
 *  Created on: Jul 15, 2024
 *      Author: i.dymov
 */

#ifndef USER_LED_H_
#define USER_LED_H_

#include "system_init.h"
#include "u8g2.h"

#define LCD_WIDTH            128U  /* LCD Size */
#define LCD_HEIGHT           64U
#define LCD_DATA_BUFFER_SIZE 1024U

TaskHandle_t * getLCDTaskHandle();
void LCD_task(void *pvParameters);
void vLCDInit();

#endif /* USER_LED_H_ */
