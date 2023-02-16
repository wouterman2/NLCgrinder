//-----------------------------------------------------------------------------
//! \file       consumable.c
//! \author     R. Weimar. RMB Holland bv
//! \brief      Contains routines for the consumable module
//! \details
//! \Attention
//! \Created on: May 5 2022
//-----------------------------------------------------------------------------

#include "consumable.h"
#include "string.h"
#include "serial.h"
#include "crc.h"
#include "rfid.h"
#include "led.h"
stcConsumable CON;

//-----------------------------------------------------------------------------
//! \brief      Initializes the consumable module
//! \details    Initializes the consumable module
//! \params		None
void CON_Init (void)
{
	for (uint8_t i = 0; i < TAGDATACOUNT ;i++)
	{
		CON.CardMemory[i] = 0;
	}
	CON.CardMemory[21] = 42;
	CON.CardMemory[22]= 0;
	CON.CardMemory[23]= 0;
	CON.CardMemory[24]= 0;
	CON.CardMemory[25]= 1;
	CON.CardMemory[26] = 'F';
	CON.CardMemory[27] = 'a';
	CON.CardMemory[28] = 'n';
	CON.CardMemory[29] = 'c';
	CON.CardMemory[30] = 'y';
	CON.CardMemory[31] = ' ';
	CON.CardMemory[32] = 'R';
	CON.CardMemory[33] = 'o';
	CON.CardMemory[34] = 'a';
	CON.CardMemory[35] = 's';
	CON.CardMemory[36] = 't';
	if (DUMMYMODERFID)
	{
		CON.Status = cPRESENT;
	}
	else
	{
		CON.Status = cNOTPRESENT;
	}
	//Init and Setup RFID Frontend
	RFID_ResetFrontEnd();
	RFID_Init14443();
}
//-----------------------------------------------------------------------------
//! \brief      Returns the consumable status
//! \details    Returns the consumable status
//! \param[out]	enuConsumable CON.Status
enuConsumable CON_GetStatus (void)
{
	return CON.Status;
}

//-----------------------------------------------------------------------------
//! \brief      Handles the RFID reading
//! \details    Returns the RFID content
//! \params		None
void CON_Handle(void)
{
	static uint16_t TickTime=0;
	TickTime ++;
	if (TickTime == 501) //Wait 500 ms so RID is not blocking all UART traffic
	{
		RFID_Field(0);
	}
	else if (TickTime == 506) //5 ms later
	{
		RFID_Field(1);
		//memset(CardMemory,0,56*16); //Clear the tag memory
		//Check if NTAG215 or NTAG216 is detected and if, read complete user memory (Block8-Block227)
		if(RFID_GetCardMemory(CON.CardMemory))
		{
			if (CON.TagVersion == 0x11) //NTAG215
			{
				//Last 8 Bytes need to be ignored - read until block 131 instead of 129!
				memset(&CON.CardMemory[32*16-8],0,8);
				LED_Set(STANDBYLED,SWITCHEDON, 0, 255, 0, 100, 500, 500, 0, 0, 0);
			}
			else if (CON.TagVersion == 0x13) //NTAG216
			{
			  //Last 8 Bytes need to be ignored - read until block 227 instead of 225!
			  memset(&CON.CardMemory[56*16-8],0,8);
			  LED_Set(STANDBYLED,SWITCHEDON, 0, 255, 0, 100, 500, 500, 0, 0, 0);
			}
		  //Now CardMemory can be used to check the recipe
		}
		else
		{
			LED_Set(STANDBYLED,SWITCHEDON, 255, 0, 0, 100, 500, 500, 0, 0, 0);
		}
		TickTime = 0;
	}
}
