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

#ifndef RFM12B_H_
#define RFM12B_H_

#include "spi.h"

#define MAX_PACKET_SIZE 40

class Rfm12b {
    public:
	    Rfm12b ();
		void Initialize ();
		uint8_t Send (uint8_t* Data, uint8_t Length);
		uint8_t Recv (uint8_t* Bfr);
		void DisplayStatus ();
    };
 
#endif /* RFM12B_H_ */
