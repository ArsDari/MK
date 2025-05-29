#include "USART.h"

volatile uint8_t stateUSART = USART_STANDBY;

volatile char inputUSART[MAX_BUFFER_SIZE];
volatile uint8_t inputCurrentElementIndex = 0;
volatile uint8_t inputLastElementIndex = 0;

volatile char outputUSART[MAX_BUFFER_SIZE];
volatile uint8_t outputCurrentElementIndex = 0;
volatile uint8_t outputLastElementIndex = 0;

void initializeUSART()
{
	UCSR0B = (1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UDORD0) | (1 << UCPHA0);
	UBRR0 = F_CPU / 16 / BAUD_RATE - 1;
}

void push(char element)
{
	if (outputLastElementIndex == MAX_BUFFER_SIZE)
	{
		outputUSART[MAX_BUFFER_SIZE - 3] = '\r';
		outputUSART[MAX_BUFFER_SIZE - 2] = '\n';
		outputUSART[MAX_BUFFER_SIZE - 1] = '\0';
	}
	else
	{
		outputUSART[outputLastElementIndex++] = element;
	}
	startTransmitUSART();
}

void pushString(char *string)
{
	while (*string != '\0')
	{
		push(*string++);
	}
}

void startTransmitUSART()
{
	if (outputCurrentElementIndex < outputLastElementIndex && stateUSART == USART_STANDBY)
	{
		UDR0 = outputUSART[outputCurrentElementIndex++];
		stateUSART = USART_TRANSMITTING;
	}
}