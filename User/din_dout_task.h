/*
 * din_dout_task.h
 *
 *  Created on: Aug 3, 2024
 *      Author: i.dymov
 */

#ifndef USER_DIN_DOUT_TASK_H_
#define USER_DIN_DOUT_TASK_H_

#include "hw_lib_keyboard.h"
#define KEY_COUNT 6

void vDIN_DOUT_Init();
void vKeyboardTask(void  * argument );

#endif /* USER_DIN_DOUT_TASK_H_ */
