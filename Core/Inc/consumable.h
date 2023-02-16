//-----------------------------------------------------------------------------
//! \file       consumable.h
//! \author     R. Weimar
//! \brief      Contains routines for the consumable module
//! \details
//! \Attention
//-----------------------------------------------------------------------------

#ifndef INC_CONSUMABLE_H_
#define INC_CONSUMABLE_H_
//-----------------------------------------------------------------------------
//! \brief  includes
#include "work.h"
#include <stdint.h>
//-----------------------------------------------------------------------------
#define TAGDATACOUNT 	58
typedef enum
{
	cUNDEFINED = 0,
	cINITIALIZE = 1,
	cNOTPRESENT = 2,
	cPRESENT = 3,

} enuConsumable;
typedef struct
{
	enuConsumable Status;
	unsigned char CardMemory[56*16]; //For both NTAG215 (504 bytes) and NTAG216 (888 bytes)
	char TagVersion;
}stcConsumable;

//extern unsigned char CardMemory[56*16];
extern stcConsumable CON;
extern enuConsumable CON_GetStatus (void);
extern void CON_Handle(void);
extern void CON_Init (void);
#endif /* INC_CONSUMABLE_H_ */
