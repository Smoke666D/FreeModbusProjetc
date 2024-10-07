 /*
------------------------------------------------------------------------------
~ File   : pid.c
~ Author : Majid Derhambakhsh
~ Version: V1.0.0
~ Created: 02/11/2021 03:43:00 AM
~ Brief  :
~ Support:
		   E-Mail : Majid.do16@gmail.com (subject : Embedded Library Support)

		   Github : https://github.com/Majid-Derhambakhsh
------------------------------------------------------------------------------
~ Description:

~ Attention  :

~ Changes    :
------------------------------------------------------------------------------
*/

#include "pid.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~ Initialize ~~~~~~~~~~~~~~~~ */
void PID_Init(PID_TypeDef *uPID, float LastOut, float LastInput)
{
	/* ~~~~~~~~~~ Set parameter ~~~~~~~~~~ */
	uPID->OutputSum = LastOut;
	uPID->LastInput = LastInput;
	
	if (uPID->OutputSum > uPID->OutMax)
	{
		uPID->OutputSum = uPID->OutMax;
	}
	else if (uPID->OutputSum < uPID->OutMin)
	{
		uPID->OutputSum = uPID->OutMin;
	}
	else { }

}

void PID(PID_TypeDef *uPID,  float *Output, float *Setpoint, float Kp, float Ki, float Kd,  PIDCD_TypeDef ControllerDirection)
{
	/* ~~~~~~~~~~ Set parameter ~~~~~~~~~~ */
	uPID->MyOutput   = Output;
	uPID->MySetpoint = Setpoint;
	

	PID_SetOutputLimits(uPID, 0, _PID_8BIT_PWM_MAX);
	
	uPID->SampleTime = _PID_SAMPLE_TIME_MS_DEF; /* default Controller Sample Time is 0.1 seconds */
	
	PID_SetControllerDirection(uPID, ControllerDirection);
	PID_SetTunings2(uPID, Kp, Ki, Kd);
	
	

}

void PID2(PID_TypeDef *uPID,  float *Output, float *Setpoint, float Kp, float Ki,float Kd, PIDCD_TypeDef ControllerDirection)
{
	PID(uPID,  Output, Setpoint, Kp, Ki, Kd, ControllerDirection);
}

/* ~~~~~~~~~~~~~~~~~ Computing ~~~~~~~~~~~~~~~~~ */
uint8_t PID_Compute(PID_TypeDef *uPID, float input)
{
	
	float error;
	float dInput;
	float output;
		/* ..... Compute all the working error variables ..... */
	//	input   = *uPID->MyInput;

	error   = (*uPID->MySetpoint - input)/10.0;
	dInput  = (input - uPID->LastInput)/10.0;

    //Интергальная составляющая
	 uPID->OutputSum     = uPID->OutputSum  + error  * 0.1 * uPID->Ki ;
		

	 uPID->OutputSum -= uPID->Kp * dInput;
    //Проверяем что не вывалились за преелы.

    if (uPID->OutputSum > uPID->OutMax)
    {
		uPID->OutputSum = uPID->OutMax;
	}
	else if (uPID->OutputSum < uPID->OutMin)
	{
		uPID->OutputSum = uPID->OutMin;
	}
	else { }
		
	//Пропорциональная составляющая
	output = uPID->Kp *  error + uPID->OutputSum  ;

	if (output > uPID->OutMax)
	{
		output = uPID->OutMax;
	}
	else if (output < uPID->OutMin)
	{
		output = uPID->OutMin;
	}
	else { }
		
	*uPID->MyOutput = output;
	uPID->LastInput = input;
	return _TRUE;
}




/* ~~~~~~~~~~~~~~~~ PID Limits ~~~~~~~~~~~~~~~~~ */
void PID_SetOutputLimits(PID_TypeDef *uPID, float Min, float Max)
{
	/* ~~~~~~~~~~ Check value ~~~~~~~~~~ */
	if (Min >= Max)
	{
		return;
	}
	
	uPID->OutMin = Min;
	uPID->OutMax = Max;
	
	/* ~~~~~~~~~~ Check PID Mode ~~~~~~~~~~ */

		
		/* ..... Check out value ..... */
		if (*uPID->MyOutput > uPID->OutMax)
		{
			*uPID->MyOutput = uPID->OutMax;
		}
		else if (*uPID->MyOutput < uPID->OutMin)
		{
			*uPID->MyOutput = uPID->OutMin;
		}
		else { }
		
		/* ..... Check out value ..... */
		if (uPID->OutputSum > uPID->OutMax)
		{
			uPID->OutputSum = uPID->OutMax;
		}
		else if (uPID->OutputSum < uPID->OutMin)
		{
			uPID->OutputSum = uPID->OutMin;
		}
		else { }
		
	

}

/* ~~~~~~~~~~~~~~~~ PID Tunings ~~~~~~~~~~~~~~~~ */
void PID_SetTunings(PID_TypeDef *uPID, float Kp, float Ki, float Kd)
{
	PID_SetTunings2(uPID, Kp, Ki, Kd);
}
void PID_SetTunings2(PID_TypeDef *uPID, float Kp, float Ki, float Kd)
{
	
	float SampleTimeInSec;
	
	/* ~~~~~~~~~~ Check value ~~~~~~~~~~ */
	if (Kp < 0 || Ki < 0 || Kd < 0)
	{
		return;
	}
	
	/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


	uPID->DispKp = Kp;
	uPID->DispKi = Ki;
	uPID->DispKd = Kd;
	
	/* ~~~~~~~~~ Calculate time ~~~~~~~~ */
	SampleTimeInSec = ((float)uPID->SampleTime) / 1000;
	
	uPID->Kp = Kp;
	uPID->Ki = Ki;// * SampleTimeInSec;
	uPID->Kd = Kd / SampleTimeInSec;
	
	/* ~~~~~~~~ Check direction ~~~~~~~~ */
	if (uPID->ControllerDirection == _PID_CD_REVERSE)
	{
		
		uPID->Kp = (0 - uPID->Kp);
		uPID->Ki = (0 - uPID->Ki);
		uPID->Kd = (0 - uPID->Kd);
		
	}
	
}

/* ~~~~~~~~~~~~~~~ PID Direction ~~~~~~~~~~~~~~~ */
void          PID_SetControllerDirection(PID_TypeDef *uPID, PIDCD_TypeDef Direction)
{
	/* ~~~~~~~~~~ Check parameters ~~~~~~~~~~ */
	if ((Direction !=uPID->ControllerDirection))
	{
		
		uPID->Kp = (0 - uPID->Kp);
		uPID->Ki = (0 - uPID->Ki);
		uPID->Kd = (0 - uPID->Kd);
		
	}

	uPID->ControllerDirection = Direction;
	
}
PIDCD_TypeDef PID_GetDirection(PID_TypeDef *uPID)
{
	return uPID->ControllerDirection;
}

/* ~~~~~~~~~~~~~~~ PID Sampling ~~~~~~~~~~~~~~~~ */
void PID_SetSampleTime(PID_TypeDef *uPID, int32_t NewSampleTime)
{
	
	double ratio;
	
	/* ~~~~~~~~~~ Check value ~~~~~~~~~~ */
	if (NewSampleTime > 0)
	{
		
		ratio = (float)NewSampleTime / (float)uPID->SampleTime;
		
		uPID->Ki *= ratio;
		uPID->Kd /= ratio;
		uPID->SampleTime = (uint32_t)NewSampleTime;
		
	}
	
}

/* ~~~~~~~~~~~~~ Get Tunings Param ~~~~~~~~~~~~~ */
float PID_GetKp(PID_TypeDef *uPID)
{
	return uPID->DispKp;
}
float PID_GetKi(PID_TypeDef *uPID)
{
	return uPID->DispKi;
}
float PID_GetKd(PID_TypeDef *uPID)
{
	return uPID->DispKd;
}
