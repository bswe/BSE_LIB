﻿/**** RFM12B library for Atmel ATmega328 *******
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

#include "rfm12b.h"
#include "rfm12b_registers.h"
#include "serial.h"
#include <string.h>
#include <util/crc16.h>
#include <avr/interrupt.h>

// the interrupt mask register
#define RFM12_INT_MSK EIMSK

// the interrupt bit in the mask register
#define RFM12_INT_BIT (INT0)

// the interrupt flag register
#define RFM12_INT_FLAG EIFR

// the interrupt bit in the flag register
#define RFM12_FLAG_BIT (INTF0)

// the interrupt control register
#define RFM12_INT_CTRL EICRA

// the interrupt bits in the control register
#define RFM12_CTRL_BIT_0 (ISC00)
#define RFM12_CTRL_BIT_1 (ISC01)

// the interrupt vector
#define RFM12_INT_VECT (INT0_vect)

#define RFM12_INT_ON() RFM12_INT_MSK |= (1<<RFM12_INT_BIT)
#define RFM12_INT_OFF() RFM12_INT_MSK &= ~(1<<RFM12_INT_BIT)

#define BAND 3
#define PREAMBLE 0xAA
#define LENGTH_BYTE 4

#define FALSE 0
#define TRUE 1

#define RF_IDLE_MODE    (RFM12B_PWRMGT_CMD | RFM12B_PWRMGT_DC_PIN)   // 8201h
#define RF_RECEIVER_ON  (RFM12B_PWRMGT_CMD | RFM12B_PWRMGT_ER_PIN | RFM12B_PWRMGT_DC_PIN)   // 8281h
#define RF_XMITTER_ON   (RFM12B_PWRMGT_CMD | RFM12B_PWRMGT_ET_PIN | RFM12B_PWRMGT_DC_PIN)   // 8221h
#define FIFO_FILL_STOP  (RFM12B_FIFORESET_CMD | RFM12B_FIFORESET_LEVEL_8 | RFM12B_FIFORESET_DR_PIN)   // CA81h
#define FIFO_FILL_START (FIFO_FILL_STOP | RFM12B_FIFORESET_FF_PIN)   // CA83h
#define GOOD_BIT_SYNC   (RFM12B_STATUS_RSSI_PIN | RFM12B_STATUS_DQD_PIN | RFM12B_STATUS_CRL_PIN)   // 01C0h
#define CONFIGURATION   (RFM12B_CFG_CMD | RFM12B_CFG_EL_PIN | RFM12B_CFG_EF_PIN | RFM12B_CFG_XTAL_12PF)   // 80C7h
#define DATARATE        RFM12B_DATARATE_CALC_HIGH(49200.0)
#define RX_CONFIG       (RFM12B_RXCTRL_CMD | RFM12B_RXCTRL_P16_PIN | RFM12B_RXCTRL_VDI_FAST | RFM12B_RXCTRL_BW_134 | \
                         RFM12B_RXCTRL_LNA_0 | RFM12B_RXCTRL_RSSI_91)   // 94A2h
#define DATA_FILTER     (RFM12B_DATAFILTER_CMD | RFM12B_DATAFILTER_AL_PIN | 4)   // C2ACh
#define FIFO_CONFIG     (RFM12B_FIFORESET_CMD | RFM12B_FIFORESET_LEVEL_8 | RFM12B_FIFORESET_FF_PIN | RFM12B_FIFORESET_DR_PIN)   // CA83h
#define AFC_CONFIG      (RFM12B_AFC_CMD | RFM12B_AFC_AUTO_VDI | RFM12B_AFC_OE_PIN | RFM12B_AFC_EN_PIN)   // C483h
#define TX_CONFIG       (RFM12B_TXCONF_CMD | RFM12B_TXCONF_FS_CALC(90000))   // 9850h 
#define PLL_CONFIG      (RFM12B_PLL_CMD | 0x40 | RFM12B_PLL_DDIT_PIN | RFM12B_PLL_BW0_PIN)   // CC77h    

#define IRQ PIN_INFO(PORT_D, PIN_2, INPUT_MODE, ACTIVE_LOW)

Spi Rfm12bSpi;
volatile uint16_t InterruptStatus;
volatile uint8_t OutputData[MAX_PACKET_SIZE+4] = "\xAA\xAA\x2D\xD4";
volatile uint8_t InputData[MAX_PACKET_SIZE];
volatile uint8_t OutputIndex, OutputLength;
volatile int8_t InputLength;
volatile uint16_t Crc;

//#define DEBUG_INTERRUPT
#ifdef DEBUG_INTERRUPT
	#include <stdio.h>
	
	#define DEBUG_INIT memset ((void*) Rfm12bStatus, 0xFF, sizeof(Rfm12bStatus));
	#define DEBUG_STATUS SaveStatus();
	#define DEBUG_CRC if (InterruptCount <= STATUS_SIZE) \
					      Rfm12bStatus[InterruptCount-1].BadCrc = Crc;
	#define DEBUG_RX_DATA if (InterruptCount <= STATUS_SIZE) \
						      Rfm12bStatus[InterruptCount-1].InputData = DataByte;

	#define STATUS_SIZE 20

	typedef struct Status {
		uint16_t InterruptStatus, BadCrc;
		uint8_t OutputLength, OutputIndex, OutputData, InputLength, InputData;
	} Status;

	volatile Status Rfm12bStatus[STATUS_SIZE];
	volatile uint32_t InterruptCount = 0;

	extern uint8_t StrBfr[50];

	void Rfm12b::DisplayStatus () {
		uint8_t i = 0;

		cli();
		sprintf ((char*) StrBfr, "C=%ld {\r\n", InterruptCount);
			SendString (StrBfr);
			while ((i < STATUS_SIZE) && (Rfm12bStatus[i].OutputLength != 0xFF)) {
				sprintf ((char*) StrBfr, "S=%0X,   ", Rfm12bStatus[i].InterruptStatus);
				SendString (StrBfr);
				SendStringAndInt (UI8_P("I#="), i, (uint8_t*) ",   ");
				SendStringAndInt (UI8_P("OL="), Rfm12bStatus[i].OutputLength, UI8_P(",   "));
				SendStringAndInt (UI8_P("OI="), Rfm12bStatus[i].OutputIndex, UI8_P(",   "));
				SendStringAndInt (UI8_P("OB="), Rfm12bStatus[i].OutputData, UI8_P(",   "));
				SendStringAndInt (UI8_P("IB="), Rfm12bStatus[i].InputData, UI8_P(",   "));
				SendStringAndInt (UI8_P("IL="), Rfm12bStatus[i].InputLength, UI8_P(",   "));
				SendStringAndInt (UI8_P("BC="), Rfm12bStatus[i].BadCrc, UI8_P("\r\n"));
				i++;
			}
		SendString(UI8_P("}\r\n"));
		sei();
	}


	void SaveStatus () {
		if (InterruptCount < STATUS_SIZE) {
			Rfm12bStatus[InterruptCount].InterruptStatus = InterruptStatus;
			Rfm12bStatus[InterruptCount].InputLength = InputLength;
			Rfm12bStatus[InterruptCount].InputData = 0;
			Rfm12bStatus[InterruptCount].OutputLength = OutputLength;
			Rfm12bStatus[InterruptCount].OutputIndex = OutputIndex;
			Rfm12bStatus[InterruptCount].OutputData = OutputData[OutputIndex];
			Rfm12bStatus[InterruptCount].BadCrc = 0;
		}
		InterruptCount++;
	}
#else
	#define DEBUG_INIT 
	#define DEBUG_STATUS
	#define DEBUG_RX_DATA
	#define DEBUG_CRC
#endif


Rfm12b::Rfm12b () {
	// initialize pins for HW SPI interface
	//InitPin (&SlaveSelect, PORT_B, PIN_2, OUTPUT_MODE, LOW);
	//InitPin (&Mosi, PORT_B, PIN_3, OUTPUT_MODE, HIGH);
	//InitPin (&Miso, PORT_B, PIN_4, INPUT_MODE, HIGH);
	//InitPin (&Clk, PORT_B, PIN_5, OUTPUT_MODE, HIGH);
	// initialize HW SPI interface
	//Rfm12bSpi.Initialize (SLAVE_SELECT, MOSI, MISO, CLK);
	
	// initialize pin for RFM12B interrupt
	//InitPin (&Irq, PORT_D, PIN_2, INPUT_MODE, LOW);
	InitPinInActive (IRQ);  // input, active low, enable pull-up
    }


void Rfm12b::Initialize () {	
	Rfm12bSpi.SendWord (RFM12B_STATUS_CMD);              // initial SPI transfer added to avoid power-up problem?
	Rfm12bSpi.SendWord (RF_IDLE_MODE);                   // disable clk pin
	Rfm12bSpi.SendWord (RFM12B_TX_CMD);                  // in case we're still in OOK mode
	Rfm12bSpi.SendWord (CONFIGURATION | (BAND << 4));    // EL (enable TX), EF (enable RX FIFO), 12.0pF, 915 MHz
	Rfm12bSpi.SendWord (RFM12B_DATARATE_CMD | DATARATE); // approx 49.2 Kbps, i.e. 10000/29/(1+6) Kbps
	Rfm12bSpi.SendWord (RX_CONFIG);                      // VDI, FAST, 134kHz, 0dBm, -91dBm
	Rfm12bSpi.SendWord (DATA_FILTER);                    // AL, !ml, DIG, DQD=4
	Rfm12bSpi.SendWord (FIFO_CONFIG);                    // FIFO8, 2-SYNC, !ff, DR
	Rfm12bSpi.SendWord (AFC_CONFIG);                     // @PWR, NO RSTRIC, !st, !fi, OE, EN
	Rfm12bSpi.SendWord (TX_CONFIG);                      // !mp, 90kHz, MAX OUT
	Rfm12bSpi.SendWord (PLL_CONFIG);                     // force to POR for A1 version per manual (CC77)
	Rfm12bSpi.SendWord (RFM12B_WAKEUP_CMD);              // Wake-Up Timer not used
	Rfm12bSpi.SendWord (RFM12B_DUTYCYCLE_CMD);           // Low Duty-Cycle not used

	//memset ((void*) InputData, 0, 10);
	OutputIndex = OutputLength = InputLength = 0;
	Crc = ~0;
	DEBUG_INIT;
	
	RFM12_INT_ON();
	sei();
	// start receiving
	Rfm12bSpi.SendWord (RF_RECEIVER_ON);
    }


uint8_t Rfm12b::Send (uint8_t* Data, uint8_t Length) {
	uint8_t i;
	
	RFM12_INT_OFF();
	if ((InputLength > 0) ||
        (Rfm12bSpi.GetWord (RFM12B_STATUS_CMD) & RFM12B_STATUS_RSSI_PIN) != 0) {
        // either already receiving something or RFM12B detects a 
        // signal, so don't send now
	    RFM12_INT_ON();
	    return 0;
        }        
	Rfm12bSpi.SendWord (RF_IDLE_MODE);      // switch off receiver
	OutputData[LENGTH_BYTE] = Length + 2;   // include 2 crc bytes in length
    memcpy ((void*) &OutputData[LENGTH_BYTE+1], Data, Length);
	OutputLength = Length + LENGTH_BYTE + 1;    // include 4 preamble bytes + 1 Length byte
    // calculate crc and put it into output buffer
	Crc = ~0;
	for (i=0; i <= Length; i++)
		Crc = _crc16_update(Crc, OutputData[i+4]);
	OutputData[OutputLength++] = Crc & 0xFF;
	OutputData[OutputLength++] = Crc >> 8;
    // add trailer byte to end of packet to ensure last byte of data is transmitted completely
	OutputData[OutputLength] = PREAMBLE;
    // pre-load preamble byte into transmit buffer to get things going quickly
	Rfm12bSpi.SendWord (RFM12B_TX_CMD + PREAMBLE);
    // turn on transmitter
    Rfm12bSpi.SendWord (RF_XMITTER_ON); // bytes will be fed via interrupts
	RFM12_INT_ON();
    return Length;
	}


uint8_t Rfm12b::Recv (uint8_t* Bfr) {
	uint8_t Length;
	
	if (InputLength < 0) {
		// invert the length back to positive and drop the length byte from the overall length
		Length = (InputLength * -1) - 1;    
		// copy from index 1 to drop the length byte from the data
		memcpy ((void*) Bfr, (void*) &InputData[1], Length);
		InputLength = 0;
		return Length;
		}
	return 0;
	}


void ResetFifo () {
	// reset the receiver fifo
	Rfm12bSpi.SendWord (FIFO_FILL_STOP);
	Rfm12bSpi.SendWord (FIFO_FILL_START);
	Crc = ~0;
	}


ISR(RFM12_INT_VECT) {
    DEBUG_STATUS;
	
	// get the RFM12B status, this will help reset the interrupt
	// and also help in determining if the data is any good
	InterruptStatus = Rfm12bSpi.GetWord (RFM12B_STATUS_CMD);
	
    if (0 == OutputLength) {
		// nothing in transmit buffer, so must be in receive mode
		// read FIFO to get the data and reset the interrupt
	    unsigned char DataByte = Rfm12bSpi.GetWordSlow (RFM12B_READ_CMD);
		DEBUG_RX_DATA;    // dump info if debug enabled
		if (((InterruptStatus & GOOD_BIT_SYNC) != GOOD_BIT_SYNC) ||
		    ((InputLength == 0) && (DataByte > MAX_PACKET_SIZE))) {
			// poor signal quality or not a legitimate packet length
			// so throw away the data and reset the FIFO
			ResetFifo();
			return;
			}
		// seems to be a strong signal so run the data thru the crc and save it
		Crc = _crc16_update(Crc, DataByte);
	    InputData[InputLength] = DataByte;
		if (InputData[0] == InputLength++) {
			// all expected bytes in packet completely received
			// so check for a good crc
			if (Crc == 0) {
				// crc is good, so indicate that a packet is ready				
				InputLength -= 2;    // drop the two crc bytes from the packet
				InputLength *= -1;   // make the length negative to indicate packet is ready
				}
			else {
				// crc is bad
				DEBUG_CRC;         // dump info if debug enabled
				InputLength = 0;   // bad packet crc, so discard
				}
			// reset FIFO to start search for sync
			ResetFifo();
			}
		}
	else if (OutputIndex <= OutputLength) 
		// in transmit mode with more data to send
		Rfm12bSpi.SendWord (RFM12B_TX_CMD + OutputData[OutputIndex++]);
	else {
		// completed a packet transmission, so switch to receive mode
		OutputIndex = OutputLength = OutputData[0] = 0;
		Rfm12bSpi.SendWord (RF_RECEIVER_ON);
		ResetFifo();
		}
	}

