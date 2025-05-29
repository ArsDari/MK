#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#define DATA 1
#define WIRE PIND4
#define WIRE_PIN PIND

#define SET_WIRE_IN DDRD |= (1 << WIRE)
#define SET_WIRE_OUT DDRD &= ~(1 << WIRE)

#define SET_WIRE_1 PORTD |= (1<<WIRE)
#define SET_WIRE_0 PORTD &= ~(1<<WIRE)

void initializeOneWire();
uint8_t readBitOneWire();
uint8_t readByteOneWire();
void sendBitOneWire(uint8_t);
void sendByteOneWire(uint8_t);

#endif