/*=========================================================================*/
/*
 * Frequency/L/C meter
 * LZs,2008
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

#include "flc.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>

#include "display.h"

uint32_t freq;
uint32_t val;
uint32_t c0;
uint32_t l0;
uint8_t f;

/*=========================================================================*/

ISR (TIMER0_OVF0_vect)
{
	freq++;
}

/*=========================================================================*/

void putd5(uint32_t n,uint8_t f)
{
uint32_t m;
uint8_t i,p;
	if(n > 999999999) n = 999999999;
	m = 100000000;
	p = f;
	for(i=0; i < 9; i++) {
		if(m == 1) p = 1;
		if(n / m != 0 || p) {
			putch(n / m + '0');
			p = 1;
		}
		n %= m;
		m /= 10;
	}
}

/*=========================================================================*/

void measure_freq()
{
	_CLI();
	TCNT0 = 0;
	freq = 0;
	_SEI();
	delay(1000);
	_CLI();
	freq = (freq << 8) | TCNT0;
	_SEI();
}

/*=========================================================================*/

//#define C	1000e-12	//1000pf
//#define L	100e-6		//100uH

 // 1 / (2 * pi * c)^2 * c
void calc_l()
{
uint32_t a;
	if(freq > 50000) {
		a = 1591550000 / freq;
		val = a*a/100000;
	} else {
		a = 15915500 / freq;
		val = a*a/10;
	}
}



// 1 / (2 * pi * f)^2 * l
void calc_c()
{
uint32_t a;
	if(freq > 50000) {
		a = 1591550000 / freq;
		val = a * a / 10000; 
	} else {
		a = 15915500 / freq;
		val = a * a; 
	}
}

/*=========================================================================*/

void calib()
{
uint8_t i;
	if((PINA & 1) == 0) return;
	clrscr(0);
	puts1(PSTR("Calibration..."));
	l0 = 0;
	c0 = 0;
	for(i=0; i < 10; i++) {
		measure_freq();
		calc_l();
		l0 = val;
		measure_freq();
		calc_c();
		c0 = val;
		if(c0 && c0 == val) return;
	}
}

/*=========================================================================*/

int main(void)
{
	DDRA = 0xfe;
	DDRB = 0x08;
	_WDR();
	WDTCR = 0x0f;

//	sbi(PORTA,1);

	delay(100);
	display_init();

	TCCR0 = 7;	// timer0 - t0 in, rising edge
	TIMSK = 2;	// tcnt0 overflow interrupt

	calib();
	f = 0;
	for( ; ; ) {
		measure_freq();
        
		clrscr(0);
		if(PINA & 1) {
			calc_c();
			puts1(PSTR("Cx: "));
			if(val < c0) val=c0;
			putd5(val-c0,0);
			puts1(PSTR("pF"));
		} else {
			calc_l();
			puts1(PSTR("Lx: "));
			if(val < l0) val=l0;
			putd5(val-l0,0);
			puts1(PSTR("uH"));
		}

		locxy(0,1);
		if(f & 1) {
			puts1(PSTR("Freq: "));
			putd5(freq,0);
			puts1(PSTR("Hz"));
		} else {
			puts1(PSTR("Vol0: "));
			if(PINA & 1) {
				putd5(c0,0); 
				puts1(PSTR("pF"));
			} else {
				putd5(l0,0);
				puts1(PSTR("uH"));
			}
		}
		f ^= 1;
	}
	return 0;
}

/*=========================================================================*/

