#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define MASK_BEFORE_PIND2 0x03
#define MASK_AFTER_PIND2 0xF8

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
	DDRD = 0xFF & ~(1 << PIND2);
	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC01);
	sei();
	while (1)
	{
		if (switch_state == 0)
		{
			if (counter > 9)
				counter = 0;
			uint8_t dec = segments[counter++];
			PORTD = ((dec << 1) & MASK_AFTER_PIND2) | (dec & MASK_BEFORE_PIND2);
			_delay_ms(500);
		}
	}
}