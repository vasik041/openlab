/*=========================================================================*/
/*
 * UI meter on Uiptfg board
 * This is free and unencumbered software released into the public domain.
 * LZs,2008,2015
 */
/*=========================================================================*/

#include "uim.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>

#include "ds1820.h"
#include "display.h"
#include "serial.h"

extern uint16_t temp;

uint16_t aval;
uint32_t u;
uint32_t i;

/*=========================================================================*/

void putch1(uint8_t ch)
{
	putch(ch);
	putch_s(ch);
}


void putd3(uint16_t n,uint8_t f)
{
uint16_t m;
uint8_t i,p;
	if(n > 999) n = 999;
	m = 100;
	p = f;
	for(i=0; i < 3; i++) {
		if(m == 1) p = 1;
		if(n / m != 0 || p) {
			putch1(n / m + '0');
			p = 1;
		}
		n %= m;
		m /= 10;
	}
}

/*=========================================================================*/

void putval(uint16_t val,uint8_t chu)
{
	if(val < 1000) {
		putd3(val,0);
		putch1('m');
		putch1(chu);
		return;
	}
	putd3(val / 1000,0);
	putch1('.');
	putd3(val % 1000,1);
	putch1(chu);
}

/*=========================================================================*/

void putt(uint16_t val)
{
	if(val < 0) {
		val = -val;
		putch1('-');
	}
	putd3(val >> 4,0);
	putch1('.');
	putch1('0' + (val & 0x0f) * 10 / 16 );
	putch1('C');
}

/*=========================================================================*/

void get_aval(uint8_t ch)
{
        ADMUX = ch;
	wt1ms();
	wt1ms();
	ADCSR |= (1 << ADSC);
	while((ADCSR & (1 << ADSC)) != 0);
	aval = ADC;
}

/*=========================================================================*/

int main(void)
{
	DDRA = 0xf8;
	DDRB = 0x38;
//	OSCCAL = _OSC_;
//	_WDR();
//	WDTCR = 0x0f;

	delay(500);
	display_init();

	ADCSR = 0x87;	// ADC
        ADMUX = 0;

	for( ; ; ) {
		readTemp();
		get_aval(0); 	u = aval * USCALE;
		get_aval(1);	i = aval * 4;

		clrstr(0);
		locxy(0,0);	putval(u,'V'); putch_s(' ');
		locxy(8,0);	putval(i,'A'); putch_s(' ');

		clrstr(1);
		locxy(0,1);	putval(u / 1000 * i,'W'); putch_s(' ');
		locxy(8,1);	putt(temp); nl();
		
		delay(1000);
	}
	return 0;
}

/*=========================================================================*/

