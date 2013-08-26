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

#include <string.h>
#include "wireless_packet.h"

Mac::Mac () {
    NextSequenceNumber = 0;
    }


uint8_t Mac::MakeRequestPacket (uint8_t* Packet, uint8_t Destination, uint8_t Source, uint8_t* Payload, uint8_t Length) {
    Packet[PACKET_LENGTH_INDEX] = Length + MAX_MAC_HEADER_SIZE - 1;  // don't include Length byte in packet length
    Packet[PACKET_TYPE_INDEX] = MAC_REQUEST_PACKET_TYPE;
    Packet[PACKET_DESTINATION_INDEX] = Destination;
    Packet[PACKET_SOURCE_INDEX] = Source;
    Packet[PACKET_SEQUENCE_INDEX] = NextSequenceNumber;
    memcpy ((void*) &Packet[PACKET_PAYLOAD_INDEX], Payload, Length);
    return NextSequenceNumber++;
    }
    

void Mac::MakeResponsePacket (uint8_t* Packet, uint8_t Destination, uint8_t Source, uint8_t SequenceNumber, uint8_t* Payload, uint8_t Length) {
    Packet[PACKET_LENGTH_INDEX] = Length + MAX_MAC_HEADER_SIZE - 1;  // don't include Length byte in packet length
    Packet[PACKET_TYPE_INDEX] = MAC_RESPONSE_PACKET_TYPE;
    Packet[PACKET_DESTINATION_INDEX] = Destination;
    Packet[PACKET_SOURCE_INDEX] = Source;
    Packet[PACKET_SEQUENCE_INDEX] = SequenceNumber;
    memcpy ((void*) &Packet[PACKET_PAYLOAD_INDEX], Payload, Length);
    }

