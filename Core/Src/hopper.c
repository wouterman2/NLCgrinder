//-----------------------------------------------------------------------------
//! \file       hopper.c
//! \author     R. Weimar. RMB Holland bv
//! \brief      Contains routines for the hopper
//! \details
//! \Attention
//! \Created on: May 5, 2022
//-----------------------------------------------------------------------------

#include "hopper.h"
#include "consumable.h"
#include "defines.h"
#include "tim.h"
#include "work.h"
#include "main.h"

stcHOP HOP;
/****************************************************************
  Function:     	Set data for dosing
  Description:  	Used to set the hopper data and store variables in EEprom
  Parameters:   	uint8_t newAddress, uint16_t newValue
  Return value: 	None
***************************************************************/
void HOP_SetParameter (uint8_t newAddress, uint16_t newValue)
{
	switch (newAddress)
	{
		case HOP_TIMEMIN:
		{
			HOP.TimeMin = newValue;
			break;
		}
		case HOP_TIMEMAX:
		{
			HOP.TimeMax = newValue;
			break;
		}
		case HOP_DUMPANGLE:
		{
			HOP.DumpAngle = newValue;
			break;
		}
		case HOP_DOSEANGLE:
		{
			HOP.DoseAngle = newValue;
			break;
		}
		case HOP_DOSESTEPSMAX:
		{
			HOP.DoseStepsMax = newValue;
			break;
		}
		case HOP_CLOSEDPULSETIME:
		{
			HOP.ClosedPulseTime = newValue;
			break;
		}
		case HOP_OPENEDPULSETIME:
		{
			HOP.OpenedPulseTime = newValue;
			break;
		}
		default: break;
	}
	EEP_WriteEEPROM (newAddress, newValue);
}
//-----------------------------------------------------------------------------
//! \brief      Handles the shutter timeout
//! \details    Sets the PWM to 0 after the HOP_MAX_OPERATION_TIME to avoid burning the motor
//! \params		None
void HOP_Handle (void)
{
	if (HOP.TimeOut < HOP_MAX_OPERATION_TIME)
		HOP.TimeOut ++;
	else
		HOP_ANGLE = 0; //Disable motor, so it will not burn
}
//-----------------------------------------------------------------------------
//! \brief      Sets the shutter angle
//! \details    Sets the PWM to set the hopper angle
//! \param[out]	enuHopper HOP.Status
void HOP_SetAngle (uint8_t newAngle)
{
	uint32_t Pulse;
	if (newAngle > 180) return;
	if (DUMMYMODESCALE)
	{
		HOP_ANGLE=0;
		return;
	}
	//Pulse = ((PULSE_180_DEGREES - PULSE_0_DEGREES) * newAngle / 180) + PULSE_0_DEGREES;
	Pulse = ((HOP.ClosedPulseTime - HOP.OpenedPulseTime) * newAngle / 180) + HOP.OpenedPulseTime;
	HOP_ANGLE = Pulse;
	HOP.TimeOut = 0;
}

//-----------------------------------------------------------------------------
//! \brief      Returns the hopper status
//! \details    Returns the hopper status
//! \param[out]	enuHopper HOP.Status
enuHopper HOP_GetStatus (void)
{
	enuHopper ReturnValue = hNOTPLACED;
	if (CON.Status == cPRESENT)
		ReturnValue = HOP.Status;

	return ReturnValue;
}
//-----------------------------------------------------------------------------
//! \brief      Initializes the hopper module
//! \details    Initializes the hopper module
//! \params		None
void HOP_Init(void)
{
	if (DUMMYMODEGRINDER)
	{
		HOP.GetAngle = 90;
	}
	//Check if this is first use. If so write default values in EEPROM
	if (EEP_ReadEEPROM (HOP_EEPROMFILLED) == 0)
	{
		EEP_WriteEEPROM(HOP_TIMEMIN, 175);
		EEP_WriteEEPROM(HOP_TIMEMAX, 500);
		EEP_WriteEEPROM(HOP_DUMPANGLE, 0);
		EEP_WriteEEPROM(HOP_DOSEANGLE, 45);
		EEP_WriteEEPROM(HOP_DOSESTEPSMAX, 5);
		EEP_WriteEEPROM(HOP_EEPROMFILLED, 1);
		EEP_WriteEEPROM(HOP_CLOSEDPULSETIME, 2475);
		EEP_WriteEEPROM(HOP_OPENEDPULSETIME, 600);
	}
	HOP.TimeMin = EEP_ReadEEPROM (HOP_TIMEMIN);
	HOP.TimeMax = EEP_ReadEEPROM (HOP_TIMEMAX);//500;
	HOP.DumpAngle = EEP_ReadEEPROM (HOP_DUMPANGLE);// 0;
	HOP.DoseAngle = EEP_ReadEEPROM (HOP_DOSEANGLE);//45;
	HOP.DoseStepsMax = EEP_ReadEEPROM (HOP_DOSESTEPSMAX);//5;
	HOP.ClosedPulseTime = EEP_ReadEEPROM (HOP_CLOSEDPULSETIME);//2475;
	if (HOP.ClosedPulseTime == 0)
	{
		HOP.ClosedPulseTime = 2475;
		EEP_WriteEEPROM(HOP_CLOSEDPULSETIME, 2475);
	}
	HOP.OpenedPulseTime = EEP_ReadEEPROM (HOP_OPENEDPULSETIME);//600;
	if (HOP.OpenedPulseTime == 0)
	{
		HOP.OpenedPulseTime = 600;
		EEP_WriteEEPROM(HOP_OPENEDPULSETIME, 600);
	}
	HOP.Status = hCLOSED;
	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
	HOP_SetAngle (180);
}




