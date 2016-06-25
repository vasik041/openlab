/*=========================================================================*/
/*
 * Sample Gen
 * LZs,2015
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

#include "sgen.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>
#include <math.h>
#include "usart.h"

volatile uint8_t  sinel[256];
volatile uint8_t  sineh[256];
volatile uint16_t phase;
volatile uint16_t step;

/*=========================================================================*/

ISR (TIMER1_OVF_vect)
{
uint8_t x = (phase >> 8);
	DACH = sineh[x];
	DACL = sinel[x];
	phase += step;
}

/*=========================================================================*/

void send_ok(uint8_t b)
{
	USART_Transmit('o');
	USART_Transmit('k');
	thex(b);
	USART_Transmit('\n');
}

// hhll
void set_freq()
{
uint8_t h;
uint8_t l;
	h = rhex();
	if(USART_Timeout()) return;
	l = rhex();
	if(USART_Timeout()) return;
	step = l + (h << 8);
	send_ok(l^h);
}


// 256 x dd
void set_sig()
{
uint16_t i,y;
uint8_t x;
uint8_t bh,bl;
	x = 0;
	for(i=0; i < 256; i++) {
		bh = rhex();
		if(USART_Timeout()) return;
		bl = rhex();
		if(USART_Timeout()) return;
		y = 2048 - (bl + (bh << 8));
		sinel[i] = LO6(y);
		sineh[i] = HI6(y);
		x ^= bl;
		x ^= bh;
	}
	send_ok(x);
}

/*=========================================================================*/

int main(void)
{
uint8_t c;
uint16_t cnt=0;

	DDRC = 0x3f;	// portc out, DAC0
	PORTC = 0;
	DDRB = 0x3f;	// portb out, DAC1
	PORTB = 0;
	DDRD = 0x82;	// LED, TXD

	USART_Init(BR_9600);

	TCCR1A = 0x02;  // fast PWM (14)
	TCCR1B = 0x18+1;// clk x1
	ICR1 = 305-1;	// top
	OCR1A = 1;	// value
	OCR1B = 1;
	TIMSK1 = 1;	// TOIE1

	DACL = 32;
	DACH = 32;

	// tre 0.5 max amp
	for(cnt=0; cnt < 128; cnt++) {
		int16_t y = 1024+(cnt << 4);
		sinel[cnt] = LO6(y);
		sineh[cnt] = HI6(y);
		y = 2048 - (cnt << 4) + 1024;
		sinel[cnt+128] = LO6(y);
		sineh[cnt+128] = HI6(y);
	}

//	saw max amp
//	for(cnt=0; cnt < 256; cnt++) {
//		int16_t y = cnt << 4;
//		sinel[cnt] = LO6(y);
//		sineh[cnt] = HI6(y);
//	}

	phase = 0;
	step = 100;
	sei();

	for( ; ; ) {
		if(USART_Ready()) {
			c = USART_Receive(STD_TIMEOUT);
			switch(c) {
				case 't': set_freq(); break;
				case 's': set_sig();  break;
			}
		}

		if(++cnt > 30000) {
			PORTD = PIND ^ 0x80; // blink
			cnt = 0;
		}
	}

	return 0;
}

/*=========================================================================*/


