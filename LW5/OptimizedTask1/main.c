#include "Task1.h"

int main(void)
{
	initializePorts();
	initializeTimer();
	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC01);
	sei();
	uint16_t counter = 0;
	uint8_t thousands, hundreds, decimals, units = 0;
	uint8_t *p;
	uint16_t dataWord = 0;
	uint32_t data32 = 0;
	
	while (1)
	{
	    if (flagTIM1A)
	    {
			flagTIM1A = 0;
			
		    thousands = counter / 1000;
			hundreds = (counter / 100) % 10;
			decimals = (counter / 10) % 10;
			units = counter % 10;
			
			p = &data32;
			
	    }
	}
}

