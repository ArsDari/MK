#include "LCD.h"

void initializeLCD()
{
	_delay_ms(50); // 40 ms + 25%
	
	PORT_LCD_CONTROL &= ~(1 << RS);
	PORT_LCD_CONTROL &= ~(1 << E);
	
	sendHalf_LCD(LCD_INIT_VALUE);
	_delay_ms(5); // 4 ms + 25%
	
	sendHalf_LCD(LCD_INIT_VALUE);
	_delay_us(125); // 100 us + 25%
	
	sendHalf_LCD(LCD_INIT_VALUE);
	_delay_us(125); // 100 us + 25%

	sendHalf_LCD(LCD_SET_4_BIT);
	_delay_us(50); // 40 us + 25%
	
	sendInstruction_LCD(LCD_SET_4_BIT);
	_delay_us(50); // 40 us + 25%
	
	sendInstruction_LCD(LCD_DISPLAY_OFF);
	_delay_us(50); // 40 us + 25%
	
	sendInstruction_LCD(LCD_CLEAR);
	_delay_ms(2); // 1.64 ms + 25%
	
	sendInstruction_LCD(LCD_ENTRY_MODE);
	_delay_us(50); // 40 us + 25%
	
	sendInstruction_LCD(LCD_DISPLAY_ON);
	_delay_us(50); // 40 us + 25%
}

void sendHalf_LCD(uint8_t value)
{
	PORT_LCD_DATA &= ~((1 << D7) | (1 << D6) | (1 << D5) | (1 << D4));
	if (value & (1 << 7)) PORT_LCD_DATA |= (1 << D7);
	if (value & (1 << 6)) PORT_LCD_DATA |= (1 << D6);
	if (value & (1 << 5)) PORT_LCD_DATA |= (1 << D5);
	if (value & (1 << 4)) PORT_LCD_DATA |= (1 << D4);
	PORT_LCD_CONTROL |= (1 << E);
	_delay_us(1);
	PORT_LCD_CONTROL &= ~(1 << E);
	_delay_us(1);
}

void sendInstruction_LCD(uint8_t instruction)
{
	PORT_LCD_CONTROL &= ~(1 << RS);
	PORT_LCD_CONTROL &= ~(1 << E);
	sendHalf_LCD(instruction);
	sendHalf_LCD(instruction << 4);
}

void send_LCD(char value)
{
	PORT_LCD_CONTROL |= (1 << RS);
	PORT_LCD_CONTROL &= ~(1 << E);
	sendHalf_LCD(value);
	sendHalf_LCD(value << 4);
}

void sendString_LCD(char *string)
{
	while (*string != '\0')
	{
		send_LCD(*string++);
		_delay_us(50); // 40 us + 25%
	}
}