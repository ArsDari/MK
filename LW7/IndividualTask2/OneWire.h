#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "USART.h"

#define DATA 1
#define WIRE PIND4
#define WIRE_DDR DDRD
#define WIRE_PORT PORTD

#define SET_WIRE_OUT WIRE_DDR |= (1 << WIRE)
#define SET_WIRE_IN WIRE_DDR &= ~(1 << WIRE)

#define SET_WIRE_1 WIRE_PORT |= (1 << WIRE)
#define SET_WIRE_0 WIRE_PORT &= ~(1 << WIRE)

#define READ_ROM 0x33
#define MATCH_ROM 0x55
#define SKIP_ROM 0xCC
#define ALARM_SEARCH 0xEC
#define CONVERT_TEMPERATURE 0x44
#define READ_SCRATCHPAD 0xBE
#define WRITE_SCRATCHPAD 0x4E
#define COPY_SCRATCHPAD 0x48
#define RECALL_SCRATCHPAD 0xB8
#define READ_POWER_SUPPLY 0xB4

extern volatile char temperatureValue[8];
extern const char symbols[];

void initializeOneWire();
void sendBitOneWire(uint8_t);
void sendOneWire(uint8_t);
uint8_t readBitOneWire();
uint8_t readOneWire();

void readROM();
void matchROM(const char[]);
void formatTemperatureValue(uint16_t);
void resetTemperatureValue();

#endif