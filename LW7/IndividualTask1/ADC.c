#include "ADC.h"

volatile uint8_t currentAnalogChannel = 0;
volatile uint8_t currentVoltageReference = AREF;
volatile uint16_t ADCValue = 0;

void initializeADC()
{
	ADMUX = (currentVoltageReference << REFS0) | currentAnalogChannel;
	ADCSRA = (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | PRESCALE_128;
	ADCSRB = (1 << ADTS2) |	(1 << ADTS0);
}