#include "SPI.h"

uint8_t segments[] =
{
	0b00111111,
	0b00000110,
	0b01011011,
	0b01001111,
	0b01100110,
	0b01101101,
	0b01111101,
	0b00000111,
	0b01111111,
	0b01101111
};

volatile uint32_t buffer7Seg = 0;

void initializeSPI()
{
	SPCR = (1 << SPE) | (1 << MSTR) |(1 << SPR1) | (1 << SPR0);
	PORTB &= ~(1 << DAT) | (1 << CLK);
}

void transmitSPI(uint8_t data)
{
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
}

void fillBuffer(uint16_t value)
{
	uint8_t *p = (uint8_t *)&buffer7Seg;
	*p++ = ~segments[value % 10];
	*p++ = ~segments[(value / 10) % 10];
	*p++ = ~segments[(value / 100) % 10];
	*p = ~segments[value / 1000];
}

void send32(uint32_t data)
{
	PORTB &= ~(1 << LAT);
	transmitSPI(data);
	transmitSPI(data >> 8);
	transmitSPI(data >> 16);
	transmitSPI(data >> 24);
	PORTB |= (1 << LAT);
}