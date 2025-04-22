#include "USART.h"

void InitUSART()
{
    UCSR0B = (1 << RXEN0 | 1 << TXEN0 | 1 << RXCIE0);
    UCSR0C = (1 << UCSZ01 | 1 << UCSZ00);
    UBRR0H = 0;
    UBRR0L = F_CPU / 16 / BAUDRATE - 1;
}

void SendChar(char symbol)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = symbol;
}

void SendString(char *buffer)
{
    while(*buffer != 0)
    {
        SendChar(*buffer++);
    }
}