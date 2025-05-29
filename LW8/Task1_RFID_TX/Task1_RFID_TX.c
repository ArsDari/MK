/*#define F_CPU 16000000UL
#define BAUDRATE 9600
#include <avr/io.h>
#include <util/delay.h>
#define CARD_DATA_SIZE 14
#define BASE_DELAY 100
#define LED_ERR 5
#define LED0 6
#define LED1 7
#define LEDS_OFF() PORTD &= ~((1 << LED1) | (1 << LED0) | (1 << LED_ERR))
//Массив данных для 4 карт
volatile char cardData[56] =
{
    0x02, 0x30, 0x37, 0x30, 0x30, 0x35, 0x34, 0x33, 0x37, 0x41,
    0x39, 0x43, 0x44, 0x03,
    0x02, 0x30, 0x37, 0x30, 0x30, 0x35, 0x34, 0x33, 0x37, 0x41,
    0x37, 0x43, 0x33, 0x03,
    0x02, 0x30, 0x37, 0x30, 0x30, 0x35, 0x34, 0x33, 0x37, 0x41,
    0x35, 0x43, 0x31, 0x03,
    0x02, 0x30, 0x37, 0x30, 0x30, 0x35, 0x34, 0x33, 0x37, 0x41,
    0x34, 0x43, 0x37, 0x03
};

void InitPorts()
{
    DDRD = 0xFF;
    PORTD = 0x00;
}

void InitUart()
{
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01 | 1 << UCSZ00);
    UBRR0H = 0;
    UBRR0L = F_CPU / BAUDRATE / 16 - 1;
}
void SendChar(char symbol)
{
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = symbol;
}

void SendPacket(char *dat)
{
    int i = 0;
    while(i < CARD_DATA_SIZE)
        SendChar(dat[i++]);
}

void Leds_On(uint8_t value)
{
    PORTD |= (value >> 1) << LED1;
    PORTD |= (value & 0x01) << LED0;
}

void Delay_Func(uint8_t iteration)
{
    //Функция необходима для формирования изменяемой задержки
    //так как _delay_ms() не принимает изменяемые переменные в качестве параметра
    int i = 0;
    while(i++ < iteration)
        _delay_ms(BASE_DELAY);
}

int main(void)
{
    InitPorts(); //Инициализация портов
    InitUart(); //Инициализация UART
    uint8_t cardIdx = 0;
    int shift = 0;
    uint8_t iterations = 0;
    uint8_t doError = 0;
    char sendData[CARD_DATA_SIZE];
    while (1)
    {
        LEDS_OFF(); //Вызов макроса выключения светодиодов
        cardIdx = rand() & 0x03; //Выбор случайной карты
        Leds_On(cardIdx); //Индикация номера кадра
        //Формирование кадра для отправки
        shift = cardIdx * CARD_DATA_SIZE;
        for (int i = 0; i < CARD_DATA_SIZE; i++)
            sendData[i] = cardData[i + shift];
        //Внесение ошибки в кадр
        doError = rand() & 0x01; //
        if(doError) sendData[1] = 0x35;
        PORTD |= doError << LED_ERR; //Индикация наличия ошибки
        //Отправка данных
        SendPacket(sendData);
        //Случайная задержка отправки сигнала
        //от 100мс до 1.7 сек
        iterations = rand() & 0x0F;
        Delay_Func(++iterations);
    }
}*/

#define F_CPU 16000000UL
#define BAUDRATE 9600
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#define CARD_DATA_SIZE 14
#define BASE_DELAY 100
#define LED_ERR 5
#define LED0 6
#define LED1 7
#define LEDS_OFF() PORTD &= ~((1 << LED1) | (1 << LED0) | (1 << LED_ERR))
//Массив данных для 4 карт
volatile char cardData[56] =
{
    0x02, 0x30, 0x37, 0x30, 0x30, 0x35, 0x34, 0x33, 0x37, 0x41,
    0x39, 0x43, 0x44, 0x03,
    0x02, 0x30, 0x37, 0x30, 0x30, 0x35, 0x34, 0x33, 0x37, 0x41,
    0x37, 0x43, 0x33, 0x03,
    0x02, 0x30, 0x37, 0x30, 0x30, 0x35, 0x34, 0x33, 0x37, 0x41,
    0x35, 0x43, 0x31, 0x03,
    0x02, 0x30, 0x37, 0x30, 0x30, 0x35, 0x34, 0x33, 0x37, 0x41,
    0x34, 0x43, 0x37, 0x03
};

void InitPorts()
{
    DDRD = 0xFF;
    PORTD = 0x00;
}

void InitUart()
{
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01 | 1 << UCSZ00);
    UBRR0H = 0;
    UBRR0L = F_CPU / BAUDRATE / 16 - 1;
}

void SendChar(char symbol)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = symbol;
}

void SendPacket(char *dat)
{
    int i = 0;
    while (i < CARD_DATA_SIZE)
	{
        SendChar(dat[i++]);
	}
}

void Leds_On(uint8_t value)
{
    PORTD |= (value >> 1) << LED1;
    PORTD |= (value & 0x01) << LED0;
}

void Delay_Func(uint8_t iteration)
{
    //Функция необходима для формирования изменяемой задержки
    //так как _delay_ms() не принимает изменяемые переменные в качестве параметра
    int i = 0;
    while(i++ < iteration)
	{
        _delay_ms(BASE_DELAY);
	}
}

int main(void)
{
    InitPorts(); //Инициализация портов
    InitUart(); //Инициализация UART
    uint8_t cardIdx = 0;
    int shift = 0;
    uint8_t iterations = 0;
    uint8_t doError = 0;
    char sendData[CARD_DATA_SIZE];
    while (1)
    {
        LEDS_OFF(); //Вызов макроса выключения светодиодов
        cardIdx = rand() & 0x03; //Выбор случайной карты
        Leds_On(cardIdx); //Индикация номера кадра
        //Формирование кадра для отправки
        shift = cardIdx * CARD_DATA_SIZE;
        for (int i = 0; i < CARD_DATA_SIZE; i++)
		{
            sendData[i] = cardData[i + shift];
		}
		//Внесение ошибки в кадр
        doError = rand() & 0x01; //
        if (doError)
        {
			sendData[1] = 0x35;
		}
		PORTD |= doError << LED_ERR; //	наличия ошибки
		//Отправка данных
        SendPacket(sendData);
        //Случайная задержка отправки сигнала
        //от 100мс до 1.7 сек
        iterations = rand() & 0x0F;
        Delay_Func(++iterations);
    }
}