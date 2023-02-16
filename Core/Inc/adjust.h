//-----------------------------------------------------------------------------
//! \file       adjust.h
//! \author     R. Weimar
//! \brief      Contains routines for the adjust module
//! \details
//! \Attention
//-----------------------------------------------------------------------------

#ifndef INC_ADJUST_H_
#define INC_ADJUST_H_
//-----------------------------------------------------------------------------
//! \brief  includes
#include "work.h"
#include <stdint.h>
//-----------------------------------------------------------------------------
#define ADJ_TIMEOUT 5000
#define ADJ_TIMEOUTNORMAL 3000
#define ADJ_PULSESPER10UM 165 //10x to incorporate 1 decimal (So basicall 16.5 pulses per um)
#define ADJ_HOMEOFFSET ((ADJ.CalibrationSize - ADJ.HomeSize)*ADJ_PULSESPER10UM/10)
#define ADJ_HOMETIMEOUT 50
#define ADJ_MAXCONTROL 10000
#define ADJ_MAXPULSES	15000


#define ADJ_CCW()		TIM10->CCR1
#define ADJ_CW() 		TIM11->CCR1

#define ADJ_Home()		HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin)
#define CW	0
#define CCW	1

typedef struct
{
	enuStatus Status;
	uint8_t Step;
	uint16_t SetGrindSize;
	uint16_t GetGrindSize;
	uint16_t GrindSizeAccuracy;
	uint16_t Play;
	uint16_t HomeSize;
	uint16_t CalibrationSize;
	uint32_t Timer;
}stcADJ;

typedef struct
{
	enuStatus Status;
	uint8_t IsHomed;
	uint8_t SpeedControl;
	uint8_t PosControl;
	uint8_t HomeFlag;
	uint16_t Ratio;
	uint32_t TimeOut;
	int32_t SetSpeed;
	int32_t GetSpeed;
	int32_t SetPosition;
	int32_t GetPosition;
	int32_t GetPositionOld;
	int32_t PositionDelta;
	int32_t GetUm;
	float P;
	float I;
	float D;
	float SpeedP;
	float SpeedI;
	float SpeedD;
	uint8_t PulsesPerRevolution;   //12
	int32_t Encoder;
	int32_t Control;
	float ErrorPOld;
	float ErrorP;
	float ErrorI;
	float ErrorD;
	float PID;
	float SpeedErrorPOld;
	float SpeedErrorP;
	float SpeedErrorI;
	float SpeedErrorD;
	float SpeedPID;
	uint16_t TimerOld;
} stcDCMotor;

extern uint8_t FlgEnc;
extern uint8_t ADJ_OverFlow;
extern stcADJ ADJ;
extern stcDCMotor ADJ_Motor;

void ADJ_HandlePosPID (void);
void ADJ_HandleSpeedPID (void);
void ADJ_SetPWM (enuStatus newStatus, uint8_t newSpeed);
void ADJ_SetUm (void);

extern uint16_t ADJ_GetParameter (uint8_t newParameter);
extern void ADJ_HandleMotor (void);
extern uint8_t ADJ_HandleTask(enuStatus newStatus);
extern void ADJ_Init(void);
extern void ADJ_ResetPosition (void);
extern uint8_t ADJ_Set (uint16_t newGrindSize);
extern void ADJ_SetParameter (uint8_t newParameter, uint16_t newValue);

#endif /* INC_ADJUST_H_ */
