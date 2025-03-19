/*
 * C.c
 *
 * Created: 10.03.2025 16:49:34
 * Author : User
 */ 

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB |= (1 << PINB5);
	uint8_t dir = 1;
	
	while (1)
	{
		dir ^= 1;
		if (dir)
			PORTB |= (1 << PINB5);
		else
			PORTB &= ~(1 << PINB5);
		_delay_ms(541,592375);
	}
}