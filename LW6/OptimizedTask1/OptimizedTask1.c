#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define INCREMENT_DELAY 100
#define TIMER_INCREMENT_DELAY F_CPU / 1024 * INCREMENT_DELAY / 1000
#define PRESCALE_1024 (1 << CS12) | (1 << CS10)

#define PRESCALE_ADC (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)

#define CLK PINB5
#define DAT PINB3
#define SS PINB2
#define LAT PINB1

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
	0b01101111
};

void initializePorts();
void initializeTimer();
void initializeSPI();
void initializeADC();

void transmitSPI(uint8_t);
void fillBuffer(uint16_t);
void send32(uint32_t);

volatile uint16_t voltageValue = 0;
volatile uint32_t segBuffer = 0;

int main(void)
{
	initializePorts();
	initializeTimer();
	initializeSPI();
	initializeADC();
	sei();
	while (1)
	{
		
	}
}

ISR(TIMER1_COMPB_vect)
{
	fillBuffer(voltageValue);
	send32(segBuffer);
}

ISR(ADC_vect)
{
	voltageValue = ADC;
}

void initializePorts()
{
	DDRB = (1 << LAT) | (1 << SS) | (1 << DAT) | (1 << CLK);
}

void initializeTimer()
{
	TCCR1A = 0;
	TCCR1B = PRESCALE_1024 | (1 << WGM12);
	TIMSK1 = (1 << OCIE1B);
	OCR1A = TIMER_INCREMENT_DELAY;
	OCR1B = TIMER_INCREMENT_DELAY;
}

void initializeSPI()
{
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
	PORTB &= ~(1 << DAT) | (1 << CLK);
}

void initializeADC()
{
	ADMUX = (1 << MUX0);
	ADCSRA = (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | PRESCALE_ADC;
	ADCSRB = (1 << ADTS2) |	(1 << ADTS0);
}

void transmitSPI(uint8_t data)
{
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
}

void fillBuffer(uint16_t value)
{
	uint8_t *p = (uint8_t *)&segBuffer;
	*p++ = ~segments[value % 10];
	*p++ = ~segments[(value / 10) % 10];
	*p++ = ~segments[(value / 100) % 10];
	*p = ~segments[value / 1000];
}

void send32(uint32_t data)
{
	PORTB &= ~(1 << LAT);
	transmitSPI(data);
	transmitSPI(data >> 8);
	transmitSPI(data >> 16);
	transmitSPI(data >> 24);
	PORTB |= (1 << LAT);
}