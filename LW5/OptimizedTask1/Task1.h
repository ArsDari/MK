#ifndef TASK1_H_
#define TASK1_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DELAY 300
#define TICKS_TIM1A F_CPU / 1024 * DELAY / 1000
#define PRESCALE_T1 (1 << CS12) | (1 << CS10)
#define CLK PINB5
#define DAT PINB3
#define LAT PINB1

void initializePorts();
void initializeTimer();
void sendByte(uint8_t data);
void sendWord(uint16_t data);
void send32(uint32_t data);

uint8_t segments[] = {
	0b00111111,
	0b00000110,
	0b01011011,
	0b01001111,
	0b01100110,
	0b01101101,
	0b01111101,
	0b00000111,
	0b01111111,
	0b01101111,
};

volatile uint16_t flagTIM1A = 0;

#endif