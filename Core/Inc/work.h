//-----------------------------------------------------------------------------
//! \file       work.h
//! \author     R. Weimar
//! \brief      Contains routines for the work module
//! \details
//! \Attention
//-----------------------------------------------------------------------------
#ifndef INC_WORK_H_
#define INC_WORK_H_
//-----------------------------------------------------------------------------
//! \brief  includes
#include "eeprom.h"
#include <stdint.h>
#include "defines.h"

//-----------------------------------------------------------------------------
#define ACTIONTIMEOUT 	500
#define SW_VER_MAJOR	2   //0 - 15   2 = Hardware 2
#define SW_VER_MINOR	2   //0 - 15   >2 = Bootloader version
#define SW_VER_BUGFIX	20
//0 - 255
#define SERIALNUMBER	0xf1456f12
#define HARDWAREVERSIONMINIMUM 2
#define HW0		(HAL_GPIO_ReadPin(HW0_GPIO_Port, HW0_Pin) == GPIO_PIN_SET)
#define HW1		(HAL_GPIO_ReadPin(HW1_GPIO_Port, HW1_Pin) == GPIO_PIN_SET)
#define HW2		(HAL_GPIO_ReadPin(HW2_GPIO_Port, HW2_Pin) == GPIO_PIN_SET)
#define HW3		(HAL_GPIO_ReadPin(HW3_GPIO_Port, HW3_Pin) == GPIO_PIN_SET)
#define LED_SPI 1

//-----------------------------------------------------------------------------
extern uint8_t Flg1ms;
extern uint8_t FlgScl;
extern uint8_t Initialized;

//-----------------------------------------------------------------------------
//! \brief private function prototypes
uint8_t WRK_GetAction (uint8_t ReqAction);
uint32_t WRK_GetHardwareVersion (void);
void WRK_HandleIdle (void);
void WRK_HandlePrepare (void);
void WRK_HandlePrepareRecipe (void);
void WRK_ResetAllModules(uint8_t newRecipe);
void WRK_HandleSequence (void);
void WRK_HandleStandby (void);
void WRK_HandleUndefined (void);
void WRK_SetMainStatus (enuStatus newStatus);
void WRK_SetSubStatus (enuStatus newStatus);
//-----------------------------------------------------------------------------
extern uint8_t DUMMYMODEGRINDER;
extern uint8_t DUMMYMODEFILTER;
extern uint8_t DUMMYMODERFID;
extern uint8_t DUMMYMODESCALE;
//! \brief  Public function prototypes
extern uint32_t WRK_GetHardwareVersion (void);
extern enuStatus WRK_GetMainStatus (void);
extern enuPrepare WRK_GetPrepareStatus (void);
extern enuStatus WRK_GetSubStatus (void);
extern uint32_t WRK_GetSoftwareVersion (void);
extern void WRK_Init (void);
extern void WRK_HandleTickTime (void);
extern void WRK_ResetAction (void);
extern void WRK_SetAction (uint8_t ReqAction);
extern void WRK_SetDummyMode (uint8_t newAddress, uint8_t newValue);

#endif /* INC_WORK_H_ */
