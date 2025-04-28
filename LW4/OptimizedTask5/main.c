#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

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

volatile uint8_t button = 0;
volatile uint8_t switch_state = 0;
volatile uint8_t counter = 0;

ISR(INT0_vect)
{
	if (switch_state == 0)
	{
		switch_state = 1;
	}
	else
	{
		switch_state = 0;
		counter = 0;
	}
}

int main(void)
{
	DDRB = 0xFF;
	PORTD |= (1 << PIND2);
	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC01);
	sei();
	while (1)
	{
		if (switch_state == 0)
		{
			if (counter > 9)
				counter = 0;
			PORTB = segments[counter++];
			_delay_ms(500);
		}
	}
}