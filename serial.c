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

#include <avr/io.h>
#include <stdio.h>			
#include "serial.h"

#define FOSC 16000000 // Clock Speed

void SerialInit (uint16_t Baudrate) {
	long int ubrr = (((((FOSC * 10) / (16L * Baudrate)) + 5) / 10) - 1);
	
	/*Set baud rate */
	UBRR0H = (uint8_t) (ubrr>>8);
	UBRR0L = (uint8_t) ubrr;
	//Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0) | (3<<UCSZ00);
	}


void SendChar (uint8_t Char) {
	/* Wait for empty transmit buffer */
	while (!( UCSR0A & (1<<UDRE0)));
		/* Put char into buffer, sends the char */
	UDR0 = Char;
	}


void SendString (uint8_t* str) {
	while (*str) 
		SendChar (*str++);
	}


void SendUInt (uint16_t d) {
    char str[10];
    
    sprintf (str,"%03u",d);
    SendString ((uint8_t*)  str);
}


void SendInt (int16_t d) {
    char str[10];
    
    sprintf (str,"%03d",d);
    SendString ((uint8_t*)  str);
}


void SendStringAndUInt (uint8_t* StartStr, uint16_t d, uint8_t* EndStr) {
    SendString (StartStr);
    SendUInt (d);
    SendString (EndStr);
}

void SendStringAndInt (uint8_t* StartStr, int16_t d, uint8_t* EndStr) {
    SendString (StartStr);
    SendInt (d);
    SendString (EndStr);
}

uint8_t ReadChar (void) {
	/* check for data to be received */
	if (UCSR0A & (1<<RXC0))
		/* Get and return received data from buffer */
		return UDR0;
	else
		return 0;
	}