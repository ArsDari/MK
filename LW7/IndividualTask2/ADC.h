#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

#define PRESCALE_128 (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)
#define AVCC 1
#define AREF 0

extern volatile uint8_t currentAnalogChannel;
extern volatile uint8_t currentVoltageReference;
extern volatile uint16_t ADCValue;

void initializeADC();

#endif