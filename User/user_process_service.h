/*
 * user_process_service.h
 *
 *  Created on: Oct 9, 2024
 *      Author: i.dymov
 */

#ifndef USER_USER_PROCESS_SERVICE_H_
#define USER_USER_PROCESS_SERVICE_H_

#include "system_init.h"
#include "hw_lib_din.h"

#define FILTER_ERROR         0x01
#define SETTING_ERROR        0x02
#define LOW_VOLTAGE_ERROR    0x04
#define HIGH_VOLTAGE_ERROR   0x08
#define DIN_ERROR            0x10
#define FIRST_CHANNEL_ERROR  0x20
#define SECOND_CHANNEL_ERROR 0x40
#define ANALOG_SENSOR_ERROR  0x80

void vCheckDoubleChannelAlarm( u8 *error_state );
void InitCleanTimer();
void CleanTimerFuncton(  DISCRET_STATE_t * state);

#endif /* USER_USER_PROCESS_SERVICE_H_ */
