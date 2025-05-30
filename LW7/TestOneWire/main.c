#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "USART.h"
#include "OneWire.h"

#define ASCII_SYMBOL_START 0x30
#define BACKSPACE_SYMBOL 0x08

void initializePorts();

volatile uint8_t allowUpdate = 0;

int main(void)
{
	initializePorts();
	initializeUSART();
	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC01);
	sei();
	pushString("USART Check - OK!\r\n");
	initializeOneWire();
	sendOneWire(SKIP_ROM);
	sendOneWire(WRITE_SCRATCHPAD);
	sendOneWire(0);
	sendOneWire(0);
	sendOneWire(0x3F);
	char stringTemperature[4];
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
			while (readBitOneWire() == 0);
			initializeOneWire();
			sendOneWire(SKIP_ROM);
			sendOneWire(READ_TEMPERATURE);
			
			uint16_t temp = readOneWire();
			temp |= readOneWire() << 8;
			
			uint8_t beforePoint = 0;
			uint8_t afterPoint = 0;
			
			beforePoint = (temp & 0x0F);
			afterPoint = (temp >> 4) & 0x3F;
			
			//if (temp >> 11 & 0x01)
			//{
				//beforePoint = -beforePoint;
				//afterPoint = -afterPoint;
			//}
			
			uint16_t tempBeforePoint = 0;
			if ((beforePoint >> 3) & 0x01)
			{
				tempBeforePoint += 2;
			}
			if ((beforePoint >> 2) & 0x01)
			{
				tempBeforePoint += 2 * 2;
			}
			if ((beforePoint >> 1) & 0x01)
			{
				tempBeforePoint += 2 * 2 * 2;
			}
			if ((beforePoint >> 0) & 0x01)
			{
				tempBeforePoint += 2 * 2 * 2 * 2;
			}
			tempBeforePoint = 10000 / tempBeforePoint;
			
			sprintf(stringTemperature, "%u.%u", afterPoint, tempBeforePoint);
			pushString("Temperature: ");
			pushString(stringTemperature);
			//sprintf(stringTemperature, "%u  ", tempBeforePoint);
			//pushString(stringTemperature);
			//push(((beforePoint >> 3) & 0x01) + 0x30);
			//push(((beforePoint >> 2) & 0x01) + 0x30);
			//push(((beforePoint >> 1) & 0x01) + 0x30);
			//push(((beforePoint >> 0) & 0x01) + 0x30);
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