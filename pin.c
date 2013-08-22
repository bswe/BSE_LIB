/**** Pin library for Atmel ATmega328 *******
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

#include <avr/io.h>
#include "pin.h"

void SetPinDirection (uint16_t PinInfo) {
    if ((PinInfo & DIRECTION_BIT) == DIRECTION_BIT)
        // pin is an output
		_SFR_IO8((PinInfo>>PORT_OFFSET)-1) |= _BV(PinInfo & PIN_NUMBER_MASK);
	else
        // pin is an input
		_SFR_IO8((PinInfo>>PORT_OFFSET)-1) &= ~_BV(PinInfo & PIN_NUMBER_MASK);
    }
    
void SetPinActive (uint16_t PinInfo) {
    if ((PinInfo & LEVEL_BIT) == LEVEL_BIT)
        // pin is active high, so turn it on
        _SFR_IO8(PinInfo>>PORT_OFFSET) |= _BV(PinInfo & PIN_NUMBER_MASK);
    else
        // pin is active low, so turn it off
        _SFR_IO8(PinInfo>>PORT_OFFSET) &= ~_BV(PinInfo & PIN_NUMBER_MASK);
}

void SetPinInActive (uint16_t PinInfo) {
    if ((PinInfo & LEVEL_BIT) == LEVEL_BIT)
        // pin is active high, so turn it off
        _SFR_IO8(PinInfo>>PORT_OFFSET) &= ~_BV(PinInfo & PIN_NUMBER_MASK);
    else
        // pin is active low, so turn it on
        _SFR_IO8(PinInfo>>PORT_OFFSET) |= _BV(PinInfo & PIN_NUMBER_MASK);
}

void InitPinInActive (uint16_t PinInfo) {
    SetPinDirection (PinInfo);
    SetPinInActive (PinInfo);
}

void InitPinActive (uint16_t PinInfo) {
    SetPinDirection (PinInfo);
    SetPinActive (PinInfo);
}

uint8_t GetPinState (uint16_t PinInfo) {
	return (_SFR_IO8((PinInfo>>PORT_OFFSET)-2) & _BV(PinInfo & PIN_NUMBER_MASK)) ? 1 : 0;
	}