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

#include "clrc663.h"

/** @file */

// ---------------------------------------------------------------------------
// Register interaction functions.
// ---------------------------------------------------------------------------
uint8_t clrc663_read_reg(uint8_t reg) {
	uint8_t instruction_tx[2] = { (reg << 1) | 0x01, 0 };
	uint8_t instruction_rx[2] = { 0 };
	clrc663_SPI_select();
	clrc663_SPI_transfer(instruction_tx, instruction_rx, 2);
	clrc663_SPI_unselect();
	return instruction_rx[1];  // the second byte the returned value.
}

void clrc663_write_reg(uint8_t reg, uint8_t value) {
	uint8_t instruction_tx[2] = { (reg << 1) | 0x00, value };
	uint8_t discard[2] = { 0 };
	clrc663_SPI_select();
	clrc663_SPI_transfer(instruction_tx, discard, 2);
	clrc663_SPI_unselect();
}

void clrc663_write_regs(uint8_t reg, const uint8_t *values, uint8_t len) {
	uint8_t instruction_tx[len + 1];
	uint8_t discard[len + 1];
	instruction_tx[0] = (reg << 1) | 0x00;
	uint8_t i;
	for (i = 0; i < len; i++) {
		instruction_tx[i + 1] = values[i];
	}
	clrc663_SPI_select();
	clrc663_SPI_transfer(instruction_tx, discard, len + 1);
	clrc663_SPI_unselect();
}

void clrc663_write_fifo(const uint8_t *data, uint16_t len) {
	uint8_t write_instruction[] = { (CLRC663_REG_FIFODATA << 1) | 0 };
	uint8_t discard[len + 1];
	clrc663_SPI_select();
	clrc663_SPI_transfer(write_instruction, discard, 1);
	clrc663_SPI_transfer(data, discard, len);
	clrc663_SPI_unselect();
}

void clrc663_read_fifo(uint8_t *rx, uint16_t len) {
	uint8_t read_instruction[] = { (CLRC663_REG_FIFODATA << 1) | 0x01,
			(CLRC663_REG_FIFODATA << 1) | 0x01 };
	uint8_t read_finish[] = { 0 };
	uint8_t discard[2];
	clrc663_SPI_select();
	clrc663_SPI_transfer(read_instruction, discard, 1);
	uint16_t i;
	for (i = 0; i < (len - 1); i++) {
		clrc663_SPI_transfer(read_instruction, rx++, 1);
	}
	clrc663_SPI_transfer(read_finish, rx++, 1);
	clrc663_SPI_unselect();
}

// ---------------------------------------------------------------------------
// Command functions.
// ---------------------------------------------------------------------------

void clrc663_cmd_read_E2(uint16_t address, uint16_t length) {
	uint8_t parameters[3] = { (uint8_t) (address >> 8), (uint8_t) (address
			& 0xFF), length };
	clrc663_flush_fifo();
	clrc663_write_fifo(parameters, 3);
	clrc663_write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_READE2);
}

void clrc663_cmd_load_reg(uint16_t address, uint8_t regaddr, uint16_t length) {
	uint8_t parameters[4] = { (uint8_t) (address >> 8), (uint8_t) (address
			& 0xFF), regaddr, length };
	clrc663_flush_fifo();
	clrc663_write_fifo(parameters, 4);
	clrc663_write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_LOADREG);
}

void clrc663_cmd_load_protocol(uint8_t rx, uint8_t tx) {
	uint8_t parameters[2] = { rx, tx };
	clrc663_flush_fifo();
	clrc663_write_fifo(parameters, 2);
	clrc663_write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_LOADPROTOCOL);
}

void clrc663_cmd_transceive(const uint8_t *data, uint16_t len) {
	clrc663_write_fifo(data, len);
	clrc663_write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_TRANSCEIVE);
}

void clrc663_cmd_idle() {
	clrc663_write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_IDLE);
}

void clrc663_cmd_load_key_E2(uint8_t key_nr) {
	uint8_t parameters[1] = { key_nr };
	clrc663_flush_fifo();
	clrc663_write_fifo(parameters, 1);
	clrc663_write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_LOADKEYE2);
}

void clrc663_cmd_auth(uint8_t key_type, uint8_t block_address,
		const uint8_t *card_uid) {
	clrc663_cmd_idle();
	uint8_t parameters[6] = { key_type, block_address, card_uid[0], card_uid[1],
			card_uid[2], card_uid[3] };
	clrc663_flush_fifo();
	clrc663_write_fifo(parameters, 6);
	clrc663_write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_MFAUTHENT);
}

void clrc663_cmd_load_key(const uint8_t *key) {
	clrc663_cmd_idle();
	clrc663_flush_fifo();
	clrc663_write_fifo(key, 6);
	clrc663_write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_LOADKEY);
}

void clrc663_cmd_softreset() {
	clrc663_write_reg(CLRC663_REG_COMMAND, CLRC663_CMD_SOFTRESET);
}

// ---------------------------------------------------------------------------
// Utility functions.
// ---------------------------------------------------------------------------

void clrc663_flush_fifo() {
	clrc663_write_reg(CLRC663_REG_FIFOCONTROL, 1 << 4);
}

uint16_t clrc663_fifo_length() {
	return clrc663_read_reg(CLRC663_REG_FIFOLENGTH);
}

void clrc663_clear_irq0() {
	clrc663_write_reg(CLRC663_REG_IRQ0, (uint8_t) ~(1 << 7));
}
void clrc663_clear_irq1() {
	clrc663_write_reg(CLRC663_REG_IRQ1, (uint8_t) ~(1 << 7));
}
uint8_t clrc663_irq0() {
	return clrc663_read_reg(CLRC663_REG_IRQ0);
}
uint8_t clrc663_irq1() {
	return clrc663_read_reg(CLRC663_REG_IRQ1);
}

uint8_t clrc663_transfer_E2_page(uint8_t *dest, uint8_t page) {
	clrc663_cmd_read_E2(page * 64, 64);
	uint8_t res = clrc663_fifo_length();
	clrc663_read_fifo(dest, 64);
	return res;
}

// ---------------------------------------------------------------------------
// Timer functions
// ---------------------------------------------------------------------------
void clrc663_activate_timer(uint8_t timer, uint8_t active) {
	clrc663_write_reg(CLRC663_REG_TCONTROL,
			((active << timer) << 4) | (1 << timer));
}

void clrc663_timer_set_control(uint8_t timer, uint8_t value) {
	clrc663_write_reg(CLRC663_REG_T0CONTROL + (5 * timer), value);
}
void clrc663_timer_set_reload(uint8_t timer, uint16_t value) {
	clrc663_write_reg(CLRC663_REG_T0RELOADHI + (5 * timer), value >> 8);
	clrc663_write_reg(CLRC663_REG_T0RELOADLO + (5 * timer), 0xFF);
}
void clrc663_timer_set_value(uint8_t timer, uint16_t value) {
	clrc663_write_reg(CLRC663_REG_T0COUNTERVALHI + (5 * timer), value >> 8);
	clrc663_write_reg(CLRC663_REG_T0COUNTERVALLO + (5 * timer), 0xFF);
}
uint16_t clrc663_timer_get_value(uint8_t timer) {
	uint16_t res = clrc663_read_reg(CLRC663_REG_T0COUNTERVALHI + (5 * timer))
			<< 8;
	res += clrc663_read_reg(CLRC663_REG_T0COUNTERVALLO + (5 * timer));
	return res;
}

// ---------------------------------------------------------------------------
// ISO 14443A
// ---------------------------------------------------------------------------

uint16_t clrc663_iso14443a_REQA() {
	return clrc663_iso14443a_WUPA_REQA(CLRC663_ISO14443_CMD_REQA);
}
uint16_t clrc663_iso14443a_WUPA() {
	return clrc663_iso14443a_WUPA_REQA(CLRC663_ISO14443_CMD_WUPA);
}

uint16_t clrc663_iso14443a_WUPA_REQA(uint8_t instruction) {
	clrc663_cmd_idle();

	clrc663_flush_fifo();

	// Set register such that we sent 7 bits, set DataEn such that we can send
	// data.
	clrc663_write_reg(CLRC663_REG_TXDATANUM, 7 | CLRC663_TXDATANUM_DATAEN);

	// disable the CRC registers.
	clrc663_write_reg(CLRC663_REG_TXCRCPRESET,
			CLRC663_RECOM_14443A_CRC | CLRC663_CRC_OFF);
	clrc663_write_reg(CLRC663_REG_RXCRCCON,
			CLRC663_RECOM_14443A_CRC | CLRC663_CRC_OFF);

	clrc663_write_reg(CLRC663_REG_RXBITCTRL, 0);

	// prepare the request.
	uint8_t send_req[] = { instruction };

	// clear interrupts
	clrc663_clear_irq0();
	clrc663_clear_irq1();

	// enable the global IRQ for Rx done and Errors.
	clrc663_write_reg(CLRC663_REG_IRQ0EN,
			CLRC663_IRQ0EN_RX_IRQEN | CLRC663_IRQ0EN_ERR_IRQEN);
	clrc663_write_reg(CLRC663_REG_IRQ1EN, CLRC663_IRQ1EN_TIMER0_IRQEN); // only trigger on timer for irq1

	// configure a timeout timer.
	uint8_t timer_for_timeout = 0;

	// Set timer to 221 kHz clock, start at the end of Tx.
	clrc663_timer_set_control(timer_for_timeout,
			CLRC663_TCONTROL_CLK_211KHZ | CLRC663_TCONTROL_START_TX_END);
	// Frame waiting time: FWT = (256 x 16/fc) x 2 FWI
	// FWI defaults to four... so that would mean wait for a maximum of ~ 5ms

	clrc663_timer_set_reload(timer_for_timeout, 1000); // 1000 ticks of 5 usec is 5 ms.
	clrc663_timer_set_value(timer_for_timeout, 1000);

	// Go into send, then straight after in receive.
	clrc663_cmd_transceive(send_req, 1);
	// block until we are done
	uint8_t irq1_value = 0;
	while (!(irq1_value & (1 << timer_for_timeout))) {
		irq1_value = clrc663_irq1();
		if (irq1_value & CLRC663_IRQ1_GLOBAL_IRQ) {  // either ERR_IRQ or RX_IRQ
			break;  // stop polling irq1 and quit the timeout loop.
		}
	}
	clrc663_cmd_idle();

	// if no Rx IRQ, or if there's an error somehow, return 0
	uint8_t irq0 = clrc663_irq0();
	if ((!(irq0 & CLRC663_IRQ0_RX_IRQ)) || (irq0 & CLRC663_IRQ0_ERR_IRQ)) {
		return 0;
	}

	uint8_t rx_len = clrc663_fifo_length();
	uint16_t res;
	if (rx_len == 2) {  // ATQA should answer with 2 bytes.
		clrc663_read_fifo((uint8_t*) &res, rx_len);
		return res;
	}
	return 0;
}

uint8_t clrc663_iso14443a_select(uint8_t *uid) {
	clrc663_cmd_idle();
	clrc663_flush_fifo();

	// we do not need atqa.
	// Bitshift to get uid_size; 0b00: single, 0b01: double, 0b10: triple, 0b11 RFU
	// uint8_t uid_size = (atqa & (0b11 << 6)) >> 6;
	// uint8_t bit_frame_collision = atqa & 0b11111;

	// enable the global IRQ for Rx done and Errors.
	clrc663_write_reg(CLRC663_REG_IRQ0EN,
			CLRC663_IRQ0EN_RX_IRQEN | CLRC663_IRQ0EN_ERR_IRQEN);
	clrc663_write_reg(CLRC663_REG_IRQ1EN, CLRC663_IRQ1EN_TIMER0_IRQEN); // only trigger on timer for irq1

	// configure a timeout timer, use timer 0.
	uint8_t timer_for_timeout = 0;

	// Set timer to 221 kHz clock, start at the end of Tx.
	clrc663_timer_set_control(timer_for_timeout,
			CLRC663_TCONTROL_CLK_211KHZ | CLRC663_TCONTROL_START_TX_END);
	// Frame waiting time: FWT = (256 x 16/fc) x 2 FWI
	// FWI defaults to four... so that would mean wait for a maximum of ~ 5ms

	clrc663_timer_set_reload(timer_for_timeout, 1000); // 1000 ticks of 5 usec is 5 ms.
	clrc663_timer_set_value(timer_for_timeout, 1000);
	uint8_t cascade_level;
	for (cascade_level = 1; cascade_level <= 3; cascade_level++) {
		uint8_t cmd = 0;
		uint8_t known_bits = 0;  // known bits of the UID at this level so far.
		uint8_t send_req[7] = { 0 };  // used as Tx buffer.
		uint8_t *uid_this_level = &(send_req[2]);
		// pointer to the UID so far, by placing this pointer in the send_req
		// array we prevent copying the UID continuously.
		uint8_t message_length;

		switch (cascade_level) {
		case 1:
			cmd = CLRC663_ISO14443_CAS_LEVEL_1;
			break;
		case 2:
			cmd = CLRC663_ISO14443_CAS_LEVEL_2;
			break;
		case 3:
			cmd = CLRC663_ISO14443_CAS_LEVEL_3;
			break;
		}

		// disable CRC in anticipation of the anti collision protocol
		clrc663_write_reg(CLRC663_REG_TXCRCPRESET,
				CLRC663_RECOM_14443A_CRC | CLRC663_CRC_OFF);
		clrc663_write_reg(CLRC663_REG_RXCRCCON,
				CLRC663_RECOM_14443A_CRC | CLRC663_CRC_OFF);

		// max 32 loops of the collision loop.
		uint8_t collision_n;
		for (collision_n = 0; collision_n < 32; collision_n++) {

			// clear interrupts
			clrc663_clear_irq0();
			clrc663_clear_irq1();

			send_req[0] = cmd;
			send_req[1] = 0x20 + known_bits;
			// send_req[2..] are filled with the UID via the uid_this_level pointer.

			// Only transmit the last 'x' bits of the current byte we are discovering
			// First limit the txdatanum, such that it limits the correct number of bits.
			clrc663_write_reg(CLRC663_REG_TXDATANUM,
					(known_bits % 8) | CLRC663_TXDATANUM_DATAEN);

			// ValuesAfterColl: If cleared, every received bit after a collision is
			// replaced by a zero. This function is needed for ISO/IEC14443 anticollision (0<<7).
			// We want to shift the bits with RxAlign
			uint8_t rxalign = known_bits % 8;

			clrc663_write_reg(CLRC663_REG_RXBITCTRL, (0 << 7) | (rxalign << 4));

			// then sent the send_req to the hardware,
			// (known_bits / 8) + 1): The ceiled number of bytes by known bits.
			// +2 for cmd and NVB.
			if ((known_bits % 8) == 0) {
				message_length = ((known_bits / 8)) + 2;
			} else {
				message_length = ((known_bits / 8) + 1) + 2;
			}

			clrc663_cmd_transceive(send_req, message_length);

			// block until we are done
			uint8_t irq1_value = 0;
			while (!(irq1_value & (1 << timer_for_timeout))) {
				irq1_value = clrc663_irq1();
				// either ERR_IRQ or RX_IRQ or Timer
				if (irq1_value & CLRC663_IRQ1_GLOBAL_IRQ) {
					break;  // stop polling irq1 and quit the timeout loop.
				}
			}
			clrc663_cmd_idle();

			// next up, we have to check what happened.
			uint8_t irq0 = clrc663_irq0();
			uint8_t error = clrc663_read_reg(CLRC663_REG_ERROR);
			uint8_t coll = clrc663_read_reg(CLRC663_REG_RXCOLL);

			uint8_t collision_pos = 0;
			if (irq0 & CLRC663_IRQ0_ERR_IRQ) {  // some error occured.
				// Check what kind of error.
				// error = clrc663_read_reg(CLRC663_REG_ERROR);
				if (error & CLRC663_ERROR_COLLDET) {
					// A collision was detected...
					if (coll & (1 << 7)) {
						collision_pos = coll & (~(1 << 7));

						// This be a true collision... we have to select either the address
						// with 1 at this position or with zero
						// ISO spec says typically a 1 is added, that would mean:
						// uint8_t selection = 1;

						// However, it makes sense to allow some kind of user input for this, so we use the
						// current value of uid at this position, first index right byte, then shift such
						// that it is in the rightmost position, ten select the last bit only.
						// We cannot compensate for the addition of the cascade tag, so this really
						// only works for the first cascade level, since we only know whether we had
						// a cascade level at the end when the SAK was received.
						uint8_t choice_pos = known_bits + collision_pos;
						uint8_t selection = (uid[((choice_pos
								+ (cascade_level - 1) * 3) / 8)]
								>> ((choice_pos) % 8)) & 1;

						// We just OR this into the UID at the right position, later we
						// OR the UID up to this point into uid_this_level.
						uid_this_level[((choice_pos) / 8)] |= selection
								<< ((choice_pos) % 8);
						known_bits++;  // add the bit we just decided.

					} else {
						// Datasheet of clrc663:
						// bit 7 (CollPosValid) not set:
						// Otherwise no collision is detected or
						// the position of the collision is out of the range of bits CollPos.
						collision_pos = 0x20 - known_bits;
					}
				} else {
					// Can this ever occur?
					collision_pos = 0x20 - known_bits;
				}
			} else if (irq0 & CLRC663_IRQ0_RX_IRQ) {
				// we got data, and no collisions, that means all is well.
				collision_pos = 0x20 - known_bits;
			} else {
				// We have no error, nor received an RX. No response, no card?
				return 0;
			}

			// read the UID Cln so far from the buffer.
			uint8_t rx_len = clrc663_fifo_length();
			uint8_t buf[5];  // Size is maximum of 5 bytes, UID[0-3] and BCC.

			clrc663_read_fifo(buf, rx_len < 5 ? rx_len : 5);

			// move the buffer into the uid at this level, but OR the result such that
			// we do not lose the bit we just set if we have a collision.
			uint8_t rbx;
			for (rbx = 0; (rbx < rx_len); rbx++) {
				uid_this_level[(known_bits / 8) + rbx] |= buf[rbx];
			}
			known_bits += collision_pos;

			if ((known_bits >= 32)) {
				break;  // done with collision loop
			}
		}  // end collission loop

		// check if the BCC matches
		uint8_t bcc_val = uid_this_level[4]; // always at position 4, either with CT UID[0-2] or UID[0-3] in front.
		uint8_t bcc_calc = uid_this_level[0] ^ uid_this_level[1]
				^ uid_this_level[2] ^ uid_this_level[3];
		if (bcc_val != bcc_calc) {
			return 0;
		}

		// clear interrupts
		clrc663_clear_irq0();
		clrc663_clear_irq1();

		send_req[0] = cmd;
		send_req[1] = 0x70;
		// send_req[2,3,4,5] // contain the CT, UID[0-2] or UID[0-3]
		send_req[6] = bcc_calc;
		message_length = 7;

		// Ok, almost done now, we reenable the CRC's
		clrc663_write_reg(CLRC663_REG_TXCRCPRESET,
				CLRC663_RECOM_14443A_CRC | CLRC663_CRC_ON);
		clrc663_write_reg(CLRC663_REG_RXCRCCON,
				CLRC663_RECOM_14443A_CRC | CLRC663_CRC_ON);

		// reset the Tx and Rx registers (disable alignment, transmit full bytes)
		clrc663_write_reg(CLRC663_REG_TXDATANUM,
				(known_bits % 8) | CLRC663_TXDATANUM_DATAEN);
		uint8_t rxalign = 0;
		clrc663_write_reg(CLRC663_REG_RXBITCTRL, (0 << 7) | (rxalign << 4));

		// actually send it!
		clrc663_cmd_transceive(send_req, message_length);

		// Block until we are done...
		uint8_t irq1_value = 0;
		while (!(irq1_value & (1 << timer_for_timeout))) {
			irq1_value = clrc663_irq1();
			if (irq1_value & CLRC663_IRQ1_GLOBAL_IRQ) { // either ERR_IRQ or RX_IRQ
				break;  // stop polling irq1 and quit the timeout loop.
			}
		}
		clrc663_cmd_idle();

		// Check the source of exiting the loop.
		uint8_t irq0_value = clrc663_irq0();
		if (irq0_value & CLRC663_IRQ0_ERR_IRQ) {
			// Check what kind of error.
			uint8_t error = clrc663_read_reg(CLRC663_REG_ERROR);
			if (error & CLRC663_ERROR_COLLDET) {
				// a collision was detected with NVB=0x70, should never happen.
				return 0;
			}
		}

		// Read the sak answer from the fifo.
		uint8_t sak_len = clrc663_fifo_length();
		if (sak_len != 1) {
			return 0;
		}
		uint8_t sak_value;
		clrc663_read_fifo(&sak_value, sak_len);

		if (sak_value & (1 << 2)) {
			// UID not yet complete, continue with next cascade.
			// This also means the 0'th byte of the UID in this level was CT, so we
			// have to shift all bytes when moving to uid from uid_this_level.
			uint8_t UIDn;
			for (UIDn = 0; UIDn < 3; UIDn++) {
				// uid_this_level[UIDn] = uid_this_level[UIDn + 1];
				uid[(cascade_level - 1) * 3 + UIDn] = uid_this_level[UIDn + 1];
			}
		} else {
			// Done according so SAK!
			// Add the bytes at this level to the UID.
			uint8_t UIDn;
			for (UIDn = 0; UIDn < 4; UIDn++) {
				uid[(cascade_level - 1) * 3 + UIDn] = uid_this_level[UIDn];
			}

			// Finally, return the length of the UID that's now at the uid pointer.
			return cascade_level * 3 + 1;
		}

	}  // cascade loop
	return 0;  // getting an UID failed.
}

// ---------------------------------------------------------------------------
// NTAG216
// ---------------------------------------------------------------------------
uint8_t NTAG_GetVersion(void) {
	unsigned char Response[10];
	clrc663_flush_fifo();

	clrc663_write_reg(CLRC663_REG_TXCRCPRESET,
			CLRC663_RECOM_14443A_CRC | CLRC663_CRC_ON);
	clrc663_write_reg(CLRC663_REG_RXCRCCON,
			CLRC663_RECOM_14443A_CRC | CLRC663_CRC_ON);

	uint8_t send_req[1] = { 0x60 };

	// configure a timeout timer.
	uint8_t timer_for_timeout = 0;  // should match the enabled interupt.

	// enable the global IRQ for idle, errors and timer.
	clrc663_write_reg(CLRC663_REG_IRQ0EN,
			CLRC663_IRQ0EN_IDLE_IRQEN | CLRC663_IRQ0EN_ERR_IRQEN);
	clrc663_write_reg(CLRC663_REG_IRQ1EN, CLRC663_IRQ1EN_TIMER0_IRQEN);

	// Set timer to 221 kHz clock, start at the end of Tx.
	clrc663_timer_set_control(timer_for_timeout,
			CLRC663_TCONTROL_CLK_211KHZ | CLRC663_TCONTROL_START_TX_END);
	// Frame waiting time: FWT = (256 x 16/fc) x 2 FWI
	// FWI defaults to four... so that would mean wait for a maximum of ~ 5ms
	clrc663_timer_set_reload(timer_for_timeout, 2000); // 2000 ticks of 5 usec is 10 ms.
	clrc663_timer_set_value(timer_for_timeout, 2000);

	uint8_t irq1_value = 0;
	uint8_t irq0_value = 0;

	clrc663_clear_irq0();  // clear irq0
	clrc663_clear_irq1();  // clear irq1

	// Go into send, then straight after in receive.
	clrc663_cmd_transceive(send_req, 1);

	// block until we are done
	while (!(irq1_value & (1 << timer_for_timeout))) {
		irq1_value = clrc663_irq1();
		if (irq1_value & CLRC663_IRQ1_GLOBAL_IRQ) {
			break;  // stop polling irq1 and quit the timeout loop.
		}
	}
	clrc663_cmd_idle();

	if (irq1_value & (1 << timer_for_timeout)) {
		// this indicates a timeout
		return 0;
	}

	irq0_value = clrc663_irq0();
	if (irq0_value & CLRC663_IRQ0_ERR_IRQ) {
		// some error
		return 0;
	}

	uint8_t buffer_length = clrc663_fifo_length();
	uint8_t rx_len = (buffer_length <= 10) ? buffer_length : 10;
	clrc663_read_fifo(Response, rx_len);
	if (rx_len == 8 && Response[1] ==0x04 && Response[2] ==0x04 && Response[6] == 0x13)
	{
		//Byte 1 indicates vendor ID -> 0x04 -> NXP
		//Byte 2 indicates tag type	-> 0.04 -> NTAG
		//Byte 6 indicates the Storage size -> 0x11 -> NTAG21,5 0x13-> NTAG216

		return (uint8_t)Response[6]; //Was 1. I return the type, so the memory can be filled correctly.
	}
	return 0;
}

uint8_t NTAG_ReadBlock(uint8_t block_address, uint8_t *dest) {
	clrc663_flush_fifo();

	clrc663_write_reg(CLRC663_REG_TXCRCPRESET,
			CLRC663_RECOM_14443A_CRC | CLRC663_CRC_ON);
	clrc663_write_reg(CLRC663_REG_RXCRCCON,
			CLRC663_RECOM_14443A_CRC | CLRC663_CRC_ON);

	uint8_t send_req[2] = { CLRC663_MF_CMD_READ, block_address };

	// configure a timeout timer.
	uint8_t timer_for_timeout = 0;  // should match the enabled interupt.

	// enable the global IRQ for idle, errors and timer.
	clrc663_write_reg(CLRC663_REG_IRQ0EN,
			CLRC663_IRQ0EN_IDLE_IRQEN | CLRC663_IRQ0EN_ERR_IRQEN);
	clrc663_write_reg(CLRC663_REG_IRQ1EN, CLRC663_IRQ1EN_TIMER0_IRQEN);

	// Set timer to 221 kHz clock, start at the end of Tx.
	clrc663_timer_set_control(timer_for_timeout,
			CLRC663_TCONTROL_CLK_211KHZ | CLRC663_TCONTROL_START_TX_END);
	// Frame waiting time: FWT = (256 x 16/fc) x 2 FWI
	// FWI defaults to four... so that would mean wait for a maximum of ~ 5ms
	clrc663_timer_set_reload(timer_for_timeout, 2000); // 2000 ticks of 5 usec is 10 ms.
	clrc663_timer_set_value(timer_for_timeout, 2000);

	uint8_t irq1_value = 0;
	uint8_t irq0_value = 0;

	clrc663_clear_irq0();  // clear irq0
	clrc663_clear_irq1();  // clear irq1

	// Go into send, then straight after in receive.
	clrc663_cmd_transceive(send_req, 2);

	// block until we are done
	while (!(irq1_value & (1 << timer_for_timeout))) {
		irq1_value = clrc663_irq1();
		if (irq1_value & CLRC663_IRQ1_GLOBAL_IRQ) {
			break;  // stop polling irq1 and quit the timeout loop.
		}
	}
	clrc663_cmd_idle();

	if (irq1_value & (1 << timer_for_timeout)) {
		// this indicates a timeout
		return 0;
	}

	irq0_value = clrc663_irq0();
	if (irq0_value & CLRC663_IRQ0_ERR_IRQ) {
		// some error
		return 0;
	}

	// all seems to be well...
	uint8_t buffer_length = clrc663_fifo_length();
	uint8_t rx_len = (buffer_length <= 16) ? buffer_length : 16;
	clrc663_read_fifo(dest, rx_len);
	return rx_len;
}
