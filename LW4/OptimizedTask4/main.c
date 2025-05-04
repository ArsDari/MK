#define F_CPU 16000000UL
#include <avr/io.h>
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

int main(void)
{
	DDRD = 0xFF;
	DDRB &= ~(1 << PINB0);
	PORTB |= (1 << PINB0);
	uint8_t button = 0;
	uint8_t switch_state = 0;
	uint8_t counter = 0;
	while (1)
	{
		button = PINB & (1 << PINB0);
		if (button == 0)
		{
			while ((PINB & (1 << PINB0)) == 0)
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
		}
		if (switch_state == 0)
		{
			if (counter > 9)
				counter = 0;
			PORTD = segments[counter++];
			_delay_ms(1000);
		}
	}
}