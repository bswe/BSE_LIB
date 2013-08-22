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

#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>
#include "pin.h"

class Spi {

	uint8_t SendByte (uint8_t DataByte);

	public:
		Spi ();
		void SendWord (uint16_t DataWord);
		uint16_t GetWord (uint16_t DataWord);
		uint16_t GetWordSlow (uint16_t DataWord);
	};
	
#endif /* SPI_H_ */
