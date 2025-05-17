#include "USART.h"

ISR(USART_TX_vect)
{
	if (currentElementIndex == lastElementIndex)
	{
		stateUART = UART_STANDBY;
		currentElementIndex = 0;
		lastElementIndex = 0;
	}
	else
		UDR0 = bufferUART[currentElementIndex++];
}

void initializeUSART()
{
    UCSR0B = (1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UDORD0) | (1 << UCPHA0);
    UBRR0 = F_CPU / 16 / BAUD_RATE - 1;
}

void push(char element)
{
    bufferUART[lastElementIndex++] = element;
    if (stateUART == UART_STANDBY)
    {
	    stateUART = UART_BUSY;
	    UDR0 = bufferUART[currentElementIndex++];
    }
}

void pushString(char *buffer)
{
    while(*buffer != 0)
        push(*buffer++);
}