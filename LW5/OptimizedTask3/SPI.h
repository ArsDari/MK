#ifndef SPI_H_
#define SPI_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>

#define CLK PINB5
#define DAT PINB3
#define SS PINB2
#define LAT PINB1
#define OE PINB0
#define BTN PIND2
#define END_TRANSMISSION 0x80

void initializeSPI();
void transmitSPI(uint8_t);
void send32(uint32_t);

#endif