/*
  The MIT License (MIT)

  Copyright (c) 2016 Ivor Wanders

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef CLRC663_H_
#define CLRC663_H_

#include "main.h"
#include "clrc663_def.h"

#ifdef __cplusplus
extern "C" {
#endif

// debug print statement.
#ifdef CLRC663_DEBUG_PRINTF
#define CLRC663_PRINTF(...) CLRC663_DEBUG_PRINTF(__VA_ARGS__)
#else
#define CLRC663_PRINTF(...)
#endif

// ---------------------------------------------------------------------------
// HAL functions.
// ---------------------------------------------------------------------------
/*! \defgroup hal Hardware Abstraction Layer
    \brief Functions that interact with the SPI hardware.

    These functions should provide all the neccessary interaction with the hardware. Only the SPI bus is used;
    two functions to select and deselect the chip and a function to transmit and receive bytes over the SPI bus.

    In the examples folder you can find implementations for these three functions for an Arduino-compatible platform
    and for use on an STM32F0xx chip using STM's Cube HAL.

    \warning These functions are marked as `extern` and it is up to the user to ensure that they exist and work
             correctly.
  @{
*/

/*!
  @brief Transmit and receive bytes via SPI.
  
    This function should provide the main SPI transfer functionality. SPI always reads a byte at the same time as it
    transmits a byte, the function should read from the `tx` array to transmit this data over MOSI and it should write
    the data received via MISO to the `rx` array.


  \param [in] len This is the number of bytes to be transfered.
  \param [in] tx The bytes from this array are transmitted, `len` bytes are always read from this argument. (MOSI)
  \param [out] rx The bytes received during transmission are written into this array, `len` bytes are always written.
               (MISO)
 */
extern void clrc663_SPI_transfer(const uint8_t* tx, uint8_t* rx, uint16_t len);

/*!
  @brief Selects the CLRC663 to accept data from the SPI bus.
  
    This function should set the Chip Select (NSS) line to the appropriate level such that the chip acceps the data
    on the SPI bus. For the CLRC663 this means setting the Chip Select line to a LOW logic level.

 */
extern void clrc663_SPI_select();

/*!
  @brief Deselects the CLRC663 to stop accepting data from the SPI bus.
  
    This function should set the Chip Select (NSS) line to the appropriate level such that the chip does not accept the
    data on the SPI bus. For the CLRC663 this means setting the Chip Select line to a HIGH logic level.

 */
extern void clrc663_SPI_unselect();


//! @}
// ---------------------------------------------------------------------------
// Register interaction functions.
// ---------------------------------------------------------------------------

/*! \defgroup register Register interaction.
    \brief Manipulate the chip's registers.

    These functions use the SPI communication functions to interact with the hardware. All interaction with the chip
    goes through these functions. In case another interface than SPI is to be used, these functions can be adapted.
  @{
*/

/*!
  @brief Reads a register.
  
  \param [in] reg Specifies which register to read.
  \return the value of the register to be read.
 */
uint8_t clrc663_read_reg(uint8_t reg);

/*!
  @brief Write a register.
  
  Sets a single register to the provided value.
  \param [in] reg Specifies which register to write.
  \param [in] value Specifies the value to write to that register.
 */
void clrc663_write_reg(uint8_t reg, uint8_t value);


/*!
  @brief Write multiple registers.
  
  Sets consecutive registers to the provided values.
  \param [in] reg Specifies at which register writing starts.
  \param [in] values An array of the values to write to the register starting from `reg`.
              The first value (`values[0]`) gets written to `reg`, the second (`values[1]`) to `reg+1`, and so on.
  \param [in] len The number of register to write.
 */
void clrc663_write_regs(uint8_t reg, const uint8_t* values, uint8_t len);

/*!
  @brief Write data to FIFO.
  
  The FIFO is located at register `#CLRC663_REG_FIFODATA`. 
  Writes to this register do not automatically increment the write pointer in the chip and multiple bytes may be written
  to this register to place them into the FIFO buffer.

  This function does not clear the FIFO beforehand, it only provides the raw transfer functionality.
  \param [in] data The data to be written into the FIFO.
  \param [in] len The number of bytes to be written into the FIFO.
 */

void clrc663_write_fifo(const uint8_t* data, uint16_t len);

/*!
  @brief Read data from FIFO.
  
  This function reads data from the FIFO into an array on the microcontroller.

  \param [out] rx The data read from the FIFO is placed into this array.
  \param [in] len The number of bytes to be read from the FIFO.

  \warning This reads regardless of `#CLRC663_REG_FIFOLENGTH`, if there aren't enough bytes present in the FIFO, they
           are read from the chip anyway, these bytes should not be used. (The returned bytes from an empty FIFO are
           often identical to the last valid byte that was read from it.)
 */
void clrc663_read_fifo(uint8_t* rx, uint16_t len);
//! @}

// ---------------------------------------------------------------------------
// Command functions
// ---------------------------------------------------------------------------

/*! \defgroup commands Commands
    \brief These activate the various commands of the chip with the right arguments.

    The chip has various commands it can execute, these commands often have arguments which should be transferred to
    the FIFO before the command is initiated. These functions provide this functionality of transferring the arguments
    to the FIFO and then initating the commands.
  @{
*/

/*!
  @brief Read data from EEPROM into the FIFO buffer.
  
  This instruction transfers data from the EEPROM (section 2) at the given address locaction into the FIFO buffer.

  \param [in] address The start address in the EEPROM to start reading from.
  \param [in] length The number of bytes to read from the EEPROM into the FIFO buffer.
 */
void clrc663_cmd_read_E2(uint16_t address, uint16_t length);

/*!
  @brief Read data from EEPROM into the registers.
  
  This instruction transfers data from the EEPROM (section 2) at the provided address into the registers..

  \param [in] address The start address in the EEPROM to start reading from.
  \param [in] regaddr The start address of the register to start writing into.
  \param [in] length The number of bytes to read and registers to write consecutively from `regaddr`.
 */
void clrc663_cmd_load_reg(uint16_t address, uint8_t regaddr, uint16_t length);

/*!
  @brief Load protocol settings.
  
  Loads register settings for the protocol indicated. Can configure different protocols for rx and tx. The most common
  protocol is `#CLRC663_PROTO_ISO14443A_106_MILLER_MANCHESTER` which is the default protocol for the SELECT procedure.

  The most common protocols are listed in the datasheet, but the CLRC663 Quickstart Guide AN11022 gives a complete
  description.

  \param [in] rx The protocol number to load for the receiving frontend.
  \param [in] tx The protocol number to load for the tranmitting frontend.

 */
void clrc663_cmd_load_protocol(uint8_t rx, uint8_t tx);

/*!
  @brief Transmit the bytes provided and go into receive mode.

  This function loads the data from the `data` array into the FIFO, and then issues the `CLRC663_CMD_TRANSCEIVE`
  command, which sends the data in the FIFO and switches to receiving mode afterwards.

  \param [in] data The data to be transmitted.
  \param [in] len The numeber of bytes to be read from `data` and be transmitted..
 */
void clrc663_cmd_transceive(const uint8_t* data, uint16_t len);

/*!
  @brief Set the device into idle mode.

  Stops the currently active command and return to idle mode.
 */
void clrc663_cmd_idle();

/*!
  @brief Loads a key from the EEPROM into the key buffer.

  This function can load a key from the MIFARE key area in the EEPROM into the key buffer. This section of the EEPROM
  can only be written. The key buffer is a part of memory in the CLRC663 used for the MIFARE authentication procedure.

  \param [in] key_nr Loads the key stored for this index.
 */
void clrc663_cmd_load_key_E2(uint8_t key_nr);

/*!
  @brief Loads the provided key into the key buffer.

  This function reads 6 bytes from the `key` array into the FIFO and then loads the key into the key buffer.

  \param [in] key Array which holds the MIFARE key, it is always 6 bytes long.
 */
void clrc663_cmd_load_key(const uint8_t* key);

/*!
  @brief Soft resets the clrc663 device

 */
void clrc663_cmd_softreset();

/*!
  @brief Perform MIFARE authentication procedure with a card.

  This function attemps to authenticate with the specified card using the key which is currently in the key buffer.
  This function is usually preceded by either the `clrc663_cmd_load_key_E2()` or `clrc663_cmd_load_key()` functions.

  \param [in] key_type The MIFARE key A or B (`CLRC663_MF_AUTH_KEY_A` = 0x60 or `CLRC663_MF_AUTH_KEY_B` = 0x61) to use.
  \param [in] block_address The block on which to authenticate.
  \param [in] card_uid The authentication procedure required the first four bytes of the card's UID to authenticate.
 */
void clrc663_cmd_auth(uint8_t key_type, uint8_t block_address, const uint8_t* card_uid);

//! @}

// ---------------------------------------------------------------------------
// Utility functions
// ---------------------------------------------------------------------------
/*! \defgroup utility Utility
    \brief Various utility functions for often performed actions.

  @{
*/


/*!
  @brief Flush the FIFO buffer.

  This function clears all contents that are currently in the FIFO.

 */
void clrc663_flush_fifo();

/*!
  @brief Get the FIFO length.

  Returns the current number of bytes in the FIFO.

  \warning This function only returns the first 8 bits of the FIFO length, if the 512 byte FIFO is used, only the least
           significant eight bits will be returned.
  
  \return The number of bytes currently in the FIFO.
 */
uint16_t clrc663_fifo_length();


/*!
  @brief Clear the interrupt0 flags.

  Resets the interrupt 0 register (`CLRC663_REG_IRQ0`).
 */
void clrc663_clear_irq0();
/*!
  @brief Clear the interrupt1 flags.

  Resets the interrupt 1 register (`CLRC663_REG_IRQ1`).
 */
void clrc663_clear_irq1();


/*!
  @brief Get the value of the interrupt 0 register.

  \return The value of the `CLRC663_REG_IRQ0` register.
 */
uint8_t clrc663_irq0();

/*!
  @brief Get the value of the interrupt 1 register.

  \return The value of the `CLRC663_REG_IRQ1` register.
 */
uint8_t clrc663_irq1();

/*!
  @brief Print an array in hexadecimal format using `CLRC663_PRINTF`.

  Prints the bytes in `data` in hexadecimal format, separated by spaces using the `CLRC663_PRINTF` macro, if defined.

  \param [in] data The array to be printed.
  \param [in] len The number of bytes to print..
 */
void clrc663_print_block(const uint8_t* data, uint16_t len);

/*!
  @brief Copy a page from EEPROM into an array on the MCU.
  
  This instruction transfers a page from the EEPROM into the FIFO and then transfers this data from the FIFO
  into an array. It always transfers 64 bytes, as such `dest` must be (atleast) 64 bytes long.

  This basically calls clrc663_cmd_read_E2() and then transfers the FIFO with clrc663_read_fifo(). This is useful for
  dumping the entire EEPROM.

  \param [out] dest The array to write the data into.
  \param [in] page The page to read from the EEPROM. (This gets multiplied by 64 to obtain the start address).
  \return The number of bytes transmitted from the FIFO into `dest`.
 */
uint8_t clrc663_transfer_E2_page(uint8_t* dest, uint8_t page);

//!  @}

// ---------------------------------------------------------------------------
// Timer functions
// ---------------------------------------------------------------------------
/*! \defgroup timer Timer
    \brief Functions for manipulating the timers.

    The CLRC663 has 5 timers, the first four can be treated more or less similarly, the last timer `Timer4` has a
    different control register.

    Timer 0-3 can be treated in a similar way, and as such the functions take an argument that specifies which timer
    to manipulate.

    Timer4 is special, read the datasheet on how to use that timer as it has other clock sources and properties.


  @{
*/

/*!
  @brief Activates a timer.

  This sets the the `CLRC663_REG_TCONTROL` register to enable or disable this timer.

  \note Seems to trigger timer reset?

  \param [in] timer Specifies which timer to use (0, 1, 2 or 3).
  \param [in] active Should be `0` to deactivate the timer, `1` to activate it.
 */
void clrc663_activate_timer(uint8_t timer, uint8_t active);

// Set the timer control field of the timer.
// value: the value to set the timer's control field to.
/*!
  @brief Sets the timer control register.

  This sets the `T[0-3]Control` register to the provided value. The value speficies the propertief of StopRx, Start
  AutoRestart and Clk for this timer.

  \param [in] timer Specifies which timer to use (0, 1, 2 or 3).
  \param [in] value This can be a combination of the defines associated with the Timer controls.
  \see CLRC663_TCONTROL_STOPRX
  \see CLRC663_TCONTROL_START_NOT, CLRC663_TCONTROL_START_TX_END, CLRC663_TCONTROL_START_LFO_WO,
       CLRC663_TCONTROL_START_LFO_WITH
  \see CLRC663_TCONTROL_CLK_13MHZ, CLRC663_TCONTROL_CLK_211KHZ, CLRC663_TCONTROL_CLK_UF_TA1, CLRC663_TCONTROL_CLK_UF_TA2
 */
void clrc663_timer_set_control(uint8_t timer, uint8_t value);

/*!
  @brief Sets the reload value of the timer.

  This counter starts counting down from this reload value, an underflow occurs when the timer reaches zero.

  \param [in] timer Specifies which timer to use (0, 1, 2 or 3).
  \param [in] value The value from which to start the counter. 
 */
void clrc663_timer_set_reload(uint8_t timer, uint16_t value);

/*!
  @brief Sets the current value of this timer..

  Sets the current value of this counter, it counts down from this given value.

  \param [in] timer Specifies which timer to use (0, 1, 2 or 3).
  \param [in] value The value to set the counter to. 
 */
void clrc663_timer_set_value(uint8_t timer, uint16_t value);


/*!
  @brief Retrieve the current value of a timer.

  Reads the current value of the given timer and returns the result.

  \param [in] timer Specifies which timer to use (0, 1, 2 or 3).
  \return The current value of this timer.
 */
uint16_t clrc663_timer_get_value(uint8_t timer);
//!  @}

// ---------------------------------------------------------------------------
// ISO 14443A
// ---------------------------------------------------------------------------
/*! \defgroup iso14443a ISO14443A
    \brief Functions for card wakeup and UID discovery.

    These functions modify registers and do not put them back into the original state. However, if called in the right
    order (`clrc663_iso14443a_WUPA_REQA()`, `clrc663_iso14443a_select()`, and then a function from @ref mifare) the
    registers should be in the right state.

    In the ISO norm the terms PCD and PICC are used. PCD stands for Proximity Coupling Device, by which they mean the
    reader device. The PICC is the Proximity Integrated Circuit Card, so the RFID tag / card. I chose to adhere the
    terminology from the CLRC663 datasheet, which simply refers to 'card' and 'reader'.

  @{
*/

/*! \brief Sends an Request Command, Type A.

    This sends the ISO14443 REQA request, cards in IDLE mode should answer to this.

    \return The Answer to request A byte (ATQA), or zero in case of no answer.
*/
uint16_t clrc663_iso14443a_REQA();

/*! \brief Sends an Wake-Up Command, Type A.

    This sends the ISO14443 WUPA request, cards in IDLE or HALT mode should answer to this.

    \return The Answer to request A byte (ATQA), or zero in case of no answer.
*/
uint16_t clrc663_iso14443a_WUPA();

/*! \brief Used to send WUPA and REQA.

    This actually sends WUPA and REQA and returns the response byte.

    \return The Answer to request A byte (ATQA), or zero in case of no answer.
*/
uint16_t clrc663_iso14443a_WUPA_REQA(uint8_t instruction);

/*! \brief Performs the SELECT procedure to discover a card's UID.

  This performs the SELECT procedure as explained in ISO 14443A, this determines the UID of the card, if multiple cards
  are present, a collision will occur, which is handled according to the norm. This collision handling is explained
  quite complex in the norm, but conceptually it is not all that complex:

    - The cascade level can be seen as a prefix to ensure both the PICC and PCD are working on identifying the same
      part of the UID.
    - The entire anti-collision scheme is more of a binary search, the PICC sends the CASCADE level prefix, then the
      NVB byte, this field determines how many bits of the UID will follow, this allows the PICC's to listen to this
      and respond if their UID's match these first bits with the UID that is transmitted. After this all PICC's (that
      have matched the UID bits already sent) respond with the remainder of their UIDS. This results in either a
      complete UID, or in case two PICC's share a few bits but then differ a bit, a collision occurs on this bit. This
      collision is detected by the PCD, at which point it can chose either to pursue the PICC(s) that has a 0b1 at that
      position, or pursue the 0b0 at that position. The ISO norm states: A typical implementation adds a (1)b.
      I use the bit value that's in the pointer at the same position as the collision, or atleast for the first cascade
      level that works, after that it's off by a byte because of the cascade tag, see the actual implementation.

  \param [out] uid: The UID of the card will be stored into this array. This array is also used to determine the choice
                    between an 0b1 or 0b0 when a collision occurs. The bit that's in `uid` at the collision position is
                    selected.
  \param [out] sak: The last SAK byte received during the SELECT procedure is placed here, this often holds information
                    about the type of card.
  \return The length of the UID in bytes (4, 7, 10), or 0 in case of failure.
*/
uint8_t clrc663_iso14443a_select(uint8_t* uid);
//!  @}

// ---------------------------------------------------------------------------
// MIFARE
// ---------------------------------------------------------------------------

/*! \brief Get The Version of the NTAG Product type.

    Reads the Product Version and returns 1 in case of NTAG216 is detected

    \return 1 for NTAG216 detected, otherwise 0 is returned.
*/
uint8_t NTAG_GetVersion(void);

/*! \brief Read a block of memory

    Try to read 16 bytes of data from the card on the given address.


    \param [in] block_address The block to read.
    \param [out] dest The array in which to write the 16 bytes read from the card.
    \return 0 for failure, otherwise the number of bytes received.
*/
uint8_t NTAG_ReadBlock(uint8_t block_address, uint8_t* dest);

#ifdef __cplusplus
}
#endif

#endif  // CLRC663_H_
