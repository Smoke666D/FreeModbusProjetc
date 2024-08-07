/*
 * hw_lib_din.h
 *
 *  Created on: 9 апр. 2024 г.
 *      Author: i.dymov
 */

#ifndef HW_LIB_HW_LIB_DIN_H_
#define HW_LIB_HW_LIB_DIN_H_

#include "main.h"
#include "string.h"
#include "init.h"
#include "filters.h"

#define DIN_PARAM_CHECK 0


#if PLATFORM == APM32
   #define BIT_t   uint8_t
   #define FLAG_t  uint8_t
#endif


#define CC_BUFFER_SIZE 21


typedef enum
{
    RPM_CH1 = 0,
    RPM_CH2 = 1,
} RPM_CHANNEL;

typedef enum
{
   RPM_VALID =1,
   RPM_INVALID = 0,
} RPM_VALID_t;


typedef enum {
    DIN_CONFIG_NEGATIVE = 0U,
    DIN_CONFIG_POSITIVE = 1U,
    RPM_CONFIG = 2U,
	DIN_IDLE   = 4,
} DIN_INPUT_TYPE;


typedef enum {
    DOUT_ACTIVE_CONFIG_NEGATIVE = 0U,
    DOUT_ACTIVE_CONFIG_POSITIVE = 1U,
} OUT_LEVEL_TYPE;

typedef enum
{
 DIN_CONFIG_OK            = 0,
 DIN_WRONG_CHANNEL_NUMBER = 1,
 DIN_CHANGE               = 2,
 DIN_NOT_CHANGE           = 3
} DIN_FUNCTION_ERROR_t;

typedef enum {
	OUT_1 = 0,
	OUT_2 = 1,
	OUT_3 = 2,
	OUT_4 = 3,
	OUT_5 = 4,
	OUT_6 = 5,
	OUT_7 = 6,
	OUT_8 = 7,
	OUT_9 = 8,
	OUT_10 = 9,
	OUT_11 = 10,
	OUT_12 = 11,
	OUT_13 = 12,
	OUT_14 = 13,
	OUT_15 = 14,
	OUT_16 = 15,
	OUT_17 = 16,
	OUT_18 = 17,
	OUT_19 = 18,
	OUT_20 = 19,
} OUT_NAME_TYPE;

typedef struct
{
   u8 index;
   u16 Data[CC_BUFFER_SIZE];
   u32 BufferData;
   RPM_VALID_t bValidFlag;
   float config_coof;
} RPM_Struct;


typedef struct DoutConfigDef_t
{
    uint8_t ucValue;
    OUT_LEVEL_TYPE ucActiveLevel;
    void (*setPortCallback)(OUT_NAME_TYPE ,  BIT_t);
} DoutCinfig_t;


typedef struct DinConfigDef_t
{
    RPM_Struct *  RPMDATA;
    DIN_INPUT_TYPE eInputType;
    uint32_t      ulCounter;
    uint32_t      ulLowCounter;
    uint32_t      ulHighCounter;
    uint8_t       ucValue;
    uint8_t       ucReadBit;
    uint8_t       ucTempValue;
    FLAG_t (*getPortCallback)(uint8_t );
} DinConfig_t;

typedef enum {
        INPUT_1 = 0U,
        INPUT_2 = 1U,
        INPUT_3 = 2U,
        INPUT_4 = 3U,
        INPUT_5 = 4U,
        INPUT_6 = 5U,
        INPUT_7 = 6U,
        INPUT_8 = 7U,
        INPUT_9 = 8U,
        INPUT_10 = 9U,
        INPUT_11 = 10U,
        INPUT_12 = 11U,
        INPUT_13 = 12U,
        INPUT_14 = 13U,
        INPUT_15 = 14U,
        INPUT_16 = 15U,
        INPUT_17 = 16U,
        INPUT_18 = 17U,
        INPUT_19 = 18U,
        INPUT_20 = 19U
} DIN_INPUT_NAME;


void xSetOut( uint8_t * data_mask);
void xGetOut( uint8_t * data_mask);
void xGetDins( uint8_t * data_mask);
//#define  ucDinGet( a )  uGetDIN( a )
uint8_t eGetDOUT(OUT_NAME_TYPE ucCh );
void InitDinStcurt();
uint8_t uGetDIN(DIN_INPUT_NAME ucCh);
void RMPDataConvert(DIN_INPUT_NAME ucCh);
DIN_INPUT_TYPE xGetDinType(DIN_INPUT_NAME ucCh);
void vRecinfigDin(  DIN_INPUT_NAME ucCh, DinConfig_t * config    );
void vDinInitStateProcess();
void InitFilters();
void vSetRPMConfig(uint8_t ch, float coof, float filter_coof);
DIN_FUNCTION_ERROR_t eDOUTConfigWtihStruct(OUT_NAME_TYPE ucCh, DoutCinfig_t * config);
uint8_t eGrtDUT(OUT_NAME_TYPE ucCh);
DIN_FUNCTION_ERROR_t eSetDUT(OUT_NAME_TYPE ucCh, uint8_t state );
DIN_FUNCTION_ERROR_t xGetRPM( DIN_INPUT_NAME ucCh, uint16_t * data);
uint16_t GetRPM( DIN_INPUT_NAME ucCh );
void vAddRPMData(DIN_INPUT_NAME ucCh, uint16_t data  );
DIN_FUNCTION_ERROR_t xGetDIN(DIN_INPUT_NAME ucCh, uint8_t * state);
u16 * uGetRPMBuffer(DIN_INPUT_NAME ucCh);
void vDinDoutProcess( void );
uint8_t ucDinGet( DIN_INPUT_NAME eChNum );
DIN_FUNCTION_ERROR_t eDinConfigWtihStruct(DIN_INPUT_NAME ucCh, DinConfig_t * config);
void RMPDataConvert(DIN_INPUT_NAME ucCh);
DIN_FUNCTION_ERROR_t eRPMConfig(DIN_INPUT_NAME ucCh, uint8_t ucRPM);
uint32_t uiGetDinMask();

#endif /* HW_LIB_HW_LIB_DIN_H_ */
