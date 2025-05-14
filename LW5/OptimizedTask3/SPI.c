#include "SPI.h"
#include <avr/io.h>

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

void send32(uint32_t data)
{
	PORTB &= ~(1 << LAT);
	transmitSPI(data >> 24);
	transmitSPI(data >> 16);
	transmitSPI(data >> 8);
	transmitSPI(data);
	PORTB |= (1 << LAT);
}