#ifndef USART_H_
#define USART_H_

/* доделать тут короче и ошибки исправить */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

#define ASCII_SYMBOL_START 0x30
#define MAX_BUFFER_SIZE 256

#define UART_BUSY 1
#define UART_STANDBY 0
#define BAUD_RATE 4800

extern char bufferUART[MAX_BUFFER_SIZE];
extern currentElementIndex = 0;
extern uint8_t lastElementIndex = 0;
extern uint8_t stateUART = UART_STANDBY;

void initializeUSART(void);
void push(char);
void pushString(char *);

#endif