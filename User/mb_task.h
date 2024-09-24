/*
 * mb_task.h
 *
 *  Created on: Aug 4, 2024
 *      Author: i.dymov
 */

#ifndef USER_MB_TASK_H_
#define USER_MB_TASK_H_


#include "mb.h"
#include "user_process.h"

#define REG_COILS_START      0x0
#define REG_COILS_NREGS      3
#define REG_DISCRETE_START   0x00
#define REG_DISCRETE_NREGS   5

u8 MB_TASK_GetMode();
eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode );
void MBTCP_task(void *pvParameters);
void MBRTU_task(void *pvParameters);
uint16_t GetSensCoof();
#endif /* USER_MB_TASK_H_ */
