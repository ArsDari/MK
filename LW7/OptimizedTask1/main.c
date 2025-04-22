#include "LCD.h"
#include "OneWire.h"
#include "USART.h"
#include <avr/interrupt.h>

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
    LCD_Write(DATA, 'T');
    LCD_Write(DATA, '=');
    LCD_Write(DATA, 0x20);
    LCD_Write(CMD, 0x40 | 0x80);
    LCD_Write(DATA, 'V');
    LCD_Write(DATA, 'a');
    LCD_Write(DATA, 'l');
    LCD_Write(DATA, 'u');
    LCD_Write(DATA, 'e');
    LCD_Write(DATA, '=');
    LCD_Write(DATA, 0x20);
    OneWire_Init();
    OneWire_SendByte(0xCC); // Skip ROM
    OneWire_SendByte(0x4E); // Write scratchpad
    OneWire_SendByte(0x00); // TH
    OneWire_SendByte(0x00); // TL
    OneWire_SendByte(0x3F); // config
    while(1)
    {
        Bin2Dec(ADC_val);
        LCD_Write(CMD, 0x47 | 0x80);
        LCD_Write(DATA, 0x30 + bcd_buffer[3]);
        LCD_Write(DATA, 0x30 + bcd_buffer[2]);
        LCD_Write(DATA, 0x30 + bcd_buffer[1]);
        LCD_Write(DATA, 0x30 + bcd_buffer[0]);
        OneWire_Init();
        OneWire_SendByte(0xCC); // Skip ROM
        OneWire_SendByte(0x44); // Convert T
        while (OneWire_Read_1_0() == 0);
        OneWire_Init();
        OneWire_SendByte(0xCC); // Skip ROM
        OneWire_SendByte(0xBE); // Read Scratchpad
        temperature = 0x00FF & OneWire_ReadByte();
        temperature |= OneWire_ReadByte() << 8;
        LCD_Write(CMD, 0x03 | 0x80);
        LCD_Write(DATA, char_tab[(temperature >> 12) & 0x0F]);
        LCD_Write(DATA, char_tab[(temperature >> 8) & 0x0F]);
        LCD_Write(DATA, char_tab[(temperature >> 4) & 0x0F]);
        LCD_Write(DATA, char_tab[(temperature >> 0) & 0x0F]);
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
        SendString("Roger that\r\n");
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