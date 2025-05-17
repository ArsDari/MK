#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#include "LCD.h"
#include "OneWire.h"
#include "USART.h"

#define INCREMENT_DELAY 100
#define TIMER_INCREMENT_DELAY F_CPU / 1024 * INCREMENT_DELAY / 1000
#define PRESCALE_1024 (1 << CS12) | (1 << CS10)

#define PRESCALE_ADC (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)

void initializePorts(void);
void initializeTimer(void);
void initializeADC(void);

void Bin2Dec(uint16_t data);

volatile char bufferUART[MAX_BUFFER_SIZE];
volatile uint8_t currentElementIndex = 0;
volatile uint8_t lastElementIndex = 0;
volatile uint8_t stateUART = UART_STANDBY;

volatile uint8_t bcd_buffer[] = {0, 0, 0, 0};
volatile uint16_t valueADC = 0;
volatile uint8_t temperature = 0;
const char char_tab[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

/* Переместить с порта Б в порт С
 Сделать это на железе
 Для оптимизации кода посмотреть значение в LCD Write, и переписать её
 Убрать в прерываниях отправку байтов. НООООО СНАЧАЛА Запустить на плате И ПОМЕНЯТЬ F_CPU на 16000000 */

int main(void)
{
    initializePorts();
    initializeTimer();
    initializeADC();
    initializeUSART();
    initializeLCD();
    EIMSK |= (1 << INT0);
    EICRA |= (1 << ISC01);
    sei();
	
    pushString("Hello\r\n");
	
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
	
	char sbuf[17];
	
    while(1)
    {
        Bin2Dec(valueADC);
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
    
}

ISR(INT0_vect)
{
    Bin2Dec(valueADC);
    pushString("Value = ");
    push(0x30 + bcd_buffer[3]);
    push(0x30 + bcd_buffer[2]);
    push(0x30 + bcd_buffer[1]);
    push(0x30 + bcd_buffer[0]);
    pushString("\r\n");
}

ISR(ADC_vect)
{
    valueADC = ADC;
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

void initializePorts(void)
{
    DDRB = 0xFF; //настройка выводов управления дисплеем
    PORTB = 0;
    DDRD = (0 << PIND2 | 1 << PIND4); //настройка вывода кнопки
    PORTD |= (1 << PIND2 | 1 << PIND4);
}

void initializeTimer(void)
{
    TCCR1A = 0;
    TCCR1B = PRESCALE_1024 | (1 << WGM12);
    TIMSK1 = (1 << OCIE1B);
    OCR1A = TIMER_INCREMENT_DELAY;
    OCR1B = TIMER_INCREMENT_DELAY;
}

void initializeADC(void)
{
    ADMUX = (1 << MUX0);
    ADCSRA = (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | PRESCALE_ADC;
    ADCSRB = (1 << ADTS2) |	(1 << ADTS0);
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