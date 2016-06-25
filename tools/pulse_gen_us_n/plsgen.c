/*=========================================================================*/
/*
 * PlsGen on Uiptfg board
 * This is free and unencumbered software released into the public domain.
 * LZs,2016
 */
/*=========================================================================*/

#include "plsgen.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>

#include "display.h"
#include "keys.h"

#define	DIV	6	// 0.5us

#define	MIN_T	(1*2)
#define	MAX_T	(20*2)
#define	DEF_T	(1*2)


#define	MIN_P	(20*2)
#define	MAX_P	(120*2)
#define	DEF_P	(50*2)


#define	MIN_N	1
#define	MAX_N	50
#define	DEF_N	20

#define	SEQ_MSK	0xfff

uint8_t period;
uint8_t tau;
uint8_t n;
uint16_t seq;

uint8_t pos;

/*=========================================================================*/

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
			putch(n / m + '0');
			p = 1;
		}
		n %= m;
		m /= 10;
	}
}


void putt(uint8_t val)
{
	putd3(val/2,0);
	if(val & 1) puts1(PSTR(".5"));
	puts1(PSTR("us"));
}


void putfq()
{
uint16_t fq = 20000/period;

	putd3(fq/10,0);
	putch('.');
	putch('0'+fq%10);
	puts1(PSTR("KHz"));
}


void refresh_view()
{
	clrstr(0);
	locxy(0,0);	putt(tau);
	locxy(8,0);	putt(period);
	locxy(0,1);	putd3(n,0); putch('p');
	locxy(8,1);	putfq();

	switch(pos) {
		case 0: locxy(0,0); break;
		case 1: locxy(8,0); break;
		case 2: locxy(0,1); break;
	}
}

/*=========================================================================*/

void init_timer1()
{
	PLLCSR = (1<<PLLE); // Enable PLL
	// Wait for PLL to lock (approx. 100ms)
	while((PLLCSR & (1 << PLOCK)) == 0);
	PLLCSR |= (1<<PCKE); // Set PLL as PWM clock source 

	TCCR1A = 0x31;  // OC1B, PWM1B
	// 0x21 non inv

	TIMSK = 0x20;
	sei();
}


void set_timer()
{
	TCCR1B = DIV;
	OCR1C = period;
	OCR1B = tau;
}


ISR (TIMER1_CMPB_vect)
{
	seq++;
	seq &= SEQ_MSK;
	if(seq == n) TCCR1A = 0x01;
	else if(seq == 0) TCCR1A = 0x31;
}

//=========================================================================

void save_params()
{
	eeprom_write_byte( (uint8_t*) 0, n);
	eeprom_write_byte( (uint8_t*) 1, period);
	eeprom_write_byte( (uint8_t*) 2, tau);
}


void load_params()
{
	n = eeprom_read_byte((uint8_t*)0);
	if(n < MIN_N || n > MAX_N) n = DEF_N;
	period = eeprom_read_byte((uint8_t*)1);
	if(period < MIN_P || period > MAX_P) period = DEF_P;
	tau = eeprom_read_byte((uint8_t*)2);
	if(tau > MAX_T) tau = DEF_T;
}


uint8_t dec()
{
uint8_t f = 0;
	switch(pos) {
		case 0:
			if(tau > MIN_T) { tau--; f = 1; }
			break;

		case 1:
			if(period > MIN_P) {
				period--;
	        		if(tau >= period/2) tau=period/2;
				f = 1;
			}
			break;
		case 2:
			if(n > MIN_N) {	n--; f = 1; }
			break;
		default:;
	}
	return f;
}


uint8_t inc()
{
uint8_t f = 0;
	switch(pos) {
		case 0:
			if(tau < MAX_T && tau < period/2) { tau++; f = 1; }
			break;
		case 1:
			if(period < MAX_P) { period++; f = 1; }
			break;
		case 2:
			if(n < MAX_N) { n++; f = 1; }
			break;
		default:;
	}
	return f;
}

/*=========================================================================*/

int main(void)
{
uint8_t f,c,oc,cnt;
	DDRA = 0xf8+3;
	DDRB = 0x38;
	PORTB |= 8;
//	OSCCAL = _OSC_;
//	_WDR();
//	WDTCR = 0x0f;

	load_params();
	init_timer1();
	set_timer();
	delay(500);
	display_init();
	clrscr(1);

	ADCSR = 0x87;	// ADC
        ADMUX = 2;

	for(oc=0,cnt=100,f=1,pos=0; ; ) {
		c = get_key();
		if(c != oc) {
			switch(c) {
				case RIGHT: f = dec();	break;
				case LEFT:  f = inc();	break;

				case UP:
					if(++pos > 2) pos = 0;
					f = 1;
					break;
				case DOWN:
					if(--pos > 2) pos = 2;
					f = 1;
					break;
			}
		}
		if(f) {
			refresh_view();
			set_timer();
			f = 0;
			cnt = 0;
		}

		delay(50);

		oc = c;
		if(cnt < 100) {
			if(++cnt == 100) save_params();
		}
	}
	return 0;
}

/*=========================================================================*/

