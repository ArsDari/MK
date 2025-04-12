#include "Task1.h"

ISR(TIMER1_COMPA_vect)
{
	flagTIM1A = 1;
	OCR1A += TICKS_TIM1A;
}

void initializePorts()
{
	DDRB = 0xFF;
}

void initializeTimer()
{
	TCCR1A = 0;
	TCCR1B = PRESCALE_T1;
	TIMSK1 = (1 << OCIE1A);
}

void sendByte(uint8_t data)
{

}

void sendWord(uint16_t data)
{

}

void send32(uint8_t data)
{

}