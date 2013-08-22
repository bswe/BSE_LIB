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
 *
 * The purpose of this library is to allow coding of all pin manipulations using 
 * constants and macros so that the compiler can optimize away everything accept 
 * the actual writes to the registers. The library supports the conceptualization 
 * of pin functionality by defining generalized pin attributes that once defined 
 * allow the library's functions to perform standardized bit manipulations.
 * 
 * This library uses macros and constants to define 4 pin attributes that
 * are then passed into generalized functions that perform specific bit
 * manipulations based on the pin's attributes.  The 4 attributes are:
 * 
 *     PortName     The ATmega328 address of the port's data register for the pin
 *     PinNumber    The number (0-7) of the pin
 *     Direction    The direction (input/output) of the pin
 *     Level        The active level (high/low) of the pin
 *     
 * These attributes are defined as constants in this file and are combined into
 * a single uint16_t constant by the PIN_INFO() macro.  As an example:
 * 
 *     #define IRQ PIN_INFO(PORT_D, PIN_2, INPUT_MODE, ACTIVE_LOW)
 *
 * would define the INT0 pin as an external interrupt that's active when low.
 * 
 *     #define RED_LED_PIN PIN_INFO(PORT_C, PIN_1, OUTPUT_MODE, ACTIVE_LOW)    
 *     
 * would define the red LED on a Blink Plug plugged into port2 of a Jeenode.
 *
 * To manipulate a pin the pin's constant is passed into one of the libraries
 * functions.  As an example, to initialize the INT0 pin defined above:
 * 
 *     	InitPinInActive (IRQ);  // input, active low, enable pull-up
 */

#ifndef PIN_H_
#define PIN_H_

#define PIN_0 0
#define PIN_1 1
#define PIN_2 2 
#define PIN_3 3
#define PIN_4 4
#define PIN_5 5
#define PIN_6 6
#define PIN_7 7

#define PORT_B 0x05
#define PORT_C 0x08
#define PORT_D 0x0B

#define INPUT_MODE  0
#define OUTPUT_MODE 1

#define ACTIVE_LOW  0
#define ACTIVE_HIGH 1

#define PORT_OFFSET 8
#define DIRECTION_OFFSET 4
#define DIRECTION_BIT (1 << DIRECTION_OFFSET)
#define LEVEL_OFFSET 3
#define LEVEL_BIT (1 << LEVEL_OFFSET)
#define PIN_NUMBER_MASK 7

#define PIN_INFO(PortName, PinNumber, Direction, Level) \
    (((PortName) << PORT_OFFSET) | ((Direction) << DIRECTION_OFFSET) | ((Level) << LEVEL_OFFSET) | (PinNumber))

#ifdef __cplusplus
	extern "C" {
#endif

    void SetPinDirection (uint16_t PinInfo);
    void SetPinActive (uint16_t PinInfo);
    void SetPinInActive (uint16_t PinInfo);
    void InitPinActive (uint16_t PinInfo);
    void InitPinInActive (uint16_t PinInfo);
	uint8_t GetPinState (uint16_t PinInfo);

#ifdef __cplusplus
	}
#endif

#endif /* PIN_H_ */
