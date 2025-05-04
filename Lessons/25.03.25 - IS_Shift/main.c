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
void SendByte(uint8_t data);
void SendWord(uint16_t data);
void Send32(uint32_t data);

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
	sei();
	uint16_t cnt = 0;
	uint16_t dataWord = 0;
	uint8_t decades, units = 0;
	uint8_t *p;
    while (1)
    {
		if (flagTIM1A)
		{
			flagTIM1A = 0;
			
			decades = cnt / 10;
			units = cnt % 10;
			
			p = (uint8_t*)&dataWord;
			*p++ = segments[units];
			*p = segments[decades];
			SendWord(dataWord);
			
			cnt = cnt < 99 ? cnt + 1 : 0;
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

void SendByte(uint8_t data)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		PORTB &= ~(1 << CLK);
		if (data & END_TRANSMISSION)
			PORTB |= (1 << DAT);
		else
			PORTB &= ~(1 << DAT);
		
		PORTB &= ~(1 << CLK);
		data <<= 1;
		PORTB |= (1 << CLK);
	}
}

void SendWord(uint16_t data)
{
	SendByte(data);
	SendByte((data >> 8));
	PORTB &= ~(1 << LAT);
	PORTB |= (1 << LAT);
	PORTB &= ~(1 << LAT);
}

void Send32(uint32_t data)
{
	
}