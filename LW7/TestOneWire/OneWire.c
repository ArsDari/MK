#include "OneWire.h"

volatile char temperatureValue[8];
const char symbols[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void initializeOneWire()
{
    SET_WIRE_OUT; // шину на выход
    SET_WIRE_0; // установить 0
    _delay_us(500); // ждать не менее 480 мкс
    SET_WIRE_1;
    SET_WIRE_IN; // шину на вход
    _delay_us(60);
    if (!(PIND & (1 << WIRE))) // ожидается импульс сброса
    {
        while (!(PIND & (1 << WIRE)));
        _delay_us(420);
    }
	else
	{
		pushString("No response.");
	}
}

void sendBitOneWire(uint8_t bit)
{
    _delay_us(2);
    SET_WIRE_OUT;
    SET_WIRE_0;
    _delay_us(10);
    if (bit != 0)
    {
        SET_WIRE_1;
        _delay_us(90);
    }
    else
    {
        SET_WIRE_0;
        _delay_us(90);
        SET_WIRE_1;
    }
}

void sendOneWire(uint8_t data)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		sendBitOneWire(data & 0x01);
		data >>= 1;
	}
}

uint8_t readBitOneWire()
{
	uint8_t bit = 0;
    _delay_us(2);
    SET_WIRE_OUT;
    SET_WIRE_0;
    _delay_us(2);
    SET_WIRE_1;
    SET_WIRE_IN;
    _delay_us(2);
	bit = (PIND & (1 << WIRE)) ? 1 : 0;
    _delay_us(90);
    return bit;
}

uint8_t readOneWire()
{
	uint8_t value = 0;
	for (uint8_t i = 0; i < 8; i++)
	{
		value |= readBitOneWire() << i;
	}
	return value;
}

void readROM()
{
	pushString("Sensor ID: ");
	initializeOneWire();
	sendOneWire(READ_ROM);
	uint8_t temp = 0;
	for (uint8_t i = 0; i < 8; i++)
	{
		temp = readOneWire();
		uint8_t biggerPage = temp >> 4 & 0x0F;
		uint8_t lowerPage = temp & 0x0F;
		push(symbols[biggerPage]);
		push(symbols[lowerPage]);
		push(' ');
	}
	pushString("\r\n");
}

void matchROM(const char id[])
{
	sendOneWire(MATCH_ROM);
	for (uint8_t index = 0; index < 8; index++)
	{
		sendOneWire(id[index]);
	}
}

void formatTemperatureValue(uint16_t unparsedValue)
{
	int8_t integerPart = 0;
	uint8_t fractionalPart = 0;
	integerPart = (unparsedValue >> 4) & 0xFF;
	fractionalPart = unparsedValue & 0x0F;
	uint8_t denominator = 0;
	if (unparsedValue & 0x08)
	{
		denominator += 2 * 2 * 2;
	}
	if (unparsedValue & 0x04)
	{
		denominator += 2 * 2;
	}
	if (unparsedValue & 0x02)
	{
		denominator += 2;
	}
	if (unparsedValue & 0x01)
	{
		denominator += 1;
	}
	fractionalPart = (denominator * 100) / 16;
	sprintf((char *)(temperatureValue), "%i.%u", integerPart, fractionalPart);
}