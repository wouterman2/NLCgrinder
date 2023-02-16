#ifndef __RFID_H
#define __RFID_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stm32l1xx_hal.h"
#include "clrc663.h"

extern SPI_HandleTypeDef hspi2;

/*!
  @brief Reset HF-Frontend + Init Cs Pin Level.

  Hardware + SW Reset of RC663 Frontend.
 */
void RFID_ResetFrontEnd(void);

/*!
  @brief Init the HF Frontend to 14443 Protocol 106kbit

  Init the RFID frontend for ISO14443A
	\return The value of the error register -> 0 means no error
 */
unsigned char RFID_Init14443(void);

/*!
  @brief Set the HF field On or Off

  Turn the HF Field on or Off
  \param [in] OnOff A value of 0 means Off else On
 */
void RFID_Field(unsigned char OnOff);

/*!
  @brief Perform an HF reset based on given timeout

  Perfrom HF field reset with given time - typically 5ms
  \param [in] timeoutms timeout value between HF reset in ms
 */
void RFID_FieldReset(unsigned char timeoutms);

/*!
  @brief Get Complete Card memory Dump

  Reads complete Memory of NTAG216
  \param [in] Data pointer to the storage array 56*16 bytes are read last 8 need to be masked out (Block 226 and 227)
 \return Status of operation 1 in case of success, else 0
 */
unsigned char RFID_GetCardMemory(unsigned char* Data);

#ifdef __cplusplus
}
#endif

#endif /* __RFID_H */
