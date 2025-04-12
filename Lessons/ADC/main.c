#include "ConfigADC.h"

volatile uint8_t ADCValue;

void InitADC()
{
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) | (7 << ADPS0);
	ADCSRB = 0;
}

ISR(ADC_vect)
{
	ADCValue = ADC;
}

int main(void)
{
	sei();
	float ADCVolts = 0;
	char[30] str;
	while (1)
	{
		if (flagTIM1A)
		{
			flagTIM1A = 0;
			
			thousend = ADCValue / 1000;
			hundreds = (ADCValue % 1000) / 100;
			decades = (ADCValue % 100) / 10;
			units = ADCValue % 10;
			
			p = &data32;
			*p++ = ~segments[units];
			*p++ = ~segments[decades];
			*p++ = ~segments[hundreds];
			*p = ~segments[thousend];
			
			Send32(data32);
			//USART_TX_Byte(units | 0x30);
			ADCValue = ADCValue * 5 / 1023;
			sprintf(str, "ADC = %d V= %f\r\n", ADCValue, ADCVolts);
			USART_SendString(str);
			counter++;
		}
	}
}