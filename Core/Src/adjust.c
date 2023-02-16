//-----------------------------------------------------------------------------
//! \file       adjust.c
//! \author     R. Weimar. RMB Holland bv
//! \brief      Contains routines for the adjust
//! \details
//! \Attention
//! \Created on: May 5, 2022
//-----------------------------------------------------------------------------

#include "adjust.h"
#include "defines.h"
#include "tim.h"
#include <stdlib.h>

stcADJ ADJ;
//! \Global Index motor status container
stcDCMotor ADJ_Motor;
//! \Global error number for index motor
uint16_t ADJ_ErrorNumber;
uint8_t FlgEnc;
float xP = 2.5;//2;
float yI = 0;//0.005;
float zD = 500;//200;
//-----------------------------------------------------------------------------
//! \brief      Gets a machine parameter related to the adjust module
//! \details    Gets a parameter
//! \param[in]	uint8_t newParameter   (ADJ_P, ADJ_I, ADJ_D)
//! \param[out]	uint16_t newValue
uint16_t ADJ_GetParameter (uint8_t newParameter)
{
	uint16_t ReturnValue = 0;
	switch (newParameter)
	{
		case ADJ_P:
		{
			ReturnValue = ADJ_Motor.P * 100;
			break;
		}
		case ADJ_I:
		{
			ReturnValue = ADJ_Motor.I * 1000;
			break;
		}
		case ADJ_D:
		{
			ReturnValue = ADJ_Motor.D;
			break;
		}
		default:
			break;
	}
	return ReturnValue;
}
//-----------------------------------------------------------------------------
//! \brief      Sets a machine parameter related to the adjust module
//! \details    Sets a parameter and stores it in EEprom
//! \param[in]	uint8_t newParameter   (ADJ_P, ADJ_I, ADJ_D)
//! \param[in]	uint16_t newValue
void ADJ_SetParameter (uint8_t newParameter, uint16_t newValue)
{
	switch (newParameter)
	{
		case ADJ_P:
		{
			ADJ_Motor.P = (float)(newValue / 100.0f);
			EEP_WriteEEPROM(newParameter, newValue);
			break;
		}
		case ADJ_I:
		{
			ADJ_Motor.I = (newValue / 1000.0f);
			EEP_WriteEEPROM(newParameter, newValue);
			break;
		}
		case ADJ_D:
		{
			ADJ_Motor.D = (float) newValue;
			EEP_WriteEEPROM(newParameter, newValue);
			break;
		}
		default:
			break;
	}
}
//-----------------------------------------------------------------------------
//! \brief      Calculates the theoretical grind size
//! \details    Converts the pulses to the grind size
//! \params		None
void ADJ_SetUm (void)
{
	ADJ_Motor.GetUm = (((ADJ_Motor.GetPosition - ADJ_HOMEOFFSET) * 10) / ADJ_PULSESPER10UM) + ADJ.CalibrationSize;
	ADJ.GetGrindSize = ADJ_Motor.GetUm;
}

//-----------------------------------------------------------------------------
//! \brief      Resets the pulse count
//! \details    Used for homing. Resets the GetPosition and pulses count
//! \params		None
void ADJ_ResetPosition (void)
{
	TIM4->CNT=0;
	ADJ_Motor.TimerOld = 0;
	ADJ_Motor.GetPosition = 0;
	ADJ_SetUm();
}
//-----------------------------------------------------------------------------
//! \brief      Handles the adjust motor task
//! \details    Autonomously handles the task and returns TASK_READY if done
//! \param[in]  enuStatus newStatus
//! \param[out] Task state TASK_UNDEFINED, TASK_READY, TASK_ERROR, TASK_BUSY
uint8_t ADJ_HandleTask (enuStatus newStatus)
{
	uint8_t ReturnValue = TASK_UNDEFINED;
	switch (newStatus)
	{
		case sHOME:
		{
			ADJ.Timer ++;
			if (ADJ.Timer > ADJ_HOMETIMEOUT)
			{
				ReturnValue = TASK_ERROR;
				ADJ.Timer = 0;
			}
			else
			{
				ReturnValue = TASK_BUSY;
				switch (ADJ.Step)
				{
					case 0:
					{
						if (ADJ_Home() == 1) //Home sensor is on, so move from home sensor
						{
							ADJ_Motor.HomeFlag = 0;
							ADJ_ResetPosition();
							ADJ_Motor.SetPosition = ADJ_MAXPULSES; //Set position away from the home sensor
							ADJ.Step = 1;
						}
						else
						{
							ADJ_Motor.HomeFlag = 1; //Reset position to 0 if home sensor is triggered
							ADJ_ResetPosition();
							ADJ_Motor.SetPosition = -ADJ_MAXPULSES;
							ADJ.Step = 3;
						}
						break;
					}
					case 1:
					{
						//if (ADJ_Motor.HomeFlag == 1) //Home sensor is on
						if (ADJ_Home() == 0) //Home sensor is off, move a little further to remove play
						{
							ADJ_ResetPosition();
							ADJ_Motor.SetPosition = ADJ.Play * ADJ_PULSESPER10UM / 10;
							ADJ.Step = 2;
						}
						break;
					}
					case 2:
					{
						if (ADJ_Motor.GetPosition == ADJ_Motor.SetPosition)
						{
							ADJ_Motor.HomeFlag = 1; //Reset position to 0 if home sensor is triggered
							ADJ_ResetPosition();
							ADJ_Motor.SetPosition = -ADJ_MAXPULSES;
							ADJ.Step = 3;
						}
						break;
					}
					case 3:
					{
						//if (ADJ_Motor.HomeFlag == 1) //Home sensor is on
						if (ADJ_Home() == 1) //Home sensor is on, so move from home sensor
						{
							//ADJ_Motor.HomeFlag = 0;
							//ADJ_ResetPosition();
							ADJ_Motor.SetPosition = ADJ_HOMEOFFSET;
							ADJ.Step = 4;
						}
						break;
					}
					case 4:
					{
						if (ADJ_Motor.SetPosition == ADJ_Motor.GetPosition)
						{
							ADJ.Step = 0;
							ADJ.Timer = 0;
							ADJ.SetGrindSize = ADJ.GetGrindSize;
							ReturnValue = TASK_READY;
						}
						break;
					}
					default:
						break;
				}
			}
			break;
		}
		default:
			break;
	}
	return ReturnValue;
}
//-----------------------------------------------------------------------------
//! \brief      Controls the index motor position PID
//! \details    Calculates the PID value for the position control
//! \param      None
void ADJ_Init(void)
{
	if (DUMMYMODEGRINDER)
	{
		ADJ.GetGrindSize = 300;
		ADJ_Motor.GetUm = 300;
		ADJ.Status = sINACTIVE;
	}
	ADJ.GrindSizeAccuracy = 2;
	ADJ.Play = 100;
	ADJ.HomeSize = 500;
	ADJ.CalibrationSize = 375;
	HAL_TIM_Base_Start(&htim10);
	HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);
	HAL_TIM_Base_Start(&htim11);
	HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);
	ADJ_Motor.P = (float) (EEP_ReadEEPROM(ADJ_P)/100.0f);
	if (ADJ_Motor.P == 0) ADJ_Motor.P = 2.5;
	ADJ_Motor.I = (float) (EEP_ReadEEPROM(ADJ_I)/1000.0f);
	if (ADJ_Motor.I == 0) ADJ_Motor.I = 0;
	ADJ_Motor.D = EEP_ReadEEPROM(ADJ_D);
	if (ADJ_Motor.D == 0) ADJ_Motor.D = 250;
	ADJ_Motor.SpeedP = 2;
	ADJ_Motor.SpeedI = 0;
	ADJ_Motor.SpeedD = 200;
	ADJ_Motor.Status=sACTIVE;
	ADJ_Motor.PulsesPerRevolution = 28; //(PPR = 7 * 4)
	ADJ_CCW()=100;
	ADJ_CW()=100;
	ADJ_Motor.PosControl = 1;
	HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_ALL);
	TIM4->CNT = 0;
}

//-----------------------------------------------------------------------------
//! \brief      Controls the index motor position PID
//! \details    Calculates the PID value for the position control
//! \param      None
void ADJ_HandlePosPID (void)
{
	ADJ_Motor.ErrorP = (float) (ADJ_Motor.SetPosition - ADJ_Motor.GetPosition);
	ADJ_Motor.ErrorI = ADJ_Motor.ErrorI + ADJ_Motor.ErrorP;
	ADJ_Motor.ErrorD = ADJ_Motor.ErrorP - ADJ_Motor.ErrorPOld;
	ADJ_Motor.ErrorPOld = ADJ_Motor.ErrorP;
	ADJ_Motor.PID = ADJ_Motor.ErrorP * ADJ_Motor.P + ADJ_Motor.ErrorI * ADJ_Motor.I + ADJ_Motor.ErrorD * ADJ_Motor.D;
	/*ADJ_Motor.Control = ADJ_Motor.Control + (int32_t) ADJ_Motor.PID;

	if (((int32_t) ADJ_Motor.PID < 5) && (abs(ADJ_Motor.GetPosition - ADJ_Motor.SetPosition) < 3))
	{
	  ADJ_Motor.Control = 0;
	  ADJ_Motor.ErrorI = 0;

	}
	if (ADJ_Motor.Control < - 5000)
	{
		ADJ_Motor.Control = - 5000 ;
	}
	if (ADJ_Motor.Control > 5000)
	{
		ADJ_Motor.Control = 5000;
	}
	*/
}

//-----------------------------------------------------------------------------
//! \brief      Controls the index motor position PID
//! \details    Calculates the PID value for the position control
//! \param      None
void ADJ_HandleSpeedPID (void)
{
	ADJ_Motor.SpeedErrorP = (float) (ADJ_Motor.SetSpeed - ADJ_Motor.GetSpeed);
	ADJ_Motor.SpeedErrorI = ADJ_Motor.SpeedErrorI + ADJ_Motor.SpeedErrorP;
	ADJ_Motor.SpeedErrorD = ADJ_Motor.SpeedErrorP - ADJ_Motor.SpeedErrorPOld;
	ADJ_Motor.SpeedErrorPOld = ADJ_Motor.SpeedErrorP;
	ADJ_Motor.SpeedPID = ADJ_Motor.SpeedErrorP * ADJ_Motor.SpeedP + ADJ_Motor.SpeedErrorI * ADJ_Motor.SpeedI + ADJ_Motor.SpeedErrorD * ADJ_Motor.SpeedD;
	/*ADJ_Motor.Control = ADJ_Motor.Control + (int32_t) ADJ_Motor.PID;
	if (ADJ_Motor.Control < - 5000)
	{
		ADJ_Motor.Control = - 5000 ;
	}
	if (ADJ_Motor.Control > 5000)
	{
		ADJ_Motor.Control = 5000;
	}
	*/
}
uint8_t ADJ_Set (uint16_t newGrindSize)
{
	uint8_t ReturnValue = TASK_BUSY;

	if (DUMMYMODEGRINDER)
	{
		if (ADJ_Motor.GetUm > newGrindSize)
		{
			ADJ_Motor.GetUm --;
		}
		else if (ADJ_Motor.GetUm < newGrindSize)
		{
			ADJ_Motor.GetUm ++;
		}
		else
		{
			ReturnValue = TASK_READY;
		}
	}
	else
	{
		if (abs(ADJ.GetGrindSize - ADJ.SetGrindSize)<= ADJ.GrindSizeAccuracy )//ADJ.GrindSizeAccuracy um accuracy
		{
			ADJ.Status = sINACTIVE;
			ReturnValue = TASK_READY;
		}
		else
		{
			ADJ.Status = sACTIVE;
		}
	}
	return ReturnValue;
}
//-----------------------------------------------------------------------------
//! \brief      Handles the adjust motor Position
//! \details    Controls the motor to the set position
//! \params     None
void ADJ_HandleMotor (void)
{
	uint16_t Timer;
	int32_t Ticks;

	if (DUMMYMODEGRINDER) return;

	Timer = TIM4->CNT;
	if (abs(Timer - ADJ_Motor.TimerOld) > 30000) //Transition through zero
	{
		if (Timer > ADJ_Motor.TimerOld)
		{
			Ticks = -(ADJ_Motor.TimerOld + (65536 - Timer));
		}
		else
		{
			Ticks = Timer + (65536 - ADJ_Motor.TimerOld);
		}
	}
	else
	{
		Ticks = Timer - ADJ_Motor.TimerOld;
	}
	ADJ_Motor.GetPosition += Ticks;
	ADJ_Motor.PositionDelta = ADJ_Motor.SetPosition - ADJ_Motor.GetPosition;
	ADJ_SetUm();

	ADJ_Motor.GetSpeed = Ticks * 365 ;

	ADJ_Motor.TimerOld = Timer;
	if (ADJ_Motor.PosControl)
	{
		ADJ_HandlePosPID();
	}
	if (ADJ_Motor.SpeedControl)
	{
		ADJ_HandleSpeedPID();
	}
	if (ADJ_Motor.PosControl)
	{
	  if (ADJ_Motor.SpeedControl) //Both controls
	  {

	  }
	  else
	  {
		  ADJ_Motor.Control = ADJ_Motor.Control + (int32_t) ADJ_Motor.PID;
	  }
	}
	else if (ADJ_Motor.SpeedControl)
	{
		ADJ_Motor.Control = ADJ_Motor.Control + (int32_t) ADJ_Motor.SpeedPID;
	}
	if (ADJ_Motor.Control < - ADJ_MAXCONTROL)
	{
		ADJ_Motor.Control = - ADJ_MAXCONTROL ;
	}
	if (ADJ_Motor.Control > ADJ_MAXCONTROL)
	{
		ADJ_Motor.Control = ADJ_MAXCONTROL;
	}
	//Handle motor stalling
	if ((ADJ_Motor.Status==sACTIVE) && (abs(ADJ_Motor.Control) > 5000) && (ADJ_Motor.GetSpeed == 0) && (ADJ_Motor.PositionDelta != 0) && (ADJ.Status == sHOME)) //Motor is controlled > 75%, but not turning or encoder not detecting any pulses
	{
		ADJ_Motor.TimeOut++;
		if (ADJ_Motor.TimeOut > ADJ_TIMEOUT)
		{
			ADJ_Motor.TimeOut = 0;
			ADJ_ErrorNumber = 13001;
			ADJ_Motor.Status = sINACTIVE;
			ADJ.Status = sUNITERROR;
		}
	}
	else if ((ADJ_Motor.Status==sACTIVE) && (abs(ADJ_Motor.Control) > 5000) && (ADJ_Motor.GetSpeed == 0) && (ADJ_Motor.PositionDelta != 0)) //Motor is controlled > 75%, but not turning or encoder not detecting any pulses
	{
		ADJ_Motor.TimeOut++;
		if (ADJ_Motor.TimeOut > ADJ_TIMEOUTNORMAL)
		{
			ADJ_Motor.TimeOut = 0;
			ADJ_ErrorNumber = 13001;
			ADJ_Motor.Status = sINACTIVE;
			ADJ.Status = sUNITERROR;
		}
	}
	else
	{
		ADJ_Motor.TimeOut = 0;
	}
    if (ADJ_Motor.Status==sINACTIVE)
	{
		ADJ_SetPWM(CW,0);
		ADJ_Motor.Status = sINACTIVE;
		ADJ_Motor.ErrorP = 0;
		ADJ_Motor.ErrorPOld = 0;
		ADJ_Motor.ErrorD = 0;
		ADJ_Motor.ErrorI = 0;
		ADJ_Motor.PID = 0;
		ADJ_Motor.SpeedErrorP = 0;
		ADJ_Motor.SpeedErrorPOld = 0;
		ADJ_Motor.SpeedErrorD = 0;
		ADJ_Motor.SpeedErrorI = 0;
		ADJ_Motor.SpeedPID = 0;
		ADJ_Motor.Control = 0;
		ADJ_Motor.SetSpeed = 0;
	}
	else if (ADJ_Motor.Control < 0)
	{
		ADJ_SetPWM(CW, - ADJ_Motor.Control/100);
	}
	else if (ADJ_Motor.Control > 0)
	{
		ADJ_SetPWM(CCW, ADJ_Motor.Control/100);
	}
}

//-----------------------------------------------------------------------------
//! \brief      Controls the adjust motor direction and speed
//! \details    Sets the PWM with the calculated speed
//! \param      None
void ADJ_SetPWM (uint8_t newDirection, uint8_t newSpeed)
{
  switch (newDirection)
  {
    case CCW:
    {
      ADJ_CCW() = 100 - newSpeed;
      ADJ_CW()=100;
      break;
    }
    case CW:
    {
      ADJ_CCW() = 100;
      ADJ_CW() = 100 - newSpeed;
      break;
    }
    default:
    {
      ADJ_CW() = 100;
      ADJ_CCW()=100;
      break;
    }
  }
}
