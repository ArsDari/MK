#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB = 0xFF;
	DDRC = 0x00;
	PORTC |= (1 << PINC5);
	while (1)
	{
		for (uint8_t i = 0; i < 6; i++)
		{
			if ((PINC & (1 << PINC5)) != 0)
				PORTB = (1 << i);
			else
				PORTB = (0x20 >> i);
			_delay_ms(200);
		}
	}
}