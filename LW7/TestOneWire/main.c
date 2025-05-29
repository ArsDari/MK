#define F_CPU 16000000UL
#include <avr/io.h>
#include "USART.h"
#include "OneWire.h"

int main(void)
{
	initializeUSART();
	sei();
    initializeOneWire();
    while (1) 
    {
		
    }
}