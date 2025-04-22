#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#define DATA 1
#define WIRE PIND4
#define PIN_WIRE PIND

#define SET_WIRE_IN DDRD |= (1 << WIRE)
#define SET_WIRE_OUT DDRD &= ~(1 << WIRE)

#define SET_1_WIRE PORTD |= (1<<WIRE)
#define SET_0_WIRE PORTD &= ~(1<<WIRE)

void OneWire_Init(void);
uint8_t OneWire_Read_1_0(void);
uint8_t OneWire_ReadByte(void);
void OneWire_Send_1_0(uint8_t);
void OneWire_SendByte(uint8_t);

#endif