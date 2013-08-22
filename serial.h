/*
 * serial.h
 *
 * Author: Bollenbacher Software Engineering
 * Created: 7/14/13
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