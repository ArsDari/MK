#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "USART.h"
#include "OneWire.h"

#define ASCII_SYMBOL_START 0x30
#define BACKSPACE_SYMBOL 0x08

void initializePorts();

volatile uint8_t allowUpdate = 0;
const char firstSensor[] = {0x28, 0xBA, 0xF9, 0x36, 0x08, 0x00, 0x00, 0xE7};
const char secondSensor[] = {0x28, 0x31, 0xC5, 0xB8, 0x00, 0x00, 0x00, 0xB9};

int main(void)
{
	initializePorts();
	initializeUSART();
	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC01);
	sei();
	pushString("USART Check - OK!\r\n");
	
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
	
	while (1) 
    {
		switch (stateUSART)
		{
			case USART_RECEIVED:
			{
				stateUSART = USART_STANDBY;
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
		}
    }
}

ISR(INT0_vect)
{
	allowUpdate = 1;
}

ISR(USART_RX_vect)
{
	uint8_t received = UDR0;
	if (received == '\r')
	{
		inputUSART[inputLastElementIndex] = '\0';
		inputCurrentElementIndex = 0;
		inputLastElementIndex = 0;
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

void initializePorts()
{
	DDRD &= ~(1 << PIND2);
	PORTD |= (1 << PIND2);
}