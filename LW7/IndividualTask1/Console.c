#include "Console.h"

volatile uint8_t stateConsole = CONSOLE_STANDBY;
volatile char consoleBufferUSART[MAX_BUFFER_SIZE];
volatile uint8_t consoleBufferCurrentElementIndex = 0;
volatile uint8_t consoleBufferLastElementIndex = 0;

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
					return CONSOLE_INVALID_ARGUMENT_IN_COMMAND;
				}
			}
			else
			{
				return CONSOLE_INVALID_ARGUMENT_IN_COMMAND;
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
						return CONSOLE_INVALID_ARGUMENT_IN_COMMAND;
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
	else if (strncmp((const char*)(consoleBufferUSART), "/send_to_LCD ", 13) == COMPARE_EQUAL)
	{
		sendInstruction_LCD(LCD_CLEAR);
		_delay_ms(2);
		char *currentChar = (char *)&consoleBufferUSART[13];
		uint8_t indexLCD = 0;
		while (indexLCD < 0x20)
		{
			if (*currentChar == '\0')
			{
				break;
			}
			else if (*currentChar == '\n')
			{
				if (indexLCD < 0x10)
				{
					sendInstruction_LCD(LCD_SET_CURSOR | LCD_SECOND_LINE);
					_delay_us(50);
					indexLCD = 0x10;
				}
				else
				{
					break;
				}
			}
			else if (indexLCD == 0x10)
			{
				sendInstruction_LCD(LCD_SET_CURSOR | LCD_SECOND_LINE);
				_delay_us(50);
				send_LCD(*currentChar);
				_delay_us(50);
				indexLCD = 0x11;
			}
			else
			{
				send_LCD(*currentChar);
				_delay_us(50);
				indexLCD++;
			}
			currentChar++;
		}
		pushString("Sent input value to LCD.\r\n");
	}
	else if (strncmp((const char*)(consoleBufferUSART), "/execute ", 9) == COMPARE_EQUAL)
	{
		if (consoleBufferUSART[10] == ' ' && consoleBufferUSART[12] == ' ' && consoleBufferUSART[14] == '\0')
		{
			int8_t leftOperand = consoleBufferUSART[9] - ASCII_SYMBOL_START;
			int8_t rightOperand = consoleBufferUSART[13] - ASCII_SYMBOL_START;
			char operation = consoleBufferUSART[11];
			char result[2];
			switch (operation)
			{
				case '+':
				{
					leftOperand += rightOperand;
					break;
				}
				case '-':
				{
					leftOperand -= rightOperand;
					break;
				}
				case '*':
				{
					leftOperand *= rightOperand;
					break;
				}
				case '/':
				{
					if (rightOperand == 0)
					{
						return CONSOLE_INVALID_ARGUMENT_IN_COMMAND;
					}
					leftOperand /= rightOperand;
					break;
				}
			}
			sendInstruction_LCD(LCD_CLEAR);
			_delay_ms(2);
			sprintf(result, "%d", leftOperand);
			sendString_LCD(result);
			_delay_us(50);
			pushString("Success!\r\n");
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