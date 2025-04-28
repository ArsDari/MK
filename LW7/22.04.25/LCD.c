#include "LCD.h"

void InitLCD(void)
{
    uint8_t BF = 0x80;
    _delay_ms(40);
    PORTB &= ~(1 << RS); // отправка полубайт поменять
    PORTB = (0x30 >> 4);
    PORTB |= (1 << E);
    asm("nop");
    asm("nop");
    asm("nop");
    PORTB &= ~(1 << E);
    PORTB = 0;
    _delay_ms(5);
    PORTB &= ~(1 << RS);
    PORTB = (0x30 >> 4);
    PORTB |= (1 << E);
    asm("nop");
    asm("nop");
    asm("nop");
    PORTB &= ~(1 << E);
    PORTB = 0;
    _delay_us(150);
    PORTB &= ~(1 << RS);
    PORTB = (0x30 >> 4);
    PORTB |= (1 << E);
    asm("nop");
    asm("nop");
    asm("nop");
    PORTB &= ~(1 << E);
    PORTB = 0;
    _delay_ms(5);
    do
    {
        BF = (0x80 & LCD_Read());
    }
    while (BF == 0x80);
    PORTB &= ~(1 << RS);
    PORTB = (0x20 >> 4);
    PORTB |= (1 << E);
    asm("nop");
    asm("nop");
    asm("nop");
    PORTB &= ~(1 << E);
    PORTB = 0;
    do
    {
        BF = (0x80 & LCD_Read());
    }
    while (BF == 0x80);
    // заработал 4-проводной интерфейс
    LCD_Write(CMD, 0x28); //2 строки, 5*8
    LCD_Write(CMD, 0x0C); //display on, cursor on
    LCD_Write(CMD, 0x06); //cnt++, shift enabled
	
	SET_CFG_OUT;
	_delay_ms(15);
	USART
}

void LCD_Write(uint8_t type, char data)
{
    uint8_t BF = 0x80;
    do
    {
        BF = 0x80 & LCD_Read();
    }
    while (BF == 0x80);
    PORTB |= (type << RS);
    PORTB |= (1 << E);
    PORTB &= ~(0x0F);
    PORTB |= (0x0F & (data >> 4)); //старшая тетрада
    PORTB &= ~(1 << E);
    asm("nop");
    asm("nop");
    asm("nop");
    PORTB |= (1 << E);
    PORTB &= ~(0x0F);
    PORTB |= (0x0F & data); //младшая тетрада
    PORTB &= ~(1 << E);
    PORTB = 0;
}

char LCD_Read(void)
{
    char retval = 0;
    PORTB &= ~(1 << RS);
    PORTB |= (1 << RW);
    DDRB &= ~(1 << D4 | 1 << D5 | 1 << D6 | 1 << D7);
    PORTB |= (1 << E);
    asm("nop");
    asm("nop");
    retval = (PINB & 0x0F) << 4;
    PORTB &= ~(1 << E);
    asm("nop");
    asm("nop");
    asm("nop");
    PORTB |= (1 << E);
    asm("nop");
    asm("nop");
    retval |= (PINB & 0x0F);
    PORTB &= ~(1 << E);
    DDRB |= (1 << D4 | 1 << D5 | 1 << D6 | 1 << D7);
    PORTB = 0;
    return retval;
}

void LCD_SendString(char *str)
{
	while (*str != 0)
	{
		LCD_Write(DATA, *str++);
	}
}

void LCD_SetPosition(uint8_t x, uint8_t y)
{
	char address = (0x40 * y + x) | 0x80;
	LCD_Write(CMD, address);
}