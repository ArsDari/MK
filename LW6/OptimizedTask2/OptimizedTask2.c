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

#define ASCII_SYMBOL_START 0x30
#define SPACE_SYMBOL 0x20
#define MAX_BUFFER_SIZE 256

#define USART_BUSY 1
#define USART_STANDBY 0
#define BAUD_RATE 4800

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
void initializeUSART();

void transmitSPI(uint8_t);
void fillBuffer(uint16_t);
void send32(uint32_t);

void push(char);
void pushString(char *);

volatile char bufferUSART[MAX_BUFFER_SIZE];
volatile uint8_t currentElementIndex = 0;
volatile uint8_t lastElementIndex = 0;
volatile uint8_t stateUSART = USART_STANDBY;

volatile uint16_t voltageValue = 0;
volatile uint32_t segBuffer = 0;

int main(void)
{
	initializePorts();
	initializeTimer();
	initializeSPI();
	initializeADC();
	initializeUSART();
	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC01);
	sei();
	pushString("USART Check - OK!\r\n");
	while (1)
	{
		
	}
}

ISR(TIMER1_COMPB_vect)
{
	fillBuffer(voltageValue);
	send32(segBuffer);
}

ISR(INT0_vect)
{
	pushString("Value = ");
	push(ASCII_SYMBOL_START + voltageValue / 1000);
	push(ASCII_SYMBOL_START + (voltageValue / 100) % 10);
	push(ASCII_SYMBOL_START + (voltageValue / 10) % 10);
	push(ASCII_SYMBOL_START + voltageValue % 10);
	pushString("\r\n");
}

ISR(ADC_vect)
{
	voltageValue = ADC;
}

ISR(USART_RX_vect)
{
	if (UDR0 == SPACE_SYMBOL)
		pushString("Roger that\r\n");
}

ISR(USART_TX_vect)
{
	if (currentElementIndex == lastElementIndex)
	{
		stateUSART = USART_STANDBY;
		currentElementIndex = 0;
		lastElementIndex = 0;
	}
	else
		UDR0 = bufferUSART[currentElementIndex++];
}

void initializePorts()
{
	DDRB = (1 << LAT) | (1 << SS) | (1 << DAT) | (1 << CLK);
	DDRD &= ~(1 << PIND2);
	PORTD |= (1 << PIND2);
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
	SPCR = (1 << SPE) | (1 << MSTR) |(1 << SPR1) | (1 << SPR0);
	PORTB &= ~(1 << DAT) | (1 << CLK);
}

void initializeADC()
{
	ADMUX = (1 << MUX0);
	ADCSRA = (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | PRESCALE_ADC;
	ADCSRB = (1 << ADTS2) |	(1 << ADTS0);
}

void initializeUSART()
{
	UCSR0B = (1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UDORD0) | (1 << UCPHA0);
	UBRR0 = F_CPU / 16 / BAUD_RATE - 1;
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

void pushString(char *string)
{
	while (*string != 0)
		push(*string++);
}

void push(char element)
{
	bufferUSART[lastElementIndex++] = element;
	if (stateUSART == USART_STANDBY)
	{
		stateUSART = USART_BUSY;
		UDR0 = bufferUSART[currentElementIndex++];
	}
}