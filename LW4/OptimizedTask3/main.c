#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB = 0xFF & ~(1 << PINB7);
	PORTB |= (1 << PINB7);
	while (1)
	{
		for (int i = 0; i < 6; i++)
		{
			if ((PINB7 & ~PORTB) != 0)
				PORTB = (1 << i);
			else
				PORTB = (0x20 >> i);
			_delay_ms(200);
		}
	}
}