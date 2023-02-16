//-----------------------------------------------------------------------------
//! \file       recipe.h
//! \author     R. Weimar
//! \brief      Contains routines for the recipe module
//! \details
//! \Attention
//-----------------------------------------------------------------------------
#ifndef INC_RECIPE_H_
#define INC_RECIPE_H_
//-----------------------------------------------------------------------------
//! \brief  includes
#include <stdint.h>
#include "serial.h"
//-----------------------------------------------------------------------------
#define RBlocksCount	8
#define RECIPE1			0
#define ActiveBlock		gRecipe.RecipeBlocks[gRecipe.CurrentBlock]

//-----------------------------------------------------------------------------
//! \brief Recipe Storage structure
typedef struct
{
	uint16_t GrindWeight;
	uint16_t GrindSize;
	uint16_t FilterType;
	uint16_t ShutterTime;
} stcRecipeBlock;
typedef struct
{
	uint8_t Valid;
	uint8_t Active;
	uint8_t Blocks;
	uint8_t CurrentBlock;
	stcRecipeBlock RecipeBlocks[RBlocksCount];
}stcRecipe;
//-----------------------------------------------------------------------------
//! \brief  Public variables
extern stcRecipe gRecipe;
//! \brief  Public functions
extern void REC_ClearRecipe(void);
extern uint8_t REC_GetNrOfBlocks(void);
extern uint8_t REC_GetCurrentBlock(void);
extern uint16_t REC_GetRequiredWeight (void);
extern uint16_t REC_GetGrindSize (void);
extern void REC_Init(void);
#endif /* INC_RECIPE_H_ */
