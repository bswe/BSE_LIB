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

#ifndef WIRELESS_PACKET_H_
#define WIRELESS_PACKET_H_

#include "rfm12b.h"

#define MAX_MAC_PACKET_SIZE  MAX_PHY_PACKET_PAYLOAD_SIZE
#define MAX_MAC_HEADER_SIZE  5
#define MAX_MAC_PAYLOAD_SIZE MAX_MAC_PACKET_SIZE - MAX_MAC_HEADER_SIZE

#define PACKET_LENGTH_INDEX      0
#define PACKET_TYPE_INDEX        1
#define PACKET_DESTINATION_INDEX 2
#define PACKET_SOURCE_INDEX      3
#define PACKET_SEQUENCE_INDEX    4
#define PACKET_PAYLOAD_INDEX     5


#define MAC_REQUEST_PACKET_TYPE  0
#define MAC_RESPONSE_PACKET_TYPE 1

class Mac {
    uint8_t SequenceNumber;
    
    public:
    Mac ();
    uint8_t MakePacket (uint8_t* Packet, uint8_t Type, uint8_t Destination, uint8_t Source, uint8_t* Payload, uint8_t Length);
};
#endif /* WIRELESS_PACKET_H_ */
