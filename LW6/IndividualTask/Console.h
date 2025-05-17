#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <string.h>

#define ASCII_SYMBOL_START 0x30
#define WITH_NULL_SYMBOL 1
#define COMPARE_EQUAL 0

#define CONSOLE_INVALID_ARGUMENT 2
#define CONSOLE_UNKNOWN_COMMAND 1
#define CONSOLE_SUCCESS 0
#define CONSOLE_BUSY 1
#define CONSOLE_STANDBY 0

uint8_t consoleHandler();

#endif