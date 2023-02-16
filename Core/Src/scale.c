//-----------------------------------------------------------------------------
//! \file       scale.c
//! \author     R. Weimar. RMB Holland bv
//! \brief      Contains routines for the scale
//! \details
//! \Attention
//! \Created on: May 5, 2022
//-----------------------------------------------------------------------------

#include "scale.h"
#include "main.h"
#include "eeprom.h"

stcSCL SCL;
/****************************************************************
  Function:     	Set parameter for scale
  Description:  	Used to set the scale parameters and store variables in EEprom
  Parameters:   	uint8_t newAddress, uint16_t newValue
  Return value: 	None
***************************************************************/
void SCL_SetParameter (uint8_t newAddress, uint16_t newValue)
{
	switch (newAddress)
	{
		case SCL_STABLETIME:
		{
			SCL.StableTime = newValue;
			break;
		}
		default: break;
	}
	EEP_WriteEEPROM (newAddress, newValue);
}
//-----------------------------------------------------------------------------
//! \brief      Returns the scale GetWeight value
//! \details    Returns the scale GetWeight value
//! \param[out]	uint16_t GetWeight x0.1g
int32_t SCL_GetWeight (void)
{
	uint32_t ReturnValue;
	if (SCL.GetWeight > 0)
		ReturnValue = 0;
	else
		ReturnValue = -SCL.GetWeight;
	return ReturnValue;
}
//-----------------------------------------------------------------------------
//! \brief      Returns the scale measured weight
//! \details    Returns the scale measured weight
//! \param[in]	uint8_t newCycles
void SCL_Measure (uint8_t newCycles)
{
	/*static int32_t Total[5];
	static uint8_t Counter;
	int32_t ReturnValue;
	Total[Counter] = SCL_ReadValue();
	SCL.rawvalue = 0;
	for (uint8_t i = 0; i<5; i++)
	{
		SCL.rawvalue += Total[i];
	}
	if (Counter < 5)
	{
		SCL.Stable = 0;
		Counter ++;
	}
	else if (Counter >= 5)
	{
		Counter = 0;
		SCL.rawvalue /= 5;
		ReturnValue = (SCL.rawvalue - SCL.offset)/SCL.factor;
		SCL.Stable = 1;
		SCL.GetWeight = ReturnValue;
	}
	*/
	if (DUMMYMODESCALE) return;
	SCL.rawvalue = 0;
	SCL.Stable = 0;
	for (uint8_t i = 0; i<newCycles; i++)
	{
		SCL.rawvalue += SCL_ReadValue();
	}
	SCL.rawvalue /= newCycles;
	SCL.GetWeightOld = SCL.GetWeight;
	SCL.GetWeight = (SCL.rawvalue - SCL.offset)/SCL.factor;
	SCL.ShotWeight = SCL.GetWeightOld - SCL.GetWeight;
	SCL.Stable = 1;
}

//-----------------------------------------------------------------------------
//! \brief      Retrieves a value from HX711
//! \details    If HX711 is ready, sends a clock signal and reads the value
//! \param      None
int32_t SCL_ReadValue(void)
{
    int32_t buffer = 0;

    if 	(HAL_GPIO_ReadPin(LCDATA_GPIO_Port, LCDATA_Pin)==1)
    {
    	return 0xffffffff;
    }
    for (uint8_t i = 0; i < 24; i++)
    {
    	HAL_GPIO_WritePin(LCCLK_GPIO_Port, LCCLK_Pin, GPIO_PIN_SET);
        buffer = buffer << 1 ;
        if (HAL_GPIO_ReadPin(LCDATA_GPIO_Port, LCDATA_Pin))
        {
            buffer ++;
        }
        HAL_GPIO_WritePin(LCCLK_GPIO_Port, LCCLK_Pin, GPIO_PIN_RESET);
    }
    //for (int i = 0; i < SCL.gain; i++)
    //{
    	HAL_GPIO_WritePin(LCCLK_GPIO_Port, LCCLK_Pin, GPIO_PIN_SET);
    	buffer = buffer ^ 0x800000; //To convert to int32
    	HAL_GPIO_WritePin(LCCLK_GPIO_Port, LCCLK_Pin, GPIO_PIN_RESET);
    //}

    return buffer;
}
//-----------------------------------------------------------------------------
//! \brief      Determines the tare value of the scale
//! \details    Stores the tare value in the structure
//! \param[out]	int32_t SCL.Offset
int32_t SCL_Tare(void)
{
	static int64_t AverageOffset = 0;
	static uint8_t Counter = 0;

	SCL.offset = 0;
	while (Counter < SCL_TARE_SAMPLES)
	{
		if (HAL_GPIO_ReadPin(LCDATA_GPIO_Port, LCDATA_Pin)==GPIO_PIN_RESET)
		{
			SCL.rawvalue = SCL_ReadValue();

			if (SCL.rawvalue != 0xffffffff)
			{
				Counter++;
				AverageOffset += SCL.rawvalue;
			}
		}
	}
	SCL.offset  = (int32_t) AverageOffset / SCL_TARE_SAMPLES;
	Counter = 0;
	AverageOffset = 0;

	return SCL.offset;
}
//-----------------------------------------------------------------------------
//! \brief      Returns the scale status
//! \details    Returns the scale status
//! \param[out]	enuStatus SCL.Status
enuStatus SCL_GetStatus (void)
{
	return SCL.Status;
}
//-----------------------------------------------------------------------------
//! \brief      Initializes the scale module
//! \details    Initializes the scale module
//! \params		None
void SCL_Init(void)
{
	if (DUMMYMODESCALE)
	{
		SCL.GetWeight = 0;
		SCL.Status=sINACTIVE;
	}
	SCL.Status = sACTIVE;
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = LCCLK_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCCLK_GPIO_Port, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LCDATA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCDATA_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(LCCLK_GPIO_Port, LCCLK_Pin, GPIO_PIN_SET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(LCCLK_GPIO_Port, LCCLK_Pin, GPIO_PIN_RESET);

	SCL.factor = 112.0;//444.147328 for 500 g loadcell;
	SCL.Accuracy = 2; //0.2 gram accurate
	SCL.gain = 1; //1 = A 128, 2 = B 32, 3 = A 64
	//Check if this is first use. If so write default values in EEPROM
	if (EEP_ReadEEPROM (SCL_EEPROMFILLED == 0))
	{
		EEP_WriteEEPROM(SCL_STABLETIME, 500);
		EEP_WriteEEPROM(SCL_EEPROMFILLED, 1);
	}
	SCL.StableTime = EEP_ReadEEPROM (SCL_STABLETIME);
}
