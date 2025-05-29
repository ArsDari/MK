#include "OneWire.h"

void initializeOneWire()
{
	DDRD |= (1 << WIRE);
	SET_0_WIRE;
	_delay_us(500);
	SET_1_WIRE;
	DDRD &= ~(1 << WIRE);
	_delay_us(60);
	if ((PIND & 1 << WIRE) == 0)
	{
		while ((PIND & 1 << WIRE) == 0);
		_delay_us(420);
	}
}

uint8_t readBitOneWire()
{
	uint8_t bit = 0;
	_delay_us(2);
	DDRD |= (1 << WIRE);
	SET_0_WIRE;
	_delay_us(2);
	SET_1_WIRE;
	DDRD &= ~(1 << WIRE);
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

uint8_t readByteOneWire()
{
	uint8_t retval = 0;
	for (uint8_t i = 0; i < 8; i++)
	{
		retval |= readBitOneWire() << i;
	}
	return retval;
}

void sendBitOneWire(uint8_t bit)
{
	_delay_us(2);
	DDRD |= (1 << WIRE);
	SET_0_WIRE;
	_delay_us(10);
	if (bit != 0)
	{
		SET_1_WIRE;
		_delay_us(90);
	}
	else
	{
		SET_0_WIRE;
		_delay_us(90);
		SET_1_WIRE;
	}
}

void sendByteOneWire(uint8_t data)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		OneWire_Send_1_0(data & 0x01);
		data >>= 1;
	}
}