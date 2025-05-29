#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include "ADC.h"
#include "SPI.h"
#include "USART.h"
#include "LCD.h"

#define COMPARE_EQUAL 0
#define CONSOLE_INVALID_ARGUMENT_IN_COMMAND 3
#define CONSOLE_INVALID_ARGUMENT 2
#define CONSOLE_UNKNOWN_COMMAND 1
#define CONSOLE_SUCCESS 0
#define CONSOLE_BUSY 1
#define CONSOLE_STANDBY 0

#define ASCII_SYMBOL_START 0x30
#define BACKSPACE_SYMBOL 0x08
#define WITH_NULL_SYMBOL 1

extern volatile uint8_t stateConsole;
extern volatile char consoleBufferUSART[MAX_BUFFER_SIZE];
extern volatile uint8_t consoleBufferCurrentElementIndex;
extern volatile uint8_t consoleBufferLastElementIndex;

uint8_t consoleHandler();

#endif