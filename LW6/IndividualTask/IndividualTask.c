#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#define INCREMENT_DELAY 100
#define TIMER_INCREMENT_DELAY F_CPU / 1024 * INCREMENT_DELAY / 1000
#define PRESCALE_1024 (1 << CS12) | (1 << CS10)

#define AVCC 1
#define AREF 0
#define PRESCALE_ADC (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)

#define CLK PINB5
#define DAT PINB3
#define SS PINB2
#define LAT PINB1

#define ASCII_SYMBOL_START 0x30
#define BACKSPACE_SYMBOL 0x08
#define WITH_NULL_SYMBOL 1
#define COMPARE_EQUAL 0

#define MAX_BUFFER_SIZE 255

#define USART_RECEIVED 4
#define USART_RECEIVING 3
#define USART_TRANSMITTED 2
#define USART_TRANSMITTING 1
#define USART_STANDBY 0

#define CONSOLE_INVALID_ARGUMENT 2
#define CONSOLE_UNKNOWN_COMMAND 1
#define CONSOLE_SUCCESS 0
#define CONSOLE_BUSY 1
#define CONSOLE_STANDBY 0
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
void startTransmitUSART();

uint8_t consoleHandler();

volatile uint8_t currentAnalogChannel = 0;
volatile uint8_t currentVoltageReference = AREF;

volatile uint8_t stateUSART = USART_STANDBY;
volatile uint8_t stateConsole = CONSOLE_STANDBY;

volatile char inputUSART[MAX_BUFFER_SIZE];
volatile uint8_t inputCurrentElementIndex = 0;
volatile uint8_t inputLastElementIndex = 0;

volatile char outputUSART[MAX_BUFFER_SIZE];
volatile uint8_t outputCurrentElementIndex = 0;
volatile uint8_t outputLastElementIndex = 0;

volatile char consoleBufferUSART[MAX_BUFFER_SIZE];
volatile uint8_t consoleBufferCurrentElementIndex = 0;
volatile uint8_t consoleBufferLastElementIndex = 0;

volatile uint16_t ADCValue = 0;
volatile uint32_t buffer7Seg = 0;

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
		switch (stateUSART)
		{
			case USART_RECEIVED:
			{
				stateUSART = USART_STANDBY;
				stateConsole = CONSOLE_BUSY;
				switch (consoleHandler())
				{
					case CONSOLE_SUCCESS:
					{
						break;
					}
					case CONSOLE_UNKNOWN_COMMAND:
					{
						pushString("Parse Failure! Unknown command.\r\n");
						break;
					}
					case CONSOLE_INVALID_ARGUMENT:
					{
						pushString("Parse Failure! Invalid argument.\r\n");
						break;
					}
					default:
					{
						pushString("Parse Failure! Unexpected error.\r\n");
						break;
					}
				}
				stateConsole = CONSOLE_STANDBY;
				break;
			}
			case USART_TRANSMITTED:
			{
				stateUSART = USART_STANDBY;
				break;
			}
			case USART_STANDBY:
			{
				startTransmitUSART();
				break;
			}
		}
	}
}

ISR(INT0_vect)
{
	pushString("ADC Value = ");
	push(ASCII_SYMBOL_START + ADCValue / 1000);
	push(ASCII_SYMBOL_START + (ADCValue / 100) % 10);
	push(ASCII_SYMBOL_START + (ADCValue / 10) % 10);
	push(ASCII_SYMBOL_START + ADCValue % 10);
	pushString(".\r\n");
}

ISR(TIMER1_COMPB_vect)
{
	
}

ISR(USART_RX_vect)
{
	uint8_t received = UDR0;
	if (received == '\r')
	{
		inputUSART[inputLastElementIndex] = '\0';
		inputCurrentElementIndex = 0;
		inputLastElementIndex = 0;
		if (stateConsole == CONSOLE_STANDBY)
		{
			memcpy((void *)(&consoleBufferUSART), (const void *)(&inputUSART), MAX_BUFFER_SIZE);
		}
		else
		{
			pushString("Console was busy! Please repeat your command...\r\n");
		}
		stateUSART = USART_RECEIVED;
	}
	else
	{
		if (received == BACKSPACE_SYMBOL)
		{
			--inputLastElementIndex;
		}
		else
		{
			inputUSART[inputLastElementIndex++] = received;
			if (inputLastElementIndex == MAX_BUFFER_SIZE)
			{
				inputLastElementIndex = 0;
				pushString("Buffer overflow! Expect wrong results...\r\n");
			}
		}
		stateUSART = USART_RECEIVING;
	}
}

ISR(USART_TX_vect)
{
	if (outputCurrentElementIndex == outputLastElementIndex)
	{
		outputCurrentElementIndex = 0;
		outputLastElementIndex = 0;
		stateUSART = USART_TRANSMITTED;
	}
	else
	{
		UDR0 = outputUSART[outputCurrentElementIndex++];
	}
}

ISR(ADC_vect)
{
	ADCValue = ADC;
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
	ADMUX = (currentVoltageReference << REFS0) | currentAnalogChannel;
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
	uint8_t *p = (uint8_t *)&buffer7Seg;
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

void push(char element)
{
	if (outputLastElementIndex == MAX_BUFFER_SIZE)
	{
		outputUSART[MAX_BUFFER_SIZE - 3] = '\r';
		outputUSART[MAX_BUFFER_SIZE - 2] = '\n';
		outputUSART[MAX_BUFFER_SIZE - 1] = '\0';
	}
	else
	{
		outputUSART[outputLastElementIndex++] = element;
	}
	startTransmitUSART();
}

void pushString(char *string)
{
	while (*string != '\0')
	{
		push(*string++);
	}
}

void startTransmitUSART()
{
	if (outputCurrentElementIndex < outputLastElementIndex && stateUSART == USART_STANDBY)
	{
		UDR0 = outputUSART[outputCurrentElementIndex++];
		stateUSART = USART_TRANSMITTING;
	}
}

uint8_t consoleHandler()
{
	if (strncmp((const char *)(consoleBufferUSART), "/set_register ", 14) == COMPARE_EQUAL)
	{
		if (strncmp((const char *)(&consoleBufferUSART[14]), "ADMUX ", 6) == COMPARE_EQUAL)
		{
			if (consoleBufferUSART[21] == ' ' && consoleBufferUSART[23] == '\0')
			{
				uint8_t analogChannel = consoleBufferUSART[20];
				uint8_t voltageReference = consoleBufferUSART[22];
				if (('0' <= analogChannel && analogChannel <= '1') && ('0' <= voltageReference && voltageReference <= '1'))
				{
					analogChannel -= ASCII_SYMBOL_START;
					voltageReference -= ASCII_SYMBOL_START;
					ADMUX &= 0xF0;
					ADMUX |= analogChannel;
					pushString("ADMUX Updated: Channel = ");
					push(analogChannel + ASCII_SYMBOL_START);
					pushString(", Voltage Reference = ");
					if (voltageReference == AREF)
					{
						ADMUX &= ~(1 << REFS0);
						pushString("AREF, internal VREF turned off");
					}
					else
					{
						ADMUX |= (1 << REFS0);
						pushString("AVCC with external capacitor at AREF pin");
					}
					pushString(".\r\n");
					currentAnalogChannel = analogChannel;
					currentVoltageReference = voltageReference;
				}
				else
				{
					return CONSOLE_INVALID_ARGUMENT;
				}
			}
			else
			{
				return CONSOLE_INVALID_ARGUMENT;
			}
		}
		else
		{
			return CONSOLE_INVALID_ARGUMENT;
		}
	}
	else if (strncmp((const char *)(consoleBufferUSART), "/get_register ", 14) == COMPARE_EQUAL)
	{
		if (strncmp((const char *)(&consoleBufferUSART[14]), "ADMUX", 5 + WITH_NULL_SYMBOL) == COMPARE_EQUAL)
		{
			pushString("ADMUX: Channel = ");
			push(currentAnalogChannel + ASCII_SYMBOL_START);
			pushString(", Voltage Reference = ");
			if (currentVoltageReference == AVCC)
			{
				pushString("AVCC with external capacitor at AREF pin");
			}
			else
			{
				pushString("AREF, internal VREF turned off");
			}
			pushString(".\r\n");
		}
		else
		{
			return CONSOLE_INVALID_ARGUMENT;
		}
	}
	else if (strncmp((const char*)(consoleBufferUSART), "/send_via_SPI ", 14) == COMPARE_EQUAL)
	{
		if (strncmp((const char *)(&consoleBufferUSART[14]), "ADC", 3 + WITH_NULL_SYMBOL) == COMPARE_EQUAL)
		{
			fillBuffer(ADCValue);
			send32(buffer7Seg);
			pushString("Sent ADC value via SPI.\r\n");
		}
		else if (strncmp((const char *)(&consoleBufferUSART[14]), "VALUE ", 6) == COMPARE_EQUAL)
		{
			if (consoleBufferUSART[23 + WITH_NULL_SYMBOL] == '\0')
			{
				uint16_t frame = 0;
				for (uint8_t i = 0; i < 4; i++)
				{
					if ('0' <= consoleBufferUSART[20 + i] && consoleBufferUSART[20 + i] <= '9')
					{
						frame = frame * 10 + (consoleBufferUSART[20 + i] - ASCII_SYMBOL_START);
					}
					else
					{
						return CONSOLE_INVALID_ARGUMENT;
					}
				}
				fillBuffer(frame);
				send32(buffer7Seg);
				pushString("Sent input value via SPI.\r\n");
			}
			else
			{
				return CONSOLE_INVALID_ARGUMENT;
			}
		}
		else
		{
			return CONSOLE_INVALID_ARGUMENT;
		}
	}
	else
	{
		return CONSOLE_UNKNOWN_COMMAND;
	}
	
	return CONSOLE_SUCCESS;
}