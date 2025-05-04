#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define INCREMENT_DELAY 300
#define TIMER_INCREMENT_DELAY F_CPU / 1024 * INCREMENT_DELAY / 1000
#define PRESCALE_1024 (1 << CS12) | (1 << CS10)

#define COUNTER_NORMAL_MODE 0
#define COUNTER_STOP 1
#define COUNTER_RESET 2

#define SHOW_ALLOWED 0
#define SHOW_DISALLOWED 1

#define CLK PINB5
#define DAT PINB3
#define LAT PINB1
#define OE PINB0
#define BTN PIND2

#define END_TRANSMISSION 0x80

uint8_t segments[] =
{
	0b00111111,
	0b00000110,
	0b01011011,
	0b01001111,
	0b01100110,
	0b01101101,
	0b01111101,
	0b00000111,
	0b01111111,
	0b01101111
};

void initializeCounter();
void startCounter();
void stopCounter();

//void send_data(uint8_t data, uint8_t ind);
//void Bin2Dec(uint16_t data);
//void SendData(uint8_t data);
void counterToSeg(uint16_t);
void sendByte(uint8_t);
void send32(uint32_t);

volatile uint8_t counterMode = COUNTER_STOP, showAllow = SHOW_ALLOWED;
volatile uint16_t counter = 0;
volatile uint32_t segBuffer = 0;

int main(void)
{
	initializeCounter();
	DDRB = (1 << OE) | (1 << LAT) | (1 << DAT) | (1 << CLK);
	PORTB &= ~(1 << OE);
	DDRD &= ~(1 << BTN);
	PORTD |= (1 << BTN);
	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC01);
	sei();
	uint8_t *p;
	send32(0);
	while (1)
	{
		if (showAllow == SHOW_ALLOWED)
		{
			showAllow = SHOW_DISALLOWED;
			p = (uint8_t*)&segBuffer;
			*p++ = segments[counter % 10];
			*p++ = segments[(counter / 10) % 10];
			*p++ = segments[(counter / 100) % 10];
			*p = segments[(counter / 1000) % 10];
			send32(segBuffer);
		}
	}
}

ISR(TIMER1_COMPA_vect)
{
	counter = counter < 10000 ? counter + 1 : 0;
	showAllow = SHOW_ALLOWED;
	OCR1A += TIMER_INCREMENT_DELAY;
}

ISR(INT0_vect)
{
	counterMode = counterMode < COUNTER_STOP ? counterMode + 1 : 0;
	switch (counterMode)
	{
		case COUNTER_NORMAL_MODE:
		{
			startCounter();
			break;
		}
		case COUNTER_STOP:
		{
			stopCounter();
			break;
		}
	}
}

void initializeCounter()
{
	OCR1A = TIMER_INCREMENT_DELAY;
	TCCR1B = PRESCALE_1024;
	TIMSK1 = (1 << OCIE1A);
}

void startCounter()
{
	counter = 0;
	TCNT1 = 0; // reset counter
	OCR1A = TIMER_INCREMENT_DELAY;
	TCCR1B = PRESCALE_1024; // prescaler on - counter on
}

void stopCounter()
{
	TCCR1B = 0; // no prescaler - no counter
}

void sendByte(uint8_t data)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		PORTB &= ~(1 << CLK);
		if (data & END_TRANSMISSION)
		PORTB |= (1 << DAT);
		else
		PORTB &= ~(1 << DAT);
		
		PORTB &= ~(1 << CLK);
		data <<= 1;
		PORTB |= (1 << CLK);
	}
}

void send32(uint32_t data)
{
	sendByte(data);
	sendByte(data >> 8);
	sendByte(data >> 16);
	sendByte(data >> 24);
	PORTB &= ~(1 << LAT);
	PORTB |= (1 << LAT);
	PORTB &= ~(1 << LAT);
}
//void Bin2Dec(uint16_t data)
//{
	//bcd_buffer[3] = (uint8_t)(data / 1000);
	//data = data % 1000;
	//bcd_buffer[2] = (uint8_t)(data / 100);
	//data = data % 100;
	//bcd_buffer[1] = (uint8_t)(data / 10);
	//data = data % 10;
	//bcd_buffer[0] = (uint8_t)(data);
//}
//
//void SendData(uint8_t data)
//{
	//for(uint8_t i = 0; i < 8; i++)
	//{
		//PORTB &= ~(1 << PINB5); //CLK low
		//if(0x80 & (data << i))
		//{
			//PORTB |= 1 << PINB3; //DAT high
		//}
		//else
		//{
			//PORTB &= ~(1 << PINB3); // DAT low
		//}
		//PORTB |= (1 << PINB5); // CLK high
	//}
//}
//
//void DisplayData(uint16_t data)
//{
	//Bin2Dec(data);
	//PORTB &= ~(1 << PINB1); //clk_out = 0
	//SendData(segments[bcd_buffer[0]]);
	//SendData(segments[bcd_buffer[1]]);
	//SendData(segments[bcd_buffer[2]]);
	//SendData(segments[bcd_buffer[3]]);
	//PORTB |= (1 << PINB1); //clk_out = 1
//}