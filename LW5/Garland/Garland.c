#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define DELAY 100
#define TIMER_DELAY F_CPU / 1024 * DELAY / 1000
#define PRESCALE_1024 (1 << CS12) | (1 << CS10)

#define CLK PINB5
#define DAT PINB3
#define SS PINB2
#define LAT PINB1

#define FORWARD 1
#define BACKWARD 0

#define REBOUND_WITH_TWO_LED 0
#define ACCUMULATE 1
#define REBOUND 2

#define FULL_GARLAND 0x0007FFFF
#define EMPTY_GARLAND 0x00000000

#define START_COUNTER 0
#define END_COUNTER 19

#define UPDATE_ALLOWED 1
#define UPDATE_DISALLOWED 0

volatile uint8_t allowUpdate = UPDATE_DISALLOWED;
volatile uint8_t effect = REBOUND_WITH_TWO_LED;
volatile uint32_t frame = EMPTY_GARLAND;
volatile uint8_t counter = START_COUNTER;
volatile uint8_t direction = FORWARD;

void initializePorts();
void initializeTimer();
void initializeSPI();
void transmitSPI(uint8_t);
void send32(uint32_t);
void generateEffect();

int main(void)
{
	initializePorts();
	initializeTimer();
	initializeSPI();
	EICRA |= (1 << ISC01);
	EIMSK |= (1 << INT0);
    sei();
    while (1)
    {
		if (allowUpdate == UPDATE_ALLOWED)
		{
			allowUpdate = UPDATE_DISALLOWED;
			generateEffect();
		}
    }
}

ISR(INT0_vect)
{
	effect = effect < 2 ? effect + 1 : 0;
	frame = EMPTY_GARLAND;
	counter = START_COUNTER;
	direction = FORWARD;
	send32(frame);
}

ISR(TIMER1_COMPA_vect)
{
	allowUpdate = UPDATE_ALLOWED;
	send32(frame);
}

void initializePorts()
{
	DDRD &= ~(1 << PIND2);
	PORTD |= (1 << PIND2);
	DDRB = (1 << LAT) | (1 << SS) | (1 << DAT) | (1 << CLK);
}

void initializeTimer()
{
	TCCR1A = 0;
	TCCR1B = PRESCALE_1024 | (1 << WGM12);
	TIMSK1 = (1 << OCIE1A);
	OCR1A = TIMER_DELAY;
}

void initializeSPI()
{
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
	PORTB &= ~(1 << DAT) | (1 << CLK);
}

void transmitSPI(uint8_t data)
{
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
}

void send32(uint32_t data)
{
	PORTB &= ~(1 << LAT);
	transmitSPI(data >> 24);
	transmitSPI(data >> 16);
	transmitSPI(data >> 8);
	transmitSPI(data);
	PORTB |= (1 << LAT);
}

void generateEffect()
{
    switch (effect)
    {
		case REBOUND_WITH_TWO_LED:
		{
			if (direction)
			{
				frame |= (1UL << counter++);
				frame |= (1UL << counter++);
				if (frame >= FULL_GARLAND)
				{
					direction = BACKWARD;
				}
			}
			else
			{
				frame &= ~(1UL << --counter);
				frame &= ~(1UL << --counter);
				if (frame == EMPTY_GARLAND)
				{
					direction = FORWARD;
				}
			}
			break;
		}
		case ACCUMULATE:
		{
			if (frame < FULL_GARLAND)
			{
				frame |= (1UL << counter++);
			}
			else
			{
				counter = START_COUNTER;
				frame = EMPTY_GARLAND;
			}
			break;
		}
		case REBOUND:
		{
			if (direction)
			{
				frame |= (1UL << counter++);
				if (frame == FULL_GARLAND)
				{
					direction = BACKWARD;
				}
			}
			else
			{
				frame &= ~(1UL << --counter);
				if (frame == EMPTY_GARLAND)
				{
					direction = FORWARD;
				}
			}
			break;
		}
    }
}