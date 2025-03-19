/*
 * P1B.c
 *
 * Created: 17.03.2025 18:07:52
 * Author : User
 */ 

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRB = 0xFF;
    while (1)
    {
		for (int i = 0; i < 6; i++)
		{
			PORTB = (1 << i);
			_delay_ms(200);
		}
    }
}

