//-----------------------------------------------------------------------------
//! \file       rfid.c
//! \author     R. Weimar. RMB Holland bv and GMMC
//! \brief      Contains routines for the rfid module
//! \details
//! \Attention
//! \Created on: August 15th, 2022

#include "rfid.h"
#include "recipe.h"
#include "consumable.h"
#include "main.h"
#include "string.h"

//-----------------------------------------------------------------------------
//! \brief      Initiates the SPI transfer
//! \details    Initiates the SPI transfer
//! \param[in]	tx,	Transmit buffer
//! \param[in]	rx,	Receive buffer
//! \param[in]	len, length of buffer
void clrc663_SPI_transfer(const uint8_t *tx, uint8_t *rx, uint16_t len)
{
	HAL_SPI_TransmitReceive(&hspi2, (unsigned char*) tx, (unsigned char*) rx,
			len, 500);
}
//-----------------------------------------------------------------------------
//! \brief      Selects the SPI
//! \details    Switches NSS low
//! \params		None
void clrc663_SPI_select()
{
	HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_RESET);
}
//-----------------------------------------------------------------------------
//! \brief      Selects the SPI
//! \details    Switches NSS high
//! \params		None
void clrc663_SPI_unselect()
{
	HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);
}
//-----------------------------------------------------------------------------
//! \brief      Resets the NFC chip
//! \details    Reset routine
//! \params		None
void RFID_ResetFrontEnd(void)
{
	HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(RFID_RES_GPIO_Port, RFID_RES_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(RFID_RES_GPIO_Port, RFID_RES_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	clrc663_cmd_idle();      //abort any ongoing command on reader
	clrc663_flush_fifo();    //flush FIFO (MOSI: 0x00 0x00)
	clrc663_cmd_softreset(); //soft reset
}
//-----------------------------------------------------------------------------
//! \brief      Initializes the NFC chip
//! \details    Init routine
//! \params		None
unsigned char RFID_Init14443(void)
{
	uint8_t ProtocolNr[] = { 0x00, 0x00 };

	clrc663_cmd_load_protocol(ProtocolNr[0], ProtocolNr[0]);
	HAL_Delay(10);
	clrc663_flush_fifo();
	unsigned char clrc663_errorReg = clrc663_read_reg(CLRC663_REG_ERROR);
	return clrc663_errorReg;
}
//-----------------------------------------------------------------------------
//! \brief      Sets the antenna
//! \details    Switches field on or off
//! \param[in]	OnOff, 0 = off, 1 = on
void RFID_Field(unsigned char OnOff)
{
	if (OnOff)
	{
		clrc663_write_reg(CLRC663_REG_DRVMOD, 0x89);
	}
	else
	{
		clrc663_write_reg(CLRC663_REG_DRVMOD, 0x81);
	}
}
//-----------------------------------------------------------------------------
//! \brief      performs a field reset
//! \details    Switches field off and on again
//! \param[in]	timeoutms, Delay between off and on
void RFID_FieldReset(unsigned char timeoutms) {
	RFID_Field(0);
	HAL_Delay(timeoutms);
	RFID_Field(1);
}
//-----------------------------------------------------------------------------
//! \brief      Retrieves data from tag
//! \details    Retrieves data and stores it
//! \param[in]	*Data, data container
unsigned char RFID_GetCardMemory(unsigned char *Data) {
	unsigned char UID[10];
	if (clrc663_iso14443a_REQA()) {
		//Card present:

		if (clrc663_iso14443a_select(UID))
		{
			//Card found - check if NTAG216
			CON.TagVersion = NTAG_GetVersion();
			if (CON.TagVersion==0x13)
			{
				//NTAG216 -> Read Complete memory (888/16 = 55.5 -> 56 ignore last 8 bytes
				for (int i = 0; i < 56; i++) {
					if (!NTAG_ReadBlock(4 + 4 * i, &Data[16 * i]))
					{
						return 0;
					}
					//if ((CON.CardMemory[25]>0)&&(CON.CardMemory[25] > 48)) CON.CardMemory[25] -= 48;
					//CON.CardMemory[21] = 42;
					//CON.CardMemory[22]= 48;
					//CON.CardMemory[23]= 48;
					//CON.CardMemory[24]= 48;
						//CardMemory[25]= 1;
					/*CON.CardMemory[26] = 'F';
					CON.CardMemory[27] = 'a';
					CON.CardMemory[28] = 'n';
					CON.CardMemory[29] = 'c';
					CON.CardMemory[30] = 'y';
					CON.CardMemory[31] = ' ';
					CON.CardMemory[32] = 'R';
					CON.CardMemory[33] = 'o';
					CON.CardMemory[34] = 'a';
					CON.CardMemory[35] = 's';
					CON.CardMemory[36] = 't';*/
					CON.Status = cPRESENT;
				}
				return 0x13;
			}
			else if (CON.TagVersion == 0x11)
			{
				//NTAG215 -> Read Complete memory (504/16 = 31.5 -> 32. Ignore last 8 bytes
				for (int i = 0; i < 32; i++)
				{
					if (!NTAG_ReadBlock(4 + 4 * i, &Data[16 * i]))
					{
						return 0;
					}
					//Temporary until tags are written correctly
					if ((CON.CardMemory[25]>0)&&(CON.CardMemory[25] > 48)) CON.CardMemory[25] -= 48;
					CON.CardMemory[21] = 42;
					CON.CardMemory[22]= 0;
					CON.CardMemory[23]= 0;
					CON.CardMemory[24]= 0;
						//CardMemory[25]= 1;
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
					CON.Status = cPRESENT;
				}
				return 0x11;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		CON.Status = cNOTPRESENT;
		REC_ClearRecipe();
		CON.TagVersion = 0;
		memset(CON.CardMemory,0,56*16); //Clear the tag memory
		return 0;
	}
}
