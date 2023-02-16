//-----------------------------------------------------------------------------
//! \file       grinder.h
//! \author     R. Weimar
//! \brief      Contains routines for the grinder module
//! \details
//! \Attention
//-----------------------------------------------------------------------------

#ifndef INC_GRINDER_H_
#define INC_GRINDER_H_
//-----------------------------------------------------------------------------
//! \brief  includes
#include "work.h"
#include <stdint.h>
//-----------------------------------------------------------------------------
typedef struct
{
	enuStatus Status;
	uint32_t Current;
}stcGND;

extern stcGND GND;

extern enuStatus GND_GetStatus (void);
extern void GND_Init(void);
extern uint8_t GND_Set (uint16_t newGrindTime);


#endif /* INC_GRINDER_H_ */
