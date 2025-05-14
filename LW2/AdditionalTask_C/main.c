#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define DELAY 300
#define OCR1A_DELAY F_CPU / 1024 * DELAY / 1000
#define LATCH PINB1
#define DATA PINB3
#define SCLK PINB5

#define UPDATE_ALLOWED 1
#define UPDATE_DISALLOWED 0
#define SHOW_ALLOWED 1
#define SHOW_DISALLOWED 0

void initializeTimer();
void sendByte(uint8_t);
void sendWord(uint16_t);

uint16_t dataWords[] =
{
	0x0001,
	0x0002,
	0x0004,
	0x0008,
	0x0010,
	0x0020,
	0x0040,
	0x0080,
	0x0101,
	0x0202,
	0x0404,
	0x0808,
	0x1010,
	0x2020,
	0x4040,
	0x8080
};

volatile uint8_t allowUpdate = UPDATE_DISALLOWED;

int main(void)
{
    DDRB |= (1 << LATCH) | (1 << DATA) | (1 << SCLK);
	DDRD |= (1 << PIND1);
	initializeTimer();
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	sei();
	uint8_t counter = 0, allowShowLED = SHOW_DISALLOWED;
    while (1)
    {
		if (allowUpdate)
		{
			allowUpdate = UPDATE_DISALLOWED;
			sendWord(dataWords[counter]);
			counter = counter < 15 ? counter + 1 : 0;
			allowShowLED ^= 1;
			if (allowShowLED)
				PORTD |= (1 << PIND1);
			else
				PORTD &= ~(1 << PIND1);
		}
    }
}

ISR(TIMER1_COMPA_vect)
{
	allowUpdate = UPDATE_ALLOWED;
	OCR1A += OCR1A_DELAY;
}

void initializeTimer()
{
	TCCR1B = (1 << CS12) | (1 << CS10);
	TIMSK1 = (1 << OCIE1A);
	OCR1A = OCR1A_DELAY;
}

void sendByte(uint8_t byte)
{
	SPDR = byte;
	while (!(SPSR & (1 << SPIF)));
}

void sendWord(uint16_t word)
{
	sendByte(word >> 8);
	sendByte(word);
	PORTB |= (1 << LATCH);
	PORTB &= ~(1 << LATCH);
}