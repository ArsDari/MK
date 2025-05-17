#ifndef LCD_H_
#define LCD_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#define D4 PINB0
#define D5 PINB1
#define D6 PINB2
#define D7 PINB3
#define RS PINB5
#define RW PINB6
#define E PINB7
#define CMD 0
#define DATA 1

void initializeLCD(void);
void LCD_Write(uint8_t type, char data);
char LCD_Read(void);
void LCD_SendString(char *str);
void LCD_SendBuffer(uint8_t *p, uint8_t count);
void LCD_SetPosition(uint8_t x, uint8_t y);

#endif