//-----------------------------------------------------------------------------
//! \file       analog.c
//! \author     R. Weimar. RMB Holland bv
//! \brief      Contains routines for the analog measurement
//! \details
//! \Attention
//! \Created on: Apr 27, 2022
//-----------------------------------------------------------------------------
#include "analog.h"
#include "adc.h"
#include "work.h"
#include <math.h>
//-----------------------------------------------------------------------------
//! \Global ADC calibration value
uint16_t vrefint_cal;                        // VREFINT calibration value
//! \Global ADC container
uint16_t ADC_Converted_Values[NROFANALOGSENSORS];
uint16_t ADC_Voltage_Values[NROFANALOGSENSORS-1];

//-----------------------------------------------------------------------------
//! \brief      ADC complete conversion interrupt
//! \details    Convert ADC values to voltages (mV)
//! \param      None
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{

	//WAT_SetBuffer();
}

//-----------------------------------------------------------------------------
//! \brief      Initializes the analog module
//! \details    Starts DMA and assign values
//! \param      None
void ANA_Init(void)
{
	vrefint_cal= *((uint16_t*)VREFINT_CAL_ADDR_CMSIS); // read VREFINT_CAL_ADDR memory location
	HAL_ADC_Start_DMA(&hadc,(uint32_t*) &ADC_Converted_Values,NROFANALOGSENSORS);
}
//-----------------------------------------------------------------------------
//! \brief      Returns the value of the selected sensor
//! \details    Converts the analog value to a temperature or pressure
//! \param[in]  enuSensor newSensor
//! 			ADC_BURRS = Burr temperature,
//!				ADC_MOTOR = Motor temperature
//!				ADC_VINTREF = Internal reference voltage
//! \param[out] float value
float ANA_GetSensorValue(enuSensor newSensor)
{
	float ADCValue = (float) (ADC_Converted_Values[(uint8_t) newSensor]);
	float ReturnValue = 0;
	float NTCRes, fTemp1, fTemp2;
	float fLog;
	float NTCB = 3740.0;
	const float SerialRes = 10000.0;
	const float R25 = 47000.0;
	uint64_t Temp;
	for (uint8_t i = 0; i< NROFANALOGSENSORS; i++)
	{
		Temp = (uint64_t)3000 * (uint64_t)vrefint_cal * (uint64_t)ADC_Converted_Values[i];
		Temp = Temp / (uint64_t) ADC_Converted_Values[NROFANALOGSENSORS - 1];
		Temp = Temp / 4095;
		ADC_Voltage_Values[i] = (uint16_t) Temp;
	}
	switch (newSensor)
	{
		case ADC_BURRS:
		case ADC_MOTOR:
		{
			if (DUMMYMODEGRINDER)
			{
				ReturnValue = 250;
			}
			else
			{
				if (ADCValue > NTC_DISCONNECTED_VALUE)
					ReturnValue = DISCONNECTED;
				else if (ADCValue < NTC_SHORTED_VALUE)
					ReturnValue = SHORTED;
				else //NTC values in range. Calculate temperature
				{
					NTCRes = ADC_Voltage_Values[newSensor] * SerialRes / (3300 - ADC_Voltage_Values[newSensor]);
					fLog = log(NTCRes/R25);
					fTemp1 = (fLog/NTCB);
					fTemp2 = 1/298.15;
					ReturnValue = (1.0f/(fTemp1+fTemp2));
					ReturnValue -= 273.15f;
					ReturnValue *= 10.0f; //x 0.1 °C
				}
			}
			break;
		}
		default:
		{
			ReturnValue = ADC_Voltage_Values[newSensor];//ADC_Voltage_Values[newSensor];
			break;
		}
	}
	return ReturnValue;
}
