/*
 * user_process.h
 *
 *  Created on: Aug 21, 2024
 *      Author: i.dymov
 */

#ifndef USER_USER_PROCESS_H_
#define USER_USER_PROCESS_H_

#include "main.h"
#include "system_types.h"
#include "hw_lib_din.h"
#include "data_model.h"





typedef enum
{
  BP_CONFIG = 0,
  ONE_CH    = 1,
  TWO_CH    = 2,

} CHANNEL_COUNT_t;





#define WORK_CODE      2
#define CALIBRATE_CODE 1
#define STOP_CODE  0


float getSETPOINT();
float GetChanne2Setting();
float ComputeSetPoint();
void UPDATE_COOF();
u8 getStateVAV();
u8 getStateDCV();
void SystemCalibraionStop();
void SystemCalibraionStart();
u8 getProcessStateCode();
float getDAC1_Out();
void USER_SetSettingChange();
u8 USER_GerErrorState();
void USER_SetControlState(u8 state);
uint16_t USER_GetSetting();
uint16_t USER_GetFact(u8 * state);
USER_PROCESS_FSM_t USER_GetProccesState( void  );
TaskHandle_t * getUserProcessTaskHandle( void  );
void user_process_task(void *pvParameters);
DISCRET_STATE_t getCurSettingState();
float fGetAnalogSetting();\
/// §¶§å§ß§Ü§è§Ú§ñ §å§ã§ä§Ñ§ß§à§Ó§Ü§Ú §ã§à§ã§ä§à§ß§Ú§ñ §Õ§Ú§ã§Ü§â§Ö§ß§ä§à§Ô§à §Ó§í§ç§à§Õ§Ñ §Ú §á§Ö§â§Ö§Ù§Ñ§á§å§ã§Ü§Ñ §Ú§ß§Õ§Ú§Ü§Ñ§ä§à§â§Ñ, §Ö§ã§Ý§Ú §ã§à§ã§ä§à§ß§Ú§Ö §á§à§Þ§Ö§ß§ñ§Ý§à§ã§î
void user_dout_set(OUT_NAME_TYPE ucCh, uint8_t state);
#endif /* USER_USER_PROCESS_H_ */
