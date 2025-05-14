#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define NEXT_DIGIT_DELAY 4
#define TIMER_7SEG_DELAY F_CPU / 1024 * NEXT_DIGIT_DELAY / 1000

#define INCREMENT_DELAY 300
#define TIMER_INCREMENT_DELAY F_CPU / 1024 * INCREMENT_DELAY / 1000
#define PRESCALE_1024 (1 << CS12) | (1 << CS10)

#define COUNTER_NORMAL_MODE 0
#define COUNTER_STOP 1

#define COUNT_ALLOWED 0
#define COUNT_DISALLOWED 1

#define UPDATE_ALLOWED 0
#define UPDATE_DISALLOWED 1

#define MASK_BEFORE_PIND2 0x03
#define MASK_AFTER_PIND2 0xF8

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
    0b01101111,
};

volatile uint8_t counterMode = COUNTER_NORMAL_MODE;
volatile uint8_t allowCount = COUNT_ALLOWED;
volatile uint8_t allowUpdate = UPDATE_ALLOWED;
volatile uint8_t whichDigit = 0;
volatile uint16_t counter = 0;
volatile uint32_t segBuffer = 0;

void initializeTimers();
void initializePorts();

int main(void)
{
    initializeTimers();
	initializePorts();
	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC01);
	sei();
    while (1)
    {
	    if (allowUpdate == UPDATE_ALLOWED)
	    {
		    allowUpdate = UPDATE_DISALLOWED;
			uint8_t *p = (uint8_t *)&segBuffer + whichDigit;
			PORTB = 0x0F & ~(1 << whichDigit);
			PORTD &= (1 << PIND2);
			PORTD |= ((*p << 1) & MASK_AFTER_PIND2) | (*p & MASK_BEFORE_PIND2);
			whichDigit = whichDigit < 3 ? whichDigit + 1 : 0;
	    }
		if (allowCount == COUNT_ALLOWED)
		{
			allowCount = COUNT_DISALLOWED;
			uint8_t *p = (uint8_t *)&segBuffer;
			*p++ = segments[counter / 1000];
			*p++ = segments[(counter / 100) % 10];
			*p++ = segments[(counter / 10) % 10];
			*p = segments[counter % 10];
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

ISR(TIMER0_COMPA_vect)
{
	allowUpdate = UPDATE_ALLOWED;
	OCR0A += TIMER_7SEG_DELAY;
}

ISR(TIMER1_COMPA_vect)
{
	if (counterMode == COUNTER_NORMAL_MODE)
	    allowCount = COUNT_ALLOWED;
    OCR1A += TIMER_INCREMENT_DELAY;
}

void initializeTimers()
{
    TCCR1A = 0;
    TCCR1B = PRESCALE_1024;
    TIMSK1 |= (1 << OCIE1A);
    OCR1A = TIMER_INCREMENT_DELAY;

    TCCR0A = 0;
    TCCR0B = PRESCALE_1024;
    TIMSK0 = (1 << OCIE0A);
    OCR0A = TIMER_7SEG_DELAY;
}

void initializePorts()
{
	DDRB = 0x0F;
	DDRD = 0xFF & ~(1 << PIND2);
	PORTD |= (1 << PIND2);
}