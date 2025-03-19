#include <avr/io.h>

int main(void)
{
	DDRB |= (1 << PINB4);
	PORTB |= (1 << PINB4);
    while (1) 
    {
		
    }
}