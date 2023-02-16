//-----------------------------------------------------------------------------
//! \file       recipe.c
//! \author     R. Weimar. RMB Holland bv
//! \brief      Contains routines for the recipe module
//! \details
//! \Attention
//! \Created on: June 9, 2022
//-----------------------------------------------------------------------------

#include "recipe.h"
#include "work.h"
#include "filter.h"
#include "serial.h"
//! \Recipe structure of respectively coffee, milk and water
stcRecipe gRecipe;
stcRecipe gRecipeStorage[FilterCount];
//-----------------------------------------------------------------------------
//! \brief      Returns the grind weight
//! \details    Returns the grind weight
//! \param[out]	uint16_t GrindWeight
uint16_t REC_GetRequiredWeight (void)
{
	return gRecipe.RecipeBlocks[0].GrindWeight;
}
//-----------------------------------------------------------------------------
//! \brief      Returns the grind size
//! \details    Returns the grind size
//! \param[out]	uint16_t GrindSize
uint16_t REC_GetGrindSize (void)
{
	return gRecipe.RecipeBlocks[0].GrindSize;
}
//-----------------------------------------------------------------------------
//! \brief      Initiates the recipe unit
//! \details    Fills default recipe values
//! \param      None
void REC_Init(void)
{
	if (DUMMYMODEGRINDER)
	{
		gRecipe.RecipeBlocks[0].GrindWeight = 120;
		gRecipe.RecipeBlocks[0].GrindSize = 350;
		gRecipe.RecipeBlocks[0].ShutterTime = 225;
		gRecipe.Blocks = 1;
		gRecipe.Valid = 1;
	}
}

//-----------------------------------------------------------------------------
//! \brief      Returns the number of blocks in the recipe
//! \details    Returns the number of blocks in the recipe RecipeType
//! \param[out]	uint8_t Number of Blocks
uint8_t REC_GetNrOfBlocks(void)
{
	return gRecipe.Blocks;
}

//-----------------------------------------------------------------------------
//! \brief      Returns the current block number the recipe
//! \details    Returns the current block number the recipe defined by RecipeType
//! \param[out]	uint8_t Active Block Number
uint8_t REC_GetCurrentBlock(void)
{
	return gRecipe.CurrentBlock;
}

//-----------------------------------------------------------------------------
//! \brief      clears the recipe
//! \details    Resets all values of the recipe
//! \params		None
void REC_ClearRecipe(void)
{
	gRecipe.RecipeBlocks[0].GrindWeight = 0;
	gRecipe.RecipeBlocks[0].GrindSize = 0;
	gRecipe.RecipeBlocks[0].ShutterTime = 0;
	gRecipe.Valid = 0;
	gRecipe.Blocks = 0;
}
