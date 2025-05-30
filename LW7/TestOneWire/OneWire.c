#include "OneWire.h"

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