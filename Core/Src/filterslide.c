//-----------------------------------------------------------------------------
//! \file       filterslide.c
//! \author     R. Weimar. RMB Holland bv
//! \brief      Contains routines for the filterslide module
//! \details
//! \Attention
//! \Created on: Apr 27, 2022

#include "filterslide.h"
#include "filter.h"

//! \Filter structure
stcFilterSlide SLD;
//-----------------------------------------------------------------------------
//! \brief      Returns the status of the filter slide
//! \details    Returns the status of the filter slide
//! \param[out] uint8_t status 0 = de-activated, 1 = activated
uint8_t SLD_GetStatus (void)
{
	//To be added: Evaluation of the sensors to determine the filter type

	if (FIL.Type == 0 )
		SLD.Status = sNOTINPOSITION;
	else
		SLD.Status = sINPOSITION;
	return SLD.Status;
}
