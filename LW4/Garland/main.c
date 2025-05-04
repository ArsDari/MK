#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define FORWARD 1
#define BACKWARD 0

#define REBOUND 0
#define ACCUMULATE 1
#define ACCUMULATE_AND_DECREASE 2

#define FULL_GARLAND 0x0007FFFF
#define EMPTY_GARLAND 0x00000000

#define START_COUNTER 0
#define END_COUNTER 19

#define MASK_FRAME 0x3F
#define MASK_BEFORE_PIND2 0x03
#define MASK_AFTER_PIND2 0xF8

volatile uint8_t effect = REBOUND;
volatile uint32_t frame = EMPTY_GARLAND;
volatile uint8_t counter = START_COUNTER;
volatile uint8_t direction = FORWARD;

void sendFrame();
void generateEffect();

int main(void)
{
	DDRB = 0xFF;
	DDRC = 0xFF;
	DDRD = 0xFF & ~(1 << PIND2);
	PORTD |= (1 << PIND2);
	EICRA |= (1 << ISC01);
	EIMSK |= (1 << INT0);
    sei();
    while (1)
    {
        sendFrame();
        generateEffect();
        _delay_ms(100);
    }
}

ISR(INT0_vect)
{
	effect = effect < 2 ? effect + 1 : 0;
	frame = EMPTY_GARLAND;
	counter = START_COUNTER;
	direction = FORWARD;
}

void sendFrame()
{
    PORTB = frame & MASK_FRAME;
    PORTC = (frame >> 6) & MASK_FRAME;
    PORTD = ((frame >> 11) & MASK_AFTER_PIND2) | (1 << PIND2) | ((frame >> 12) & MASK_BEFORE_PIND2);
}

void generateEffect()
{
    switch (effect)
    {
		case REBOUND:
		{
			if (direction)
			{
				if (frame < FULL_GARLAND)
				{
					frame |= (1UL << counter++);
					frame |= (1UL << counter++);
				}
				if (frame >= FULL_GARLAND)
					direction = BACKWARD;
			}
			else
			{
				if (frame > EMPTY_GARLAND)
				{
					frame &= ~(1UL << --counter);
					frame &= ~(1UL << --counter);
				}
				if (frame <= EMPTY_GARLAND)
					direction = FORWARD;
			}
			break;
		}
		case ACCUMULATE:
		{
			if (frame < FULL_GARLAND)
				frame |= (1UL << counter++);
			else
			{
				counter = START_COUNTER;
				frame = EMPTY_GARLAND;
			}
			break;
		}
		case ACCUMULATE_AND_DECREASE:
		{
			if (direction)
			{
				frame |= (1UL << counter++);
				if (frame == FULL_GARLAND)
					direction = BACKWARD;
			}
			else
			{
				frame &= ~(1UL << --counter);
				if (frame == EMPTY_GARLAND)
					direction = FORWARD;
			}
			break;
		}
    }
}