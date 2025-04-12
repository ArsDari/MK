#ifndef CONFIGADC_H_
#define CONFIGADC_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define DELAY 300
#define TICKS_TIM1A F_CPU / 1024 * DELAY / 1000
#define PRESCALE_T1 (1 << CS12) | (1 << CS10)

#define CLK PINB5
#define DAT PINB3
#define LAT PINB1

void InitTimer();
void SendByte(uint8_t data);
void SendWord(uint16_t data);
void Send32(uint32_t data);

void SPI_MasterInit();
void SPI_MasterTransmit(uint8_t data);

void USART_Init();
void USART_TX_Byte(uint8_t data);
void USART_SendString(uint8_t *str);

void InitADC();

#endif