/*
 * Part_1_a.c
 *
 * Created: 17.03.2025 17:41:03
 * Author : User
 */ 

#include <avr/io.h>


int main(void)
{
    DDRB |= (1 << PINB4);
	PORTB |= (1 << PINB4);
    while (1) 
    {
    }
}

