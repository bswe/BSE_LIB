/*
 * usart.h
 *
 * Author: Bollenbacher Software Engineering
 * Created: 7/14/13
 */ 


#ifndef USART_H_
#define USART_H_

#define FOSC 16000000 // Clock Speed
//#define BAUD 9600	  // Baud Rate
#define BAUD 57600	  // Baud Rate
#define MYUBRR (((((FOSC * 10) / (16L * BAUD)) + 5) / 10) - 1)

void USART_Init (unsigned int Baud);
void USART_Sendbyte (unsigned char data);
void USART_Send_string (const char *str);
void USART_Send_int (unsigned int d);
unsigned char USART_Receive (void);

#endif /* USART_H_ */