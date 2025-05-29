#include "OneWire.h"

void OneWire_Init(void)
{
    SET_WIRE_OUT; // шину на выход
    SET_WIRE_0; // установить 0
    _delay_us(500); // ждать не менее 480 мкс
    SET_WIRE_1;
    SET_WIRE_IN; // шину на вход
    _delay_us(60);
    if ((PIND & (1 << WIRE)) == 0) // ожидается импульс сброса
    {
        while ((PIND & (1 << WIRE)) == 0);
        _delay_us(420);
    }
}

void OneWire_Send_1_0(uint8_t bit)
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

void OneWire_SendByte(uint8_t data)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		sendBitOneWire(data & 0x01);
		data >>= 1;
	}
}

uint8_t OneWire_Read_1_0(void)
{
    uint8_t bit = 0;
    _delay_us(2);
    SET_WIRE_OUT;
    SET_WIRE_0;
    _delay_us(2);
    SET_WIRE_1;
    SET_WIRE_OUT;
    _delay_us(2);
    if((PIND & 1 << WIRE) == 0)
    {
        bit = 0;
    }
    else
    {
        bit = 1;
    }
    _delay_us(90);
    return bit;
}

uint8_t OneWire_ReadByte(void)
{
	uint8_t retval = 0;
	for (uint8_t i = 0; i < 8; i++)
	{
		retval |= readBitOneWire() << i;
	}
	return retval;
}