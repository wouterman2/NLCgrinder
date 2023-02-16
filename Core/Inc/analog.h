//-----------------------------------------------------------------------------
//! \file       analog.h
//! \author     R. Weimar
//! \brief      Contains routines for analog measurement
//! \details
//! \Attention
//-----------------------------------------------------------------------------

#ifndef INC_ANALOG_H_
#define INC_ANALOG_H_
//-----------------------------------------------------------------------------
//! \brief  includes
#include <stdint.h>
//-----------------------------------------------------------------------------
//! \brief defines
#define NTC_DISCONNECTED_VALUE	3900
#define NTC_SHORTED_VALUE		100
#define DISCONNECTED				-273.15
#define SHORTED						1200.15

typedef enum
{
	ADC_BURRS = 0, //Burr sensor
	ADC_MOTOR = 1, //Motor sensor
	ADC_SW5 = 2, //HAL 0
	ADC_SW6 = 3, //HAL 1
	ADC_SW7 = 4, //HAL 2
} enuSensor;
//! \Brief public variables
extern uint16_t vrefint_cal;
//-----------------------------------------------------------------------------
//! \brief private defines
#define NROFANALOGSENSORS 6
extern uint16_t ADC_Voltage_Values[NROFANALOGSENSORS-1];
//-----------------------------------------------------------------------------
//! \brief Public functions
extern void ANA_Init(void);
extern float ANA_GetSensorValue(enuSensor newSensor);
#endif /* INC_ANALOG_H_ */
