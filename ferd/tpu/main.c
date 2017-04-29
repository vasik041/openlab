/****************************************************************************/
/*
 * TPU driver
 * LZs,2014
 */
/****************************************************************************/

#include "main.h"
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <util/delay.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <math.h>

volatile uint32_t tau;
volatile uint16_t duty;

//=========================================================================

void delay1()
{
	_NOP(); _NOP(); _NOP(); _NOP(); _NOP();
	_NOP(); _NOP(); _NOP(); _NOP(); _NOP();
	_NOP(); _NOP(); _NOP(); _NOP(); _NOP();
	_NOP(); _NOP(); _NOP(); _NOP(); _NOP();

	_NOP(); _NOP(); _NOP(); _NOP(); _NOP();
	_NOP(); _NOP(); _NOP(); _NOP(); _NOP();
	_NOP(); _NOP(); _NOP(); _NOP(); _NOP();
	_NOP(); _NOP(); _NOP(); _NOP(); _NOP();
}


void delay(uint8_t n)
{
	while(n--) {
		_WDR();
		delay1();
	}
}

//=========================================================================
// 1,8,64,256,1024
// 1 2 3  4   5

void setup_timer1()
{
uint8_t div = 1;
uint16_t t16 = 1;
uint64_t t1 = SCALE_1(tau);

	if(t1 < 0x10000l) {
		div = 1;
		t16 = (uint16_t)t1;
	} else if(t1 >= 0x10000l && t1 < 0x80000l) {
		div = 2;
		t16 = (uint16_t)(t1 >> 3);
	} else if(t1 >= 0x80000l && t1 < 0x400000l) {
		div = 3;
		t16 = (uint16_t)(t1 >> 6);
	}

	TCCR1A = 0xa2; //f +0x10
	TCCR1B = 0x18+div;
	ICR1 = t16 ? (t16-1) : t16;
	OCR1A = (uint16_t)((uint32_t)duty * (uint32_t)t16 / 1000l);
	OCR1B = (uint16_t)((uint32_t)duty * (uint32_t)t16 / 1000l);
}

//=========================================================================

uint8_t n;
uint8_t cnt;
uint8_t led;

#define CENTRAL 74
#define	SWEEP	20
#define START   (CENTRAL+SWEEP/2)
#define END	(CENTRAL-SWEEP/2)


int main(void)
{
	DDRD = 0xe2;	// pd1,5,6,7 out
	DDRB = 0x07;	// pb0-2 out
	DDRC = 0xff;	// pc0-7 out

	T40;
	K00;
	K10;
	K20;
	LED0;

	tau = 20000;    // 20us
	duty = 100;	// 10%
	setup_timer1();

	for(n=START,cnt=0,led=0; ; ) {
		T41;	delay1(); K01;
		T40;	delay1(); K00;
		delay(n);

		T41;	delay1(); K11;
		T40;	delay1(); K10;
		delay(n);

		T41;	delay1(); K21;
		T40;	delay1(); K20;
		delay(n);

		if(++cnt < 5) continue;
		cnt = 0;
		
		if(--n > END) continue;	
		if(led) { LED0; led=0; } else { LED1; led=1; }
		n = START;
	}

	return 0;
}

//=========================================================================

