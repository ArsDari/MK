#include "SPI.h"
#include <avr/interrupt.h>

#define INCREMENT_DELAY 300
#define TIMER_INCREMENT_DELAY F_CPU / 1024 * INCREMENT_DELAY / 1000
#define PRESCALE_1024 (1 << CS12) | (1 << CS10)

#define COUNTER_NORMAL_MODE 0
#define COUNTER_STOP 1

#define COUNT_ALLOWED 0
#define COUNT_DISALLOWED 1

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

void initializeCounter();
void initializePorts();

volatile uint8_t counterMode = COUNTER_STOP, allowCount = COUNT_DISALLOWED;
volatile uint16_t counter = 0;
volatile uint32_t segBuffer = 0;

int main(void)
{
	initializeCounter();
	initializePorts();
	initializeSPI();
	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC01);
	sei();
	while (1)
	{
		uint8_t *p = (uint8_t *)&segBuffer;
		*p++ = segments[counter % 10];
		*p++ = segments[(counter / 10) % 10];
		*p++ = segments[(counter / 100) % 10];
		*p = segments[counter / 1000];
		send32(segBuffer);
		if (allowCount == COUNT_ALLOWED)
		{
			allowCount = COUNT_DISALLOWED;
			counter = counter < 9999 ? counter + 1 : 0;
		}
	}
}

ISR(INT0_vect)
{
	if (counterMode == COUNTER_NORMAL_MODE)
		counterMode = COUNTER_STOP;
	else
	{
		counterMode = COUNTER_NORMAL_MODE;
		counter = 0;
	}
}

ISR(TIMER1_COMPA_vect)
{
	if (counterMode == COUNTER_NORMAL_MODE)
		allowCount = COUNT_ALLOWED;
	OCR1A += TIMER_INCREMENT_DELAY;
}

void initializeCounter()
{
	TCCR1A = 0;
	TCCR1B = PRESCALE_1024;
	TIMSK1 = (1 << OCIE1A);
	OCR1A = TIMER_INCREMENT_DELAY;
}

void initializePorts()
{
	DDRB = (1 << OE) | (1 << LAT) | (1 << SS) | (1 << DAT) | (1 << CLK);
	PORTB &= ~(1 << OE);
	DDRD &= ~(1 << BTN);
	PORTD |= (1 << BTN);
}