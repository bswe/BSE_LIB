/**** RFM12B library for Atmel ATmega328 *******
 * 
 * This software is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 *
 * @author Bollenbacher Software Engineering
 * Created: 8/19/13
 */

#include "spi.h"

#define SLAVE_SELECT PIN_INFO(PORT_B, PIN_2, OUTPUT_MODE, ACTIVE_LOW)
#define MOSI         PIN_INFO(PORT_B, PIN_3, OUTPUT_MODE, ACTIVE_HIGH)
#define MISO         PIN_INFO(PORT_B, PIN_4, INPUT_MODE, ACTIVE_HIGH)
#define CLK          PIN_INFO(PORT_B, PIN_5, OUTPUT_MODE, ACTIVE_HIGH)

Spi::Spi () {	
	SetPinInActive (SLAVE_SELECT);
	SetPinDirection (SLAVE_SELECT);
	SetPinDirection (MOSI);
	SetPinDirection (MISO);
	SetPinDirection (CLK);
	SPCR = _BV(SPE) | _BV(MSTR);
	// use MCU clk/2 (2 x 1/4th) for writes and fast status reads and
	// clk/8 (2 x 1/16th) for slow fifo reads, see RecvWordSlow)
	SPSR |= _BV(SPI2X);	
    }
	

uint8_t Spi::SendByte (uint8_t DataByte) {
    SPDR = DataByte;
    while (!(SPSR & _BV(SPIF)));
    return SPDR;	
	}


void Spi::SendWord (uint16_t DataWord) {
    // writing can take place at 8 MHz
    SetPinActive(SLAVE_SELECT);
    SendByte (DataWord >> 8);
    SendByte (DataWord);
	SetPinInActive (SLAVE_SELECT);
	}


uint16_t Spi::GetWordSlow (uint16_t DataWord) {
	uint16_t reply;

	// reading the fifo can take place at only less than 2.5 MHz
	SPCR |= _BV(SPR0);       // set the SPR0 bit to divide clk by 16
    SetPinActive(SLAVE_SELECT);
	reply = SendByte (DataWord >> 8) << 8;
	reply |= SendByte (DataWord);
	SetPinInActive (SLAVE_SELECT);
	SPCR &= ~_BV(SPR0);      // clear the SPR0 bit to divide clk by 4
	return reply;
}


uint16_t Spi::GetWord (uint16_t DataWord) {
	uint16_t reply;

    // reading status word at full 8 MHz
    SetPinActive(SLAVE_SELECT);
    reply = SendByte (DataWord >> 8) << 8;
    reply |= SendByte (DataWord);
	SetPinInActive (SLAVE_SELECT);
    return reply;
    }
