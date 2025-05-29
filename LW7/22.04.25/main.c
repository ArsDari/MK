#include <avr/interrupt.h>
#include <string.h>
#include "LCD.h"
#include "OneWire.h"
#include "USART.h"

void InitPorts(void);
void InitTimer1(void);
void Bin2Dec(uint16_t data);
void DisplayData (uint16_t data);
void InitADC(void);
/* Переместить с порта Б в порт С
 Сделать это на железе
 Для оптимизации кода посмотреть значение в LCD Write, и переписать её
 Убрать в прерываниях отправку байтов. НООООО СНАЧАЛА Запустить на плате И ПОМЕНЯТЬ F_CPU на 16000000 */
volatile uint8_t bcd_buffer[] = {0, 0, 0, 0};
volatile uint16_t ADC_val, temperature = 0;
const char char_tab[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

int main(void)
{
    InitPorts();
    InitTimer1();
    EIMSK |= (1 << INT0);
    EICRA |= (1 << ISC01);
    InitADC();
    InitUSART();
    InitLCD();
    sei();
    SendString("Hello\r\n");
	
	LCD_SetPosition(0, 0);
	LCD_SendString("T= ");

    LCD_SetPosition(0, 1);
	LCD_SendString("Value= ");

    OneWire_Init();
    OneWire_SendByte(0xCC); // Skip ROM
    OneWire_SendByte(0x4E); // Write scratchpad
    OneWire_SendByte(0x00); // TH
    OneWire_SendByte(0x00); // TL
    OneWire_SendByte(0x3F); // config
	
	char S_Buf[S_BUF_SIZE];
	uint8_t LCD_Buf[LCD_BUF_SIZE];
	
    while(1)
    {
		switch (rxState)
		case RX_COMPLETED
		{
			if (line)
				p_Buf = &LCD_Buf[i];
			else
				p_Buf = LCD_Buf;
			
			uint8_t i = 0;
			while (rx_Buf[i] != 0)
			{
				p_Buf++ = rxBuf[i];
				i++;
			}
			
			rxState = RX_IDLE;
			LCD_SetPosition(0, line);
			LCD_SendBuffer(pBuf, 16);
		}
			memcpy(LCD_Buf, rx_Buf, rx_Idx);
			rx_State = RX_IDLE;
			LCD_SetPosition(0, 0);
			LCD_SendBuffer(LCD_Buf, )
			
			/* задача переделать SendBuffer вместо присваивания по индексу убрать их вовсе / line равно 1, то смещаем убрать lcdbuf вообщем надо сделать дохуя и больше;;;; как сделать так, чтобы старые строчки поднимались наверх 
			
			Скопиравать код Пети, и решить дз: сделать так чтобы первоначальное значение появлялось сверху а потом все прхиодящие уходили вниз*/
        Bin2Dec(ADC_val);
		LCD_SetPosition(7, 1);
		sbuf[0] = 0x30 + bcd_buffer[3];
		sbuf[1] = 0x30 + bcd_buffer[2];
		sbuf[2] = 0x30 + bcd_buffer[1];
		sbuf[3] = 0x30 + bcd_buffer[0];
		sbuf[4] = 0;
		LCD_SendString(sbuf);
        OneWire_Init();
        OneWire_SendByte(0xCC); // Skip ROM
        OneWire_SendByte(0x44); // Convert T
        while (OneWire_Read_1_0() == 0);
        OneWire_Init();
        OneWire_SendByte(0xCC); // Skip ROM
        OneWire_SendByte(0xBE); // Read Scratchpad
        temperature = 0x00FF & OneWire_ReadByte();
        temperature |= OneWire_ReadByte() << 8;
        //LCD_Write(CMD, 0x03 | 0x80);
        //LCD_Write(DATA, char_tab[(temperature >> 12) & 0x0F]);
        //LCD_Write(DATA, char_tab[(temperature >> 8) & 0x0F]);
        //LCD_Write(DATA, char_tab[(temperature >> 4) & 0x0F]);
        //LCD_Write(DATA, char_tab[(temperature >> 0) & 0x0F]);
    }
}

//--------------------------------------------

ISR(TIMER1_COMPB_vect)
{
    //DisplayData(0x1E61);
}

ISR(INT0_vect)
{
    Bin2Dec(ADC_val);
    SendString("Value = ");
    SendChar(0x30 + bcd_buffer[3]);
    SendChar(0x30 + bcd_buffer[2]);
    SendChar(0x30 + bcd_buffer[1]);
    SendChar(0x30 + bcd_buffer[0]);
    SendString("\r\n");
}

ISR(ADC_vect)
{
    ADC_val = ADC;
}

ISR(USART_RX_vect)
{
    if(UDR0 == 0x20)
    {
		LCD_SetPosition(0, 0);
        LCD_SendString("Roger that\r\n");
    }
}

//--------------------------------------------

void InitPorts(void)
{
    DDRB = 0xFF; //настройка выводов управления дисплеем
    PORTB = 0;
    DDRD = (0 << PIND2 | 1 << PIND4); //настройка вывода кнопки
    PORTD |= (1 << PIND2 | 1 << PIND4);
}

void InitTimer1(void)
{
    TCCR1A = 0;
    TCCR1B = (1 << CS11 | 1 << CS10 | 1 << WGM12);
    TCNT1 = 0;
    TIMSK1 |= (1 << OCIE1B);
    OCR1A = 12500;
    OCR1B = 12500;
}

void Bin2Dec(uint16_t data)
{
    bcd_buffer[3] = (uint8_t)(data / 1000);
    data = data % 1000;
    bcd_buffer[2] = (uint8_t)(data / 100);
    data = data % 100;
    bcd_buffer[1] = (uint8_t)(data / 10);
    data = data % 10;
    bcd_buffer[0] = (uint8_t)(data);
}

void DisplayData(uint16_t data)
{
    Bin2Dec(data);
}

void InitADC(void)
{
    ADMUX = (1 << MUX0);
    //Align left, ADC1
    ADCSRB = (1 << ADTS2 | 1 << ADTS0); //Start on Timer1 COMPB
    //Enable, auto update, IRQ enable
    ADCSRA = (1 << ADEN | 1 << ADATE | 1 << ADIE);
}