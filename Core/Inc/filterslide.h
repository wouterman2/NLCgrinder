//-----------------------------------------------------------------------------
//! \file       filterslide.h
//! \author     R. Weimar
//! \brief      Contains routines for the filter slide module
//! \details
//! \Attention
//-----------------------------------------------------------------------------

#ifndef INC_FILTERSLIDE_H_
#define INC_FILTERSLIDE_H_
//-----------------------------------------------------------------------------
//! \brief  includes
#include <stdint.h>
#include "work.h"
//-----------------------------------------------------------------------------
typedef struct
{
	enuStatus Status;
}stcFilterSlide;

extern uint8_t SLD_GetStatus (void);
#endif /* INC_FILTERSLIDE_H_ */
