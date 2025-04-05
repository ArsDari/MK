#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB = 0xFF & ~(1 << PINB7);
	PORTB |= (1 << PINB7);
	int button = 0;
	while (1)
	{
		for (int i = 0; i < 6; i++)
		{
			button = PINB & (1 << PINB7);
			if (button != 0)
			{
				PORTB = (1 << i);
				_delay_ms(200);
			}
			else
			{
				PORTB = (0x20 >> i);
				_delay_ms(200);
			}
		}
	}
}