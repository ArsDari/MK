#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUDRATE 9600
#define CARD_DATA_SIZE 5
#define CARD_COUNT 4

#define READY 1
#define RECEIVING 2
#define RECEIVED 3

#define UART_RCV_ON UCSR0B = (1 << RXEN0) | (1 << RXCIE0)
#define UART_RCV_OFF UCSR0B = 0x00

#define TX_SOFT 1
#define SET_0_SOFT PORTD &= ~(1 << TX_SOFT)
#define SET_1_SOFT PORTD |= (1 << TX_SOFT)

volatile char cardData[CARD_DATA_SIZE * CARD_COUNT] =
{
	0x22, 0x00, 0xB2, 0x86, 0x63,
	0x22, 0x00, 0xB2, 0x76, 0xC7,
	0x07, 0x00, 0x54, 0x37, 0xA7,
	0x07, 0x00, 0x54, 0x37, 0xA5
};

volatile char inputBuffer[15];
volatile char tempCardData[CARD_DATA_SIZE + 1];
volatile uint8_t bufferCounter = 0;
volatile uint8_t stateUSART = READY;

void initializePorts();
void initializeUSART();
void sendSoftUSART(char);
void sendStringSoftUSART(char *);
uint8_t charToByte(char);

int main(void)
{
	initializePorts();
	initializeUSART();
	SET_1_SOFT;
	sendStringSoftUSART("Hello\r\n");
	sei();
	int8_t cardId = -1;
	int8_t currentMatch = -1;
	uint8_t calculatedCRC = 0;
	uint8_t recievedCRC = 0;
	while (1)
	{
		if (stateUSART == RECEIVED)
		{
			stateUSART = READY;
			recievedCRC = (charToByte(inputBuffer[11]) << 4);
			recievedCRC |= charToByte(inputBuffer[12]);
			calculatedCRC = 0;
			for (uint8_t i = 0; i < CARD_DATA_SIZE; i++)
			{
				uint16_t tempValue = (charToByte(inputBuffer[2 * i + 1]) << 4) | (charToByte(inputBuffer[2 * i + 2]));
				tempCardData[i] = tempValue & 0xFF;
				calculatedCRC ^= tempCardData[i];
			}
			UART_RCV_OFF;
			sendStringSoftUSART((char *)(inputBuffer));
			if ((recievedCRC == calculatedCRC) & (recievedCRC != 0))
			{
				sendStringSoftUSART(" CRC OK ");
			}
			else
			{
				sendStringSoftUSART(" CRC InCorrect ");
			}
			cardId = -1;
			uint8_t shift = 0;
			for (uint8_t i = 0; i < CARD_COUNT; i++)
			{
				currentMatch = tempCardData[CARD_DATA_SIZE - 1] ^ cardData[shift + CARD_DATA_SIZE - 1];
				for (int8_t j = CARD_DATA_SIZE - 2; j >= 0; j--)
				{
					currentMatch += tempCardData[j] ^ cardData[shift + j];
					if (currentMatch > 0)
					{
						break;
					}
				}
				shift += CARD_DATA_SIZE;
				if (currentMatch == 0)
				{
					cardId = i;
					break;
				}
			}
			if (cardId >= 0)
			{
				sendStringSoftUSART("CARD INDEX = ");
				sendSoftUSART(cardId + 0x30);
			}
			else
			{
				sendStringSoftUSART("CARD NOT FOUND ");
			}
			sendStringSoftUSART("\r\n");
			UART_RCV_ON;
		}
	}
}

ISR(USART_RX_vect)
{
	char received = UDR0;
	if (received == 0x02)
	{
		bufferCounter = 0;
		stateUSART = RECEIVING;
	}
	if (stateUSART == RECEIVING)
	{
		inputBuffer[bufferCounter++] = received;
		if (bufferCounter > 14)
		{
			bufferCounter = 0;
		}
	}
	if (received == 0x03 && bufferCounter > 0)
	{
		stateUSART = RECEIVED;
	}
}

uint8_t charToByte(char value)
{
	switch(value)
	{
		case '0':
			return 0x00;
		case '1':
			return 0x01;
		case '2':
			return 0x02;
		case '3':
			return 0x03;
		case '4':
			return 0x04;
		case '5':
			return 0x05;
		case '6':
			return 0x06;
		case '7':
			return 0x07;
		case '8':
			return 0x08;
		case '9':
			return 0x09;
		case 'A':
			return 0x0A;
		case 'B':
			return 0x0B;
		case 'C':
			return 0x0C;
		case 'D':
			return 0x0D;
		case 'E':
			return 0x0E;
		case 'F':
			return 0x0F;
		default:
			return 0x00;
	}
}

void initializePorts()
{
	DDRD = 0xFF;
	PORTD = 0x00;
	DDRC = 0xFF;
	PORTC = 0x00;
}

void initializeUSART()
{
	UCSR0B = (1 << RXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	UBRR0 = F_CPU / BAUDRATE / 16 - 1;
}

void sendSoftUSART(char symbol)
{
	uint8_t i = 0x01;
	SET_0_SOFT;
	_delay_us(17.36);
	while (i > 0)
	{
		if (symbol & i)
		{
			SET_1_SOFT;
		}
		else
		{
			SET_0_SOFT;
		}
		i <<= 1;
		_delay_us(17.36);
	}
	SET_1_SOFT;
	_delay_us(17.36);
}

void sendStringSoftUSART(char *buffer)
{
	while(*buffer != 0)
	{
		sendSoftUSART(*buffer++);
	}
}