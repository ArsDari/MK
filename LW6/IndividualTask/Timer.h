#ifndef TIMER_H_
#define TIMER_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>

#define INCREMENT_DELAY 100
#define TIMER_INCREMENT_DELAY F_CPU / 1024 * INCREMENT_DELAY / 1000
#define PRESCALE_1024 (1 << CS12) | (1 << CS10)

void initializeTimer();

#endif