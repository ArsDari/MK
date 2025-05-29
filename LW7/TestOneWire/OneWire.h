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
#define WIRE_PORT PORTD

#define SET_WIRE_IN WIRE_PIN |= (1 << WIRE)
#define SET_WIRE_OUT WIRE_PIN &= ~(1 << WIRE)

#define SET_WIRE_1 WIRE_PORT |= (1 << WIRE)
#define SET_WIRE_0 WIRE_PORT &= ~(1 << WIRE)

void initializeOneWire();
uint8_t readBitOneWire();
uint8_t readByteOneWire();
void sendBitOneWire(uint8_t);
void sendByteOneWire(uint8_t);

#endif