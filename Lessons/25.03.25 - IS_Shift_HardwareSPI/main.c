#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define DELAY 300
#define DELAY_TIM1A F_CPU / 1024 * DELAY / 1000
#define PRESCALE_T1 (1 << CS12) | (1 << CS10)

#define CLK PINB5
#define DAT PINB3
#define LAT PINB1
#define END_TRANSMISSION 0x80

volatile uint8_t flagTIM1A = 0;

void InitTimer();
void Send32(uint32_t data);
void SPI_MasterInit();
void SPI_MasterTransmit(uint8_t);

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

ISR(TIMER0_COMPA_vect)
{
	flagTIM1A = 1;
	OCR1A += DELAY_TIM1A;
}

int main(void)
{
	DDRB = 0xFF;
	InitTimer();
	SPI_MasterInit();
	sei();
	uint16_t cnt = 0;
	uint32_t data32 = 0;
	uint8_t thousands, hundreds, decades, units = 0;
	uint8_t *p;
	while (1)
	{
		if (flagTIM1A)
		{
			flagTIM1A = 0;
			
			thousands = cnt / 1000;
			hundreds = (cnt / 100) % 10;
			decades = (cnt / 10) % 10;
			units = cnt % 10;
			
			p = (uint8_t*)&data32;
			*p++ = segments[units];
			*p++ = segments[decades];
			*p++ = segments[hundreds];
			*p = segments[thousands];
			Send32(data32);
			
			cnt = cnt < 10000 ? cnt + 1 : 0;
		}
	}
}

void InitTimer()
{
	TCCR1A = 0;
	TCCR1B = PRESCALE_T1;
	TIMSK1 = (1 << OCIE1A);
	OCR1A = DELAY_TIM1A;
}

void Send32(uint32_t data)
{
	SPI_MasterTransmit(data);
	SPI_MasterTransmit(data >> 8);
	SPI_MasterTransmit(data >> 16);
	SPI_MasterTransmit(data >> 24);
	PORTB &= ~(1 << LAT);
	PORTB |= (1 << LAT);
	PORTB &= ~(1 << LAT);
}

void SPI_MasterInit()
{
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void SPI_MasterTransmit(uint8_t data)
{
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
}