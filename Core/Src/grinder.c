//-----------------------------------------------------------------------------
//! \file       grinder.c
//! \author     R. Weimar. RMB Holland bv
//! \brief      Contains routines for the grinder
//! \details
//! \Attention
//! \Created on: May 5, 2022
//-----------------------------------------------------------------------------

#include "grinder.h"
#include "main.h"

stcGND GND;

//-----------------------------------------------------------------------------
//! \brief      Returns the grinder status
//! \details    Returns the grinder status
//! \param[out]	enuStatus GND.Status
enuStatus GND_GetStatus (void)
{
	return GND.Status;
}
//-----------------------------------------------------------------------------
//! \brief      Initializes the grinder module
//! \details    Initializes the grinder module
//! \params		None
void GND_Init(void)
{
	GND.Status = sINACTIVE;
}

uint8_t GND_Set (uint16_t newGrindTime)
{
	static uint16_t GrindCounter = 0;
	uint8_t ReturnValue = TASK_BUSY;

	//if (DUMMYMODEGRINDER)
	//{
		HAL_GPIO_WritePin(M1_GPIO_Port, M1_Pin, GPIO_PIN_SET);
		GND.Status = sACTIVE;
		GrindCounter += 100;
		if (GrindCounter >= newGrindTime)
		{
			HAL_GPIO_WritePin(M1_GPIO_Port, M1_Pin, GPIO_PIN_RESET);
			GrindCounter = 0;
			ReturnValue = TASK_READY;
			GND.Status = sINACTIVE;
		}
	//}

	return ReturnValue;
}
