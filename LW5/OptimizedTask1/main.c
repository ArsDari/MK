#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define NEXT_DIGIT_DELAY 4
#define TIMER_7SEG_DELAY F_CPU / 1024 * NEXT_DIGIT_DELAY / 1000

#define INCREMENT_DELAY 100
#define TIMER_INCREMENT_DELAY F_CPU / 1024 * INCREMENT_DELAY / 1000

#define PRESCALE_1024 (1 << CS12) | (1 << CS10)

#define COUNTER_NORMAL_MODE 0
#define COUNTER_STOP 1
#define COUNTER_RESET 2

#define COUNT_ALLOWED 0
#define COUNT_DISALLOWED 1

#define UNIT 0
#define DECADE 1
#define HUNDRED 2
#define THOUSAND 3

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

volatile uint8_t counterMode = COUNTER_NORMAL_MODE, allowCount = COUNT_ALLOWED, whichDigit = UNIT;

void initializeTimers();

int main(void)
{
    initializeTimers();
	DDRB |= 0x0F;
    DDRD = 0xFF & ~(1 << PIND2);
    PORTD |= (1 << PIND2);
    EIMSK |= (1 << INT0);
    EICRA |= (1 << ISC01);
    sei();
	uint16_t counter = 0;
    while (1)
    {
        switch (counterMode)
        {
			case COUNTER_NORMAL_MODE:
			{
				if (allowCount == COUNT_ALLOWED)
				{
					allowCount = COUNT_DISALLOWED;
					counter = counter < 9999 ? counter + 1 : 0;
				}
				break;
			}
			case COUNTER_STOP:
			{
				allowCount = COUNT_DISALLOWED;
				break;
			}
			case COUNTER_RESET:
			{
				counterMode = COUNTER_NORMAL_MODE;
				counter = 0;
				break;
			}
        }
        uint8_t data = 0;
        switch (whichDigit)
        {
			case THOUSAND:
			{
				data = segments[counter / 1000];
				break;
			}
			case HUNDRED:
			{
				data = segments[(counter / 100) % 10];
				break;
			}
			case DECADE:
			{
				data = segments[(counter / 10) % 10];
				break;
			}
			case UNIT:
			{
				data = segments[counter % 10];
				break;
			}
        }
        PORTB = 0x0F & ~(1 << whichDigit);
        PORTD = ((data << 1) & MASK_AFTER_PIND2) | (1 << PIND2) | (data & MASK_BEFORE_PIND2);
    }
}

ISR(INT0_vect)
{
    counterMode++;
}

ISR(TIMER0_COMPA_vect)
{
	whichDigit = whichDigit < 3 ? whichDigit + 1 : 0;
	OCR1A += TIMER_7SEG_DELAY;
}

ISR(TIMER1_COMPA_vect)
{
    allowCount = COUNT_ALLOWED;
    OCR1A += TIMER_INCREMENT_DELAY;
}

void initializeTimers()
{
    TCCR1A = 0;
    TCCR1B = PRESCALE_1024;
    TIMSK1 |= (1 << OCIE1A);
    OCR1A = TIMER_INCREMENT_DELAY; // INCREMENT TIMER

    TCCR0A = 0;
    TCCR0B = PRESCALE_1024;
    TIMSK0 = (1 << OCIE0A);
    OCR0A = TIMER_7SEG_DELAY; // 7SEG TIMER
}