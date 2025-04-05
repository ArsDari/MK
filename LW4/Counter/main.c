#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t segments[] =
{
	0b00111111,
	0b00000110,
	0b01011011,
	0b01001111,
	0b01100110,
	0b01101101,
	0b01111101,
	0b00000111,
	0b01111111,
	0b01101111
};

volatile uint8_t counter = 0;

ISR(INT0_vect)
{
	if (counter < 15)
		counter++;
	else
		counter = 0;
}

int main(void)
{
	DDRB = 0xFF;
	DDRC = 0xFF;
	PORTD |= (1 << PIND2); // set input for PIND
	EIMSK |= (1 << INT0); // turn on INT0
	EICRA |= (1 << ISC01); // set interruptions on rising edge INT0
	sei();
	while (1)
	{
		PORTB = segments[counter % 10];
		PORTC = segments[counter / 10];
	}
}