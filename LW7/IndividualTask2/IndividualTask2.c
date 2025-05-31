#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADC.h"
#include "Console.h"
#include "SPI.h"
#include "Timer.h"
#include "USART.h"
#include "LCD.h"
#include "OneWire.h"

void initializePorts();
void matchROM(const char[]);
void formatTemperatureValue(uint16_t);

volatile uint8_t allowUpdate = 0;
const char firstSensor[] = {0x28, 0xF1, 0x6E, 0x44, 0xD4, 0xDE, 0x2A, 0x58};
const char secondSensor[] = {0x28, 0xC7, 0x38, 0x45, 0xD4, 0x86, 0x15, 0x7F};
const char symbols[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
char temperatureResponse[MAX_BUFFER_SIZE];

int main(void)
{
	initializePorts();
	initializeTimer();
	initializeSPI();
	initializeADC();
	initializeUSART();
	initializeLCD();
	initializeOneWire();
	matchROM(firstSensor);
	sendOneWire(WRITE_SCRATCHPAD);
	sendOneWire(0);
	sendOneWire(0);
	sendOneWire(0x3F);
	initializeOneWire();
	matchROM(secondSensor);
	sendOneWire(WRITE_SCRATCHPAD);
	sendOneWire(0);
	sendOneWire(0);
	sendOneWire(0x7F);
	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC01);
	sei();
	fillBuffer(0);
	send32(buffer7Seg);
	pushString("USART Check - OK!\r\n");
	sendString_LCD("LCD Check - OK!");
	while (1)
	{
		switch (stateUSART)
		{
			case USART_RECEIVED:
			{
				stateUSART = USART_STANDBY;
				stateConsole = CONSOLE_BUSY;
				if (consoleHandler() != CONSOLE_SUCCESS)
				{
					pushString("Parse Failure! ");
					switch (consoleHandler())
					{
						case CONSOLE_UNKNOWN_COMMAND:
						{
							pushString("Unknown command.\r\n");
							break;
						}
						case CONSOLE_INVALID_ARGUMENT:
						{
							pushString("Invalid argument.\r\n");
							break;
						}
						case CONSOLE_INVALID_ARGUMENT_IN_COMMAND:
						{
							pushString("Invalid argument in passed command.\r\n");
							break;
						}
						default:
						{
							pushString("Unexpected error.\r\n");
							break;
						}
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
		if (allowUpdate)
		{
			char tempString[32];
			tempString = "1: ";
			allowUpdate = 0;
			initializeOneWire();
			sendOneWire(SKIP_ROM);
			sendOneWire(CONVERT_TEMPERATURE);
			_delay_ms(825);
			initializeOneWire();
			matchROM(firstSensor);
			sendOneWire(READ_SCRATCHPAD);
			uint16_t unparsedTemperature = readOneWire();
			unparsedTemperature |= readOneWire() << 8;
			formatTemperatureValue(unparsedTemperature);
			memcpy(tempString[3], temperatureValue, 8);
			temperatureValue[]
			pushString("Temperature of first sensor: ");
			pushString((char *)(temperatureValue));
			pushString("\r\n");
			initializeOneWire();
			matchROM(secondSensor);
			sendOneWire(READ_SCRATCHPAD);
			unparsedTemperature = readOneWire();
			unparsedTemperature |= readOneWire() << 8;
			formatTemperatureValue(unparsedTemperature);
			pushString("Temperature of second sensor: ");
			pushString((char *)(temperatureValue));
			pushString("\r\n");
			sendString_LCD();
		}
	}
}

ISR(INT0_vect)
{
	allowUpdate = 1;
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
		if (received == BACKSPACE_SYMBOL && inputLastElementIndex > 0)
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
	PORTB = 0;
	DDRD = ((1 << RS) | (1 << RW) | (1 << E)) & ~(1 << PIND2);
	PORTD = (1 << PIND2);
	DDRC |= (1 << D7) | (1 << D6) | (1 << D5) | (1 << D4);
	PORTC &= ~((1 << D7) | (1 << D6) | (1 << D5) | (1 << D4));
}