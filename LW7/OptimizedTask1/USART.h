#ifndef USART_H_
#define USART_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>

#define BAUDRATE 4800

void InitUSART(void);
void SendChar(char symbol);
void SendString(char * buffer);

#endif