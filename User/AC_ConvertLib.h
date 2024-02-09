/*
 * AC_ConvertLib.h
 *
 *  Created on: Feb 8, 2024
 *      Author: i.dymov
 */

#ifndef USER_AC_CONVERTLIB_H_
#define USER_AC_CONVERTLIB_H_

#include "main.h"
#include "math.h"

#define A 220
uint16_t vRCFilter( uint16_t input,uint16_t * old_output);

#define MAX_ZERO_POINT 20
#define FASE_DETECT_HISTERESIS  30

void vADCFindFreq(int16_t * data, uint16_t * count, uint8_t off, int16_t AMP);
float  xADCRMS(int16_t * source, uint16_t size, uint8_t cc );
//���֧�ӧڧ�ߧ�� ���ߧܧ�ڧ� ��ѧҧ��� �� ��֧�֧ާ֧ߧߧ�� �ߧѧ���ا֧ߧڧ֧�
void vDecNetural(int16_t * data);
int16_t  xADCMax( int16_t * source,  uint16_t * delay );

#endif /* USER_AC_CONVERTLIB_H_ */
