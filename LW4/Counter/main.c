#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#define MASK_BEFORE_PIND2 0x03
#define MASK_AFTER_PIND2 0xF8
#define MASK_BEFORE_PINC6 0x3F
#define MASK_ONLY_PINB0 0x01

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

volatile uint8_t switch_state = 1;

ISR(INT0_vect)
{
	switch_state = 1;
}

int main(void)
{
	DDRB = 0x01;
	DDRC = 0xFF;
	DDRD = 0xFF & ~(1 << PIND2);
	EIMSK |= (1 << INT0); // включить прерывание INT0
	EICRA = (1 << ISC01) | (1 << ISC00); // по нарастающему фронту
	sei();
	uint8_t counter = 15;
	while (1)
	{
		if (switch_state)
		{
			switch_state = 0;
			counter = counter < 15 ? counter + 1 : 0;
			uint8_t dec = segments[counter / 10];
			uint8_t unit = segments[counter % 10];
			PORTD = ((dec << 1) & MASK_AFTER_PIND2) | (dec & MASK_BEFORE_PIND2); // два бита до PIND2 и после все остальные биты
			PORTC = unit & MASK_BEFORE_PINC6;
			PORTB = (unit >> PINB6) & MASK_ONLY_PINB0;
		}
	}
}