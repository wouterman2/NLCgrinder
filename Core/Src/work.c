//-----------------------------------------------------------------------------
//! \file       work.c
//! \author     R. Weimar. RMB Holland bv
//! \brief      Contains routines for the general sequence
//! \details
//! \Attention
//! \Created on: May 5, 2022
//-----------------------------------------------------------------------------

#include "work.h"
#include "serial.h"
#include "led.h"
#include "recipe.h"
#include "hopper.h"
#include "adjust.h"
#include "scale.h"
#include "grinder.h"
#include "consumable.h"
#include "tim.h"
#include "filter.h"
#include "eeprom.h"
//! \Global tick time flag
uint8_t Flg1ms;
//! \Global Work module structure
stcWRK WRK;
//! \Global flag to indicate all modules are initialized
uint8_t Initialized;
//! \Global action that is received from the display unit
uint8_t gAction;
uint8_t gActionAccepted;
//! \Global dummy mode settings
uint8_t DUMMYMODEGRINDER = 0;  //Active when 1, Not active, when 0
uint8_t DUMMYMODEFILTER = 0;  //Active when 1, Not active, when 0
uint8_t DUMMYMODERFID = 0;  //If RFID is missing set to 1
uint8_t DUMMYMODESCALE = 0;  //If scale is missing set to 1
uint8_t OpenAngle = 0;
/****************************************************************
  Function:     	Set selected dummy mode
  Description:  	Used to set the dummymode and store variables in EEprom
  Parameters:   	uint8_t newAddress, uint8_t newValue
  Return value: 	None
***************************************************************/
void WRK_SetDummyMode (uint8_t newAddress, uint8_t newValue)
{
	switch (newAddress)
	{
		case WRK_DUMMYMODEGRINDER:
		{
			DUMMYMODEGRINDER = newValue;
			break;
		}
		case WRK_DUMMYMODEFILTER:
		{
			DUMMYMODEFILTER = newValue;
			break;
		}
		case WRK_DUMMYMODERFID:
		{
			DUMMYMODERFID = newValue;
			break;
		}
		case WRK_DUMMYMODESCALE:
		{
			DUMMYMODESCALE = newValue;
			break;
		}
		default: break;
	}
	EEP_WriteEEPROM (newAddress, newValue);
}

//-----------------------------------------------------------------------------
//! \brief      Initalizes the work module
//! \details    Initalizes the work module
//! \params		None
void WRK_Init (void)
{
	DUMMYMODEGRINDER = EEP_ReadEEPROM (WRK_DUMMYMODEGRINDER);
	DUMMYMODEFILTER = EEP_ReadEEPROM (WRK_DUMMYMODEFILTER);
	DUMMYMODERFID = EEP_ReadEEPROM (WRK_DUMMYMODERFID);
	DUMMYMODESCALE = EEP_ReadEEPROM (WRK_DUMMYMODESCALE);
	if (DUMMYMODEGRINDER)
	{
		WRK_SetMainStatus(sIDLE);
		return;
	}
	if (WRK_GetHardwareVersion() < HARDWAREVERSIONMINIMUM)
	{
		LED_Set(STANDBYLED, BLINKING, 255, 0, 0, 255, 100, 100, 0, 0, 0);
		LED_Set(OUTLETLED, BLINKING, 255, 0, 0, 255, 100, 100, 0, 0, 0);
		LED_Set(WATERTANKLEDS, BLINKING, 255, 0, 0, 255, 100, 100, 0, 0, 0);
		WRK_SetMainStatus(sHARDWAREINCOMPATIBLE);
	}
	WRK.PrepareStatus[0] = pINACTIVE;
}
//-----------------------------------------------------------------------------
//! \brief      Returns the prepare status
//! \details    Returns the prepare status
//! \param[out]	enuPrepare WRK.PrepareStatus
enuPrepare WRK_GetPrepareStatus (void)
{
	return WRK.PrepareStatus[0];
}
//-----------------------------------------------------------------------------
//! \brief      Returns the main status
//! \details    Returns the main status
//! \param[out]	enuStatus WRK.MainStatus
enuStatus WRK_GetMainStatus (void)
{
	return WRK.MainStatus;
}
//-----------------------------------------------------------------------------
//! \brief      Returns the sub status
//! \details    Returns the sub status
//! \param[out]	enuStatus WRK.MainStatus
enuStatus WRK_GetSubStatus (void)
{
	return WRK.SubStatus;
}
//-----------------------------------------------------------------------------
//! \brief      Handles the sequence of the coffee machine
//! \details    Handles the different modules
//! \param      None
void WRK_HandleSequence (void)
{
	static uint8_t TickTimer=30;
	if (TickTimer ++ < 99) return; //100 ms interval
	TickTimer = 0;

	switch (WRK.MainStatus)
	{
		case sUNDEFINED:
		{
			WRK_HandleUndefined();
			break;
		}
		case sSTANDBY:
		{
			WRK_HandleStandby();
			break;
		}
		case sIDLE: //Machine is switched from standby but is not performing any tasks
		{
			WRK_HandleIdle ();
			break;
		}
		case sPREPARE: //Machine is switched from standby but is not performing any tasks
		{
			WRK_HandlePrepare ();
			break;
		}
		default:
			break;
	}
}

//-----------------------------------------------------------------------------
//! \brief      Handles the UNDEFINED status of sequence of the grinder
//! \details    Handles the UNDEFINED status of sequence of the grinder
//! \param      None
void WRK_HandleUndefined (void)
{
	switch (WRK.SubStatus)
	{
		case sUNDEFINED:
		{
			LED_Set(STANDBYLED, BLINKING, 0, 0, 255, 50, 500, 500, 5, 0, 0); //Blinking Blue
			if (!DUMMYMODESCALE)
			{
				SCL_Tare();
			}
			ADJ.Timer = 0;
			WRK.SubStatus = sWAIT;
			break;
		}
		case sWAIT:
		{
			//Initialize grinder
			if (!DUMMYMODEGRINDER)
			{
				uint8_t ReturnValue = ADJ_HandleTask (sHOME);
				if ((ReturnValue == TASK_READY)||(ReturnValue == TASK_ERROR))
				{
					//All LED off
					LED_Set(STANDBYLED, BREATHING, 255, 0, 0, 50, 0, 0, 5, 0, 0); //Breathing Red
					//Set status to STANDBY
					WRK_SetMainStatus(sSTANDBY);
				}
				/*else if (ReturnValue == TASK_ERROR)
				{
					//Handle error
				}
				*/
			}
			else
			{
				//All LED off
				LED_Set(STANDBYLED, BREATHING, 255, 0, 0, 50, 0, 0, 5, 0, 0); //Breathing Red
				//Set status to STANDBY
				WRK_SetMainStatus(sSTANDBY);
			}
			break;
		}
		default:
			break;
	}

}

//-----------------------------------------------------------------------------
//! \brief      Handles the STANDBY status of sequence of the coffee machine
//! \details    Handles the STANDBY status of sequence of the coffee machine
//! \param      None
void WRK_HandleStandby (void)
{
	switch (WRK.SubStatus)
	{
		case sUNDEFINED:
		{
			//Temporary: LEDs off
			if (LED[STANDBYLED].Effect == SWITCHEDON)
				LED_Set(STANDBYLED, FADEOUT, 255, 255, 255, LED[STANDBYLED].Intensity, 0, 0, 5, 0, 0); //Fade out
			else if (LED[STANDBYLED].Effect != FADEOUT)
				LED_Set(STANDBYLED, BREATHING, 255, 0, 0, 50, 0, 0, 5, 0, 0); //Breathing Red
			//Set status to WAIT
			WRK_SetSubStatus (sWAIT);
			break;
		}
		case sWAIT:
		{
			if (LED[STANDBYLED].Effect == SWITCHEDOFF)
			{
				LED_Set(STANDBYLED, BREATHING, 255, 0, 0, 50, 0, 0, 5, 0, 0); //Breathing Red
			}
			if (WRK_GetAction(MTypeSwitchOn)) //Received Switch to IDLE command
			{
				WRK_SetMainStatus(sIDLE);
			}
			else if (WRK_GetAction(MTypeStandby)) //Received Switch to STANDBY command
			{
				WRK.MainStatus = sSTANDBY; //Not really needed, but put here to respond to UART quickly, since otherwise a time out would occur
			}
			break;
		}
		default:
			break;
	}
}

//-----------------------------------------------------------------------------
//! \brief      Handles the IDLE status of sequence of the grinder
//! \details    Handles the IDLE status of sequence of the grinder
//! \param      None
void WRK_HandleIdle (void)
{
	switch (WRK.SubStatus)
	{
		case sUNDEFINED:
		{
			if (DUMMYMODEGRINDER)
			{
				gRecipe.Valid=1;
			}
			//Temporary: Later will be controlled from the Linux system
			LED_Set(STANDBYLED, FADEIN, 255, 255, 255, 50, 0, 0, 5, 0, 0); //On White
			//Set status to WAIT
			WRK_SetSubStatus (sWAIT);

			break;
		}
		case sWAIT:
		{
			//if (ADJ_Motor.GetPosition <= 5005) ADJ_Motor.SetPosition = 20500;
			//if (ADJ_Motor.GetPosition >= 20495) ADJ_Motor.SetPosition = 5000;
			if (WRK_GetAction(MTypeStandby))//Received Switch to STANDBY command
			{
				WRK_SetMainStatus(sSTANDBY);
			}
			else if (WRK_GetAction(MTypeSwitchOn)) //Received Switch to IDLE command
			{
				WRK.MainStatus = sIDLE; //Not really needed, but put here to respond to UART quickly, since otherwise a time out would occur
			}
			else if (
						(WRK_GetAction(MTypeStartRecipe1)) ||  //Received Command 0x11: Start recipe 1
						(FIL_GetTrigger() == 1) //Received trigger
				    )
			{
				if (((CON.Status != cNOTPRESENT)||(DUMMYMODERFID))&&(gRecipe.Valid == 1))
				{
					//Start grinding requested. Check if recipe is valid and hopper is present
					if (DUMMYMODEGRINDER) ADJ.GetGrindSize = 300;
					gRecipe.CurrentBlock = 0;
					gRecipe.Active = 1;
					WRK_SetMainStatus(sPREPARE);
				}
				else
					SCL_Measure(1);
			}
			else
				SCL_Measure(1);
			break;
		}
		default:
			break;
	}
}
//-----------------------------------------------------------------------------
//! \brief      Handles the reset of all modules after recipe is finished
//! \details    Makes sure that no module is still active unexpectedly
//! \param[in]  uint8_t newRecipe
void WRK_ResetAllModules(uint8_t newRecipe)
{
	gRecipe.Active = 0;
}
//-----------------------------------------------------------------------------
//! \brief      Handles the PREPARE status of sequence of the coffee machine
//! \details    Handles the PREPARE status of sequence of the coffee machine
//! \param      None
void WRK_HandlePrepare (void)
{
	switch (WRK.SubStatus)
	{
		case sUNDEFINED:
		{
			//Check if all recipes are finished.
			if (gRecipe.Active == 0)
			{
				WRK_ResetAllModules(RECIPE1);
				WRK_SetMainStatus(sIDLE);
			}
			else //Not all are finished. Good to go.
			{
				WRK_SetSubStatus (sACTIVE);
			}
			break;
		}
		case sACTIVE:
		{
			if (gRecipe.Active == 1) //Recipe x started
			{
				WRK_HandlePrepareRecipe();
			}
			//Check if all recipes are finished
			if (gRecipe.Active == 0)
			{
				WRK_ResetAllModules(RECIPE1);
				WRK_SetMainStatus(sIDLE);
			}
			//Check if other recipes are started
			if (WRK_GetAction(MTypeStartRecipe1)) //Received Command 0x11: Start recipe 1
			{
				gRecipe.CurrentBlock = 0;
				gRecipe.Active = 1;
			}
			break;
		}
		default:
			break;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim5)
	{
		if (HOP.Status == hDUMPPOSITION)
		{
			HAL_TIM_Base_Stop_IT (&htim5);
			TIM5->CNT = 0;
			HOP_SetAngle(180); //return to fill position
			HOP.Counter = 0;
			HOP.Status = hCHECKWEIGHT;
		}
		else if (HOP.Status == hDOSEPOSITION)
		{
			HAL_TIM_Base_Stop_IT (&htim5);
			TIM5->CNT = 0;
			HOP_SetAngle(180); //return to fill position
			HOP.Counter = 0;
			HOP.Status = hCHECKWEIGHT;
		}
	}
}

//-----------------------------------------------------------------------------
//! \brief      Handles the actual grinding
//! \details    Handles the action needed to grind the coffee
//! \params		None
void WRK_HandlePrepareRecipe (void)
{
	static uint32_t PrepareCounter = 0;
	static uint8_t ADJ_RemovePlayFlag;
	static uint8_t NoAdjustNeeded;

	if (PrepareCounter < 65535) PrepareCounter += 1; //1 tick = 100 ms

	if (WRK_GetAction(MTypeStopRecipe1)) //Received Command 0x14: Stop preparation
	{
		//Switch off everything in a safe way
		WRK_ResetAllModules(0);
		gRecipe.Active = 0;
		WRK.PrepareStatus[0] = pINACTIVE;
	}
	else
	{
		switch (WRK.PrepareStatus[0])
		{
			case pUNDEFINED:
			case pINACTIVE:
			{
				if (abs(gRecipe.RecipeBlocks[0].GrindSize - ADJ.GetGrindSize) <= ADJ.GrindSizeAccuracy) //No need to adjust
				{
					NoAdjustNeeded = 1;
				}
				else
				{
					NoAdjustNeeded = 0;
					if (ADJ.GetGrindSize >= gRecipe.RecipeBlocks[0].GrindSize + ADJ.Play) //No play compensation needed
					{
						ADJ.SetGrindSize = gRecipe.RecipeBlocks[0].GrindSize;
						ADJ_Motor.SetPosition = (ADJ.SetGrindSize - ADJ.HomeSize)* ADJ_PULSESPER10UM / 10;
						ADJ_RemovePlayFlag = 1;
					}
					else //Play compensation needed
					{
						ADJ.SetGrindSize = gRecipe.RecipeBlocks[0].GrindSize + ADJ.Play;
						ADJ_Motor.SetPosition = (ADJ.SetGrindSize - ADJ.HomeSize)* ADJ_PULSESPER10UM / 10;
						ADJ_RemovePlayFlag = 0;
					}

				}

				HOP.Status = hFILLPOSITION;
				HOP_SetAngle(90); //Fill the hopper free-wheel compartment
				HOP.TimerValue = (uint32_t) (gRecipe.RecipeBlocks[0].ShutterTime);//GrindWeight * 5 / 2); //Initial shutter time
				if (HOP.TimerValue < HOP.TimeMin) HOP.TimerValue = HOP.TimeMin;
				if (HOP.TimerValue > HOP.TimeMax) HOP.TimerValue = HOP.TimeMax;
				HOP.Counter = 0;
				WRK.PrepareStatus[0] = pADJUST;
				SCL.offset = SCL.rawvalue;
				SCL.GetWeight = 0;
				SCL.GetWeightOld = 0;
				//SCL.InitialWeight = SCL.GetWeight;
				HOP.DoseCounter = 0;
				//TEMP

				break;
			}
			case pADJUST:
			{
				if (NoAdjustNeeded) //No need to adjust
				{
					PrepareCounter = 0;
					TIM5->ARR = HOP.TimerValue*1000; // Opening time shutter in us
					HAL_TIM_Base_Start_IT (&htim5);
					HOP_SetAngle(HOP.DumpAngle);
					HOP.Status = hDUMPPOSITION;
					WRK.PrepareStatus[0] = pDOSE;
				}
				else
				{
					if (DUMMYMODEGRINDER)
					{
						ADJ.Status = sACTIVE;
						ADJ_RemovePlayFlag = 1;
					}
					if (ADJ_RemovePlayFlag == 0) //First go to coarser, to eliminate play
					{
						if (ADJ_Set(gRecipe.RecipeBlocks[0].GrindSize + ADJ.Play) == TASK_READY)
						{
							ADJ_RemovePlayFlag = 1;
							ADJ.SetGrindSize = gRecipe.RecipeBlocks[0].GrindSize;
							ADJ_Motor.SetPosition = (ADJ.SetGrindSize - ADJ.HomeSize)* ADJ_PULSESPER10UM / 10;
						}
					}
					else if (ADJ_RemovePlayFlag == 1)//Flag = 1. Now go back to finer and eliminate the play
					{
						if (ADJ_Set(gRecipe.RecipeBlocks[0].GrindSize) == TASK_READY)
						{
							ADJ_RemovePlayFlag = 0;
							PrepareCounter = 0;
							TIM5->ARR = HOP.TimerValue*1000; // Opening time shutter in us
							HAL_TIM_Base_Start_IT (&htim5);
							HOP_SetAngle(HOP.DumpAngle);
							HOP.Status = hDUMPPOSITION;
							WRK.PrepareStatus[0] = pDOSE;
						}
					}
				}
				break;
			}
			case pDOSE:
			{
				/*if (HOP.Status == hFILLPOSITION)
				{
					if (HOP.Counter < 10)  //Less than 200 ms
					{
						HOP.Counter ++;
					}
					else
					{
						TIM5->CNT=0;
						TIM5->ARR = HOP.TimerValue*1000; // us
						HAL_TIM_Base_Start_IT (&htim5);
						HOP_SetAngle(OpenAngle);
						HOP.Status = hDUMPPOSITION;
					}
				}
				else
				*/
				if (HOP.Status == hCHECKWEIGHT)
				{
					if (HOP.Counter == 3)
						HOP_SetAngle(90);
					if (HOP.Counter < SCL.StableTime/100 + 3)  //Wait x ms for beans stop moving
					{
						HOP.Counter ++;
					}
					else
					{
						if (DUMMYMODESCALE)
						{
							if (SCL.GetWeight == 0)
							{
								SCL.GetWeight = -(gRecipe.RecipeBlocks[0].GrindWeight * 2 / 3);
							}
							else
							{
								SCL.GetWeight += (-gRecipe.RecipeBlocks[0].GrindWeight / 6);
							}
							if (SCL.GetWeight <= -gRecipe.RecipeBlocks[0].GrindWeight)
							{
								HOP_SetAngle(180);
								HOP.Status = hCLOSED;
								WRK.PrepareStatus[0] = pGRIND;
							}
							else
							{
								HOP.Counter = 0;
							}
						}
						else
						{
							SCL_Measure(1);
							/*if (SCL.ShotWeight <= 0) //Fault situation. No beans anymore or malfunction of scale
							{
								HOP.DoseCounter++;
								//To add error handling
								HOP_SetAngle(180); //Close hopper
								WRK.PrepareStatus[0] = pGRIND; //Start grinding
							}
							else*/
							if (
									((SCL.GetWeight) <= -(int32_t) gRecipe.RecipeBlocks[0].GrindWeight + SCL.Accuracy)||
									(HOP.DoseCounter > HOP.DoseStepsMax)
								) //Normal situation, weight reached --> Grinding
							{
								HOP_SetAngle(180); //Close hopper
								WRK.PrepareStatus[0] = pGRIND; //Start grinding
							}
							else //Weight not reached yet. Calculate new opening time and dose again
							{
								HOP.DoseCounter++;
								if (SCL.ShotWeight > 0)
								{
									SCL.RestWeight =  (int32_t) gRecipe.RecipeBlocks[0].GrindWeight + (SCL.GetWeight);
									HOP.TimerValue = (uint32_t) (((SCL.RestWeight * ((int32_t)HOP.TimerValue - HOP.TimeMin) / (SCL.ShotWeight)) + HOP.TimeMin)*9/10);
								}
								if (HOP.TimerValue < HOP.TimeMin) HOP.TimerValue = HOP.TimeMin;
								if (HOP.TimerValue > HOP.TimeMax) HOP.TimerValue = HOP.TimeMax;
								TIM5->CNT=0;
								TIM5->ARR = HOP.TimerValue*1000; // us
								HAL_TIM_Base_Start_IT (&htim5);
								HOP_SetAngle(HOP.DoseAngle);
								HOP.Status = hDOSEPOSITION;
							}
						}
					}
				}
				break;
			}
			case pGRIND:
			{
				//if (DUMMYMODEGRINDER)
				//{
					if (GND_Set(gRecipe.RecipeBlocks[0].GrindWeight * 40) == TASK_READY)
					{
						gRecipe.Active = 0;
						WRK.PrepareStatus[0] = pINACTIVE;
					}
				//}
				break;
			}
			default:
				break;
		}
	}
}

//-----------------------------------------------------------------------------
//! \brief      Routine to set an action to be executed by the work module
//! \details    Sets the action and resets the counter. Checks if a command can be executed
//! \params 	None
void WRK_SetAction (uint8_t newAction)
{
	//check commands validity before try executing
	if ((newAction == MTypeStartRecipe1)&& (gRecipe.Valid == 0)) //Start recipe, but no recipe loaded
	{
		SER_SetStatus (MStatNoValidRecipe); //No valid recipe loaded
	}
	else if ((newAction == MTypeStopRecipe1) && (gRecipe.Active == 0)) //Stop recipe, but recipe is not active
	{
		SER_SetStatus (MStatRecipeNotActive); //No valid recipe loaded
	}
	else //Commands can be executed. Try executing
	{
		gAction = newAction;
		gActionAccepted = 0;
		WRK.CounterAction = 0;
	}

}

//-----------------------------------------------------------------------------
//! \brief      Routine to reset a pending action
//! \details    Resets the action and counter
//! \params 	None
void WRK_ResetAction (void)
{
	gAction = 0;
	gActionAccepted = 0;
	WRK.CounterAction = 0;
}

//-----------------------------------------------------------------------------
//! \brief      Routine to initiate return message or timeout
//! \details    Checks if the command timed out or is accepted
//! \details    Initiates the sending of the respective return message
//! \params 	None
void WRK_HandleAction (void)
{
	static uint8_t TickTime = 10;
	if (TickTime ++ < 99) return;
	TickTime = 0;

	if ((gAction > 0) || (gActionAccepted))
	{
		WRK.CounterAction += 100;
		if (WRK.CounterAction > ACTIONTIMEOUT)
		{
			if (gActionAccepted)
			{
				SER_SetStatus (0x01); //Action executed
			}
			else
			{
				SER_SetStatus (0x13); //Action timed out
			}
			gActionAccepted = 0;
			gAction = 0;
		}
		else if (gActionAccepted)
		{
			SER_SetStatus (0x01); //Action executed
			gActionAccepted = 0;
			gAction = 0;
		}
	}
}
//-----------------------------------------------------------------------------
//! \brief      Routine to get an action to be executed
//! \details    Compares the required action with the active action
//! \details    Returns 1 and sets gActionAccepted if identical, Returns 0 if not
//! \param[in]  uint8_t ReqAction
uint8_t WRK_GetAction (uint8_t ReqAction)
{
	uint8_t returnValue = 0;
	if (gAction > 0)
	{
		if (ReqAction == gAction)
		{
			gAction = 0;
			gActionAccepted = 1;
			returnValue = 1;
		}
	}
	return returnValue;
}

//-----------------------------------------------------------------------------
//! \brief      Handles the tick time counter
//! \details    Handles 1 ms time interrupt
//! \param      None
void WRK_HandleTickTime (void)
{
	if (!Initialized) return;

	SER_Handle();
	WRK_HandleSequence();
	WRK_HandleAction();
	ADJ_HandleMotor();
	HOP_Handle();
	LED_Handle();
	if ((!DUMMYMODERFID) && (WRK.MainStatus != sPREPARE)) CON_Handle(); //Only RFID handling if the machine is not preparing grinds
}

//-----------------------------------------------------------------------------
//! \brief      Returns the software version
//! \details    Makes 1 integer of the software version that can be sent by UART
//! \params		None
uint32_t WRK_GetSoftwareVersion (void)
{
	return (SW_VER_MAJOR << 12) + (SW_VER_MINOR << 8) + SW_VER_BUGFIX;
}
//-----------------------------------------------------------------------------
//! \brief      Returns the hardware version
//! \details    Makes 1 integer of the hardware version that can be sent by UART
//! \params		None
uint32_t WRK_GetHardwareVersion (void)
{
	return (HW0 + HW1*2 + HW2*4 + HW3*8);
}

//-----------------------------------------------------------------------------
//! \brief      Handles the main status setting
//! \details    Set the status and saves to old status. Sets the sub status to UNDEFINED
//! \param[in]  enuStatus newStatus
void WRK_SetMainStatus (enuStatus newStatus)
{
	WRK.MainStatusOld = WRK.MainStatus;
	WRK.MainStatus = newStatus;
	WRK.SubStatusOld = WRK.SubStatus;
	WRK.SubStatus = sUNDEFINED;
}

//-----------------------------------------------------------------------------
//! \brief      Handles the sub status setting
//! \details    Set the status and saves to old status.
//! \param      Non
void WRK_SetSubStatus (enuStatus newStatus)
{
 	WRK.SubStatusOld = WRK.SubStatus;
	WRK.SubStatus = newStatus;
}
