#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define INCREMENT_DELAY 300
#define TIMER_INCREMENT_DELAY F_CPU / 1024 * INCREMENT_DELAY / 1000
#define PRESCALE_1024 (1 << CS12) | (1 << CS10)

#define COUNTER_NORMAL_MODE 0
#define COUNTER_STOP 1

#define COUNT_ALLOWED 0
#define COUNT_DISALLOWED 1

#define CLK PINB5
#define DAT PINB3
#define LAT PINB1
#define OE PINB0
#define BTN PIND2

#define ONE_BIT 0x80

uint8_t segments[] =
{
	0b00111111, // g, f, e, d, c, b, a
	0b00000101,
	0b01011011,
	0b01001111,
	0b01100101,
	0b01101110,
	0b01111110,
	0b00000111,
	0b01111111,
	0b01101111
};

void initializeCounter();
void initializePorts();
void sendWord(uint16_t);

volatile uint8_t counterMode = COUNTER_STOP, allowCount = COUNT_DISALLOWED;
volatile uint8_t counter = 0;
volatile uint16_t segBuffer = 0;

int main(void)
{
	initializeCounter();
	initializePorts();
	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC01);
	sei();
	while (1)
	{
		uint8_t *p = (uint8_t*)&segBuffer;
		*p++ = segments[counter % 10];
		*p = segments[counter / 10];
		sendWord(segBuffer);
		if (allowCount == COUNT_ALLOWED)
		{
			allowCount = COUNT_DISALLOWED;
			counter = counter < 99 ? counter + 1 : 0;
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
	DDRB = (1 << OE) | (1 << LAT) | (1 << DAT) | (1 << CLK);
	PORTB &= ~(1 << OE);
	DDRD &= ~(1 << BTN);
	PORTD |= (1 << BTN);
}

void sendByte(uint8_t data)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		if ((data << i) & ONE_BIT)
			PORTB |= (1 << DAT);
		else
			PORTB &= ~(1 << DAT);
		PORTB &= ~(1 << CLK);
		PORTB |= (1 << CLK);
	}
}

void sendWord(uint16_t data)
{
	PORTB &= ~(1 << LAT);
	sendByte(data >> 8);
	sendByte(data);
	PORTB |= (1 << LAT);
}