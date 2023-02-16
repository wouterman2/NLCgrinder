//-----------------------------------------------------------------------------
//! \file       hopper.h
//! \author     R. Weimar
//! \brief      Contains routines for the hopper module
//! \details
//! \Attention
//-----------------------------------------------------------------------------

#ifndef INC_HOPPER_H_
#define INC_HOPPER_H_
//-----------------------------------------------------------------------------
//! \brief  includes
#include <stdint.h>
#include "eeprom.h"
//-----------------------------------------------------------------------------
//#define PULSE_0_DEGREES	 		600
//#define PULSE_180_DEGREES		2475
#define HOP_MAX_OPERATION_TIME	1000 //ms. Maximum operating time of the servo. If not finished the movement in this time, it will be disabled.
#define HOP_ANGLE				TIM3->CCR3
typedef enum
{
	hUNDEFINED = 0,
	hINITIALIZE = 1,
	hNOTPLACED = 2,
	hCLOSED = 3,
	hDUMPPOSITION= 4,
	hDOSEPOSITION = 5,
	hFILLPOSITION = 6,
	hCHECKWEIGHT = 7,
} enuHopper;

typedef struct
{
	enuHopper Status;
	uint16_t TimeMin;
	uint16_t TimeMax;
	uint8_t DumpAngle;
	uint8_t DoseAngle;
	uint8_t DoseStepsMax;
	uint8_t SetAngle;
	uint8_t GetAngle;
	uint32_t TimeOut;
	uint32_t TimerValue;
	uint32_t Counter;
	uint32_t DoseCounter;
	uint16_t ClosedPulseTime;
	uint16_t OpenedPulseTime;
}stcHOP;

extern stcHOP HOP;
extern enuHopper HOP_GetStatus (void);
extern void HOP_Handle (void);
extern void HOP_Init(void);
extern void HOP_SetAngle (uint8_t newAngle);
extern void HOP_SetParameter (uint8_t newAddress, uint16_t newValue);
#endif /* INC_HOPPER_H_ */
