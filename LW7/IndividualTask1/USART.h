#ifndef USART_H_
#define USART_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>

#define MAX_BUFFER_SIZE 255
#define BAUD_RATE 4800

#define USART_RECEIVED 4
#define USART_RECEIVING 3
#define USART_TRANSMITTED 2
#define USART_TRANSMITTING 1
#define USART_STANDBY 0

extern volatile uint8_t stateUSART;

extern volatile char inputUSART[MAX_BUFFER_SIZE];
extern volatile uint8_t inputCurrentElementIndex;
extern volatile uint8_t inputLastElementIndex;

extern volatile char outputUSART[MAX_BUFFER_SIZE];
extern volatile uint8_t outputCurrentElementIndex;
extern volatile uint8_t outputLastElementIndex;

void initializeUSART();
void push(char);
void pushString(char *);
void startTransmitUSART();

#endif