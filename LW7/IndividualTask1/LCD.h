#ifndef LCD_H_
#define LCD_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <util/delay.h>

#define DDR_LCD_CONTROL DDRD
#define PORT_LCD_CONTROL PORTD
#define RS PORTD7
#define RW PORTD6
#define E PORTD5

#define DDR_LCD_DATA DDRC
#define PORT_LCD_DATA PORTC
#define D7 PORTC5
#define D6 PORTC4
#define D5 PORTC3
#define D4 PORTC2

#define LCD_FIRST_LINE 0x00
#define LCD_SECOND_LINE 0x40

#define LCD_CLEAR 0x01
#define LCD_RETURN_CARRIAGE 0x02
#define LCD_ENTRY_MODE 0x06
#define LCD_DISPLAY_OFF 0x08
#define LCD_DISPLAY_ON 0x0C // cursor off, don't blink character
#define LCD_INIT_VALUE 0x30
#define LCD_SET_4_BIT 0x28 // 4-bit data, 2-line display, 5 x 7 font
#define LCD_SET_CURSOR 0x80

void initializeLCD();
void sendHalf_LCD(uint8_t);
void sendInstruction_LCD(uint8_t);
void send_LCD(char);
void sendString_LCD(char *);

#endif