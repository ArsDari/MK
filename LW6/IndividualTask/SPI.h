#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

#define CLK PINB5
#define DAT PINB3
#define SS PINB2
#define LAT PINB1

extern uint8_t segments[];
extern volatile uint32_t buffer7Seg;

void initializeSPI();
void transmitSPI(uint8_t);
void fillBuffer(uint16_t);
void send32(uint32_t);

#endif