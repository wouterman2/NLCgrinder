//-----------------------------------------------------------------------------
//! \file       scale.h
//! \author     R. Weimar
//! \brief      Contains routines for the scale module
//! \details
//! \Attention
//-----------------------------------------------------------------------------
#ifndef INC_SCALE_H_
#define INC_SCALE_H_
//-----------------------------------------------------------------------------
//! \brief  includes
#include "work.h"
#include "eeprom.h"
#include <stdint.h>
#include "eeprom.h"
//-----------------------------------------------------------------------------
#define SCL_TARE_SAMPLES 20
#define SCL_SAMPLES 1

typedef struct
{
	enuStatus Status;
	uint8_t Tared;
	uint8_t Stable;
	int32_t GetWeight;
	int32_t GetWeightOld;
	int32_t ShotWeight;
	int32_t RestWeight;
	uint8_t Accuracy;
	//int32_t Weight;
	int32_t offset;
	int32_t gain;
	int32_t rawvalue;
	int32_t StableTime;
	float factor;
}stcSCL;

extern stcSCL SCL;
//Local functions
int32_t SCL_Tare(void);
int32_t SCL_ReadValue(void);
//Public functions
extern enuStatus SCL_GetStatus (void);
extern int32_t SCL_GetWeight (void);
extern void SCL_Measure (uint8_t newCycles);
extern void SCL_Init(void);
extern void SCL_Measure (uint8_t newCycles);
extern void SCL_SetParameter (uint8_t newAddress, uint16_t newValue);
#endif /* INC_SCALE_H_ */
