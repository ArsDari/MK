#include "Timer.h"

void initializeTimer()
{
	TCCR1A = 0;
	TCCR1B = PRESCALE_1024 | (1 << WGM12);
	TIMSK1 = (1 << OCIE1B);
	OCR1A = TIMER_INCREMENT_DELAY;
	OCR1B = TIMER_INCREMENT_DELAY;
}