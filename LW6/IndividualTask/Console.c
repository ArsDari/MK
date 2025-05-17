#include "Console.h"

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
	else if (!strncmp((const char *)(consoleBufferUSART), "/get_register ", 14))
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
	else if (!strncmp((const char*)(consoleBufferUSART), "/sendToSPI ", 11) == COMPARE_EQUAL)
	{
		if (strncmp((const char *)(&consoleBufferUSART[11]), "ADC", 3 + WITH_NULL_SYMBOL) == COMPARE_EQUAL)
		{
			fillBuffer(ADCValue);
			send32(buffer7Seg);
			pushString("Sent ADC value via SPI.\r\n");
		}
		else if (strncmp((const char *)(&consoleBufferUSART[11]), "VALUE ", 6) == COMPARE_EQUAL)
		{
			if (consoleBufferUSART[21 + WITH_NULL_SYMBOL] == '\0')
			{
				uint16_t frame = 0;
				for (uint8_t i = 0; i < 4; i++)
				{
					if ('0' <= consoleBufferUSART[17 + i] && consoleBufferUSART[17 + i] <= '9')
					{
						frame = frame * 10 + (consoleBufferUSART[17 + i] - ASCII_SYMBOL_START);
					}
					else
					{
						return CONSOLE_INVALID_ARGUMENT;
					}
				}
				fillBuffer(frame);
				send32(frame);
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