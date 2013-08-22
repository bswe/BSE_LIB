/**** Serial USART library for Atmel ATmega328 *******
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

#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdint.h>

#define UI8_P(char_ptr) ((uint8_t*) (char_ptr))

#ifdef __cplusplus
extern "C" {
	#endif

	void SerialInit (uint16_t Baudrate);
	void SerialSend (uint8_t Char);
	void SendString (uint8_t* str);
	void SendInt (uint16_t d);
	void SendStringAndInt (uint8_t* StartStr, uint16_t d, uint8_t* EndStr);
	uint8_t ReadChar (void);

#ifdef __cplusplus
}
#endif

#endif /* SERIAL_H_ */