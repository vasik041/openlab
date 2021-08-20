/*=========================================================================*/
/*
 * PlsGen on Uiptfg board
 * This is free and unencumbered software released into the public domain.
 * LZs,2016-2021
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

#define	DIV	6	// 0.5us pulse time step

#define	MIN_T	(1*2)   // min pulse time
#define	MAX_T	(50*2)  // max pulse time
#define	DEF_T	(1*2)   // default pulse time

#define	MIN_P	(20*2)	// min pulse period
#define	MAX_P	(120*2) // max pulse period
#define	DEF_P	(50*2)  // default pulse period

#define	MIN_N	1	// min number of pulses
#define	MAX_N	100     // max number of pulses
#define	DEF_N	5       // default number of pulses

#define	MIN_BUT	2       // min burst period
#define	MAX_BUT	100     // max burst period
#define	DEF_BUT	40      // default burst period

//#define INV_OUT

#ifdef INV_OUT
#define TIMER_MODE 0x31 //inv
#else
#define TIMER_MODE 0x21 //non inv
#endif

uint8_t period;
uint8_t tau;
uint8_t n;
uint8_t mode;		// 0 - manual, 1 - auto (bursts)
uint8_t burst_period;
uint8_t seq;
uint8_t run;
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


void putfq(uint8_t m)
{
uint16_t fq = 20000/period;
        fq /= m;
	putd3(fq/10,0);
	putch('.');
	putch('0'+fq%10);
	putch('K');
}


void refresh_view()
{
	clrstr(0);
	locxy(0,0);	putt(tau);
	locxy(6,0);	putt(period);
	locxy(11,0);	putfq(1);

	locxy(0,1);	putd3(n,0); putch('p');
	locxy(4,1);  	if(mode) putch('a'); else putch('m');
        locxy(6,1);	putd3(burst_period,0);	putch('x');
	locxy(11,1);	putfq(burst_period);

        // cursor
	switch(pos) {
		case 0: locxy(0,0); break; // pulse time
		case 1: locxy(6,0); break; // pulse period
		case 2: locxy(0,1); break; // number of pulses
		case 3: locxy(4,1); break; // mode
		case 4: locxy(6,1); break; // burst frequency
	}
}

/*=========================================================================*/

void init_timer1()
{
	PLLCSR = (1<<PLLE); // Enable PLL
	// Wait for PLL to lock (approx. 100ms)
	while((PLLCSR & (1 << PLOCK)) == 0);
	PLLCSR |= (1<<PCKE); // Set PLL as PWM clock source 

	TCCR1A = TIMER_MODE;  // 0x21 non inv/ 0x31 inv / OC1B, PWM1B

	TIMSK = 0x20;
	sei();
}


void set_timer()
{
	TCCR1B = DIV;
	OCR1C = period;
	OCR1B = tau;
}


ISR (TIMER1_COMPB_vect)
{
	if(mode || run) {
	    seq++;
	    if(seq > burst_period) seq = 0; 
	}
	if(seq == n) {
#ifndef INV_OUT
	    PORTB &= ~(1 << 3);
#endif
	    TCCR1A = 0x01;
	    run = 0;
	} else if(seq == 0) {
	    TCCR1A = TIMER_MODE;
#ifndef INV_OUT
	    PORTB |= (1 << 3);
#endif
	}
}

//=========================================================================

void save_params()
{
	eeprom_write_byte( (uint8_t*) 0, n);
	eeprom_write_byte( (uint8_t*) 1, period);
	eeprom_write_byte( (uint8_t*) 2, tau);
	eeprom_write_byte( (uint8_t*) 3, mode);
        eeprom_write_byte( (uint8_t*) 4, burst_period);
}


void load_params()
{
	n = eeprom_read_byte((uint8_t*)0);
	if(n < MIN_N || n > MAX_N) n = DEF_N;

	period = eeprom_read_byte((uint8_t*)1);
	if(period < MIN_P || period > MAX_P) period = DEF_P;

	tau = eeprom_read_byte((uint8_t*)2);
	if(tau > MAX_T) tau = DEF_T;

	mode = eeprom_read_byte((uint8_t*)3) & 1;

	burst_period = eeprom_read_byte((uint8_t*)4);
	if(burst_period < MIN_BUT || burst_period > MAX_BUT) burst_period = DEF_BUT;
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
		case 3:
			mode++;
                        mode &= 1;
			f = 1;
			break;
                case 4:
			if(burst_period > MIN_BUT) {
			    burst_period--;
                            if(n > burst_period/2) n = burst_period/2;
			    f = 1;
			}
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
			if(n < MAX_N && n < burst_period/2) { n++; f = 1; }
			break;
		case 3:
                        mode++;
                        mode &= 1;
			f = 1;
			break;
                case 4:
			if(burst_period < MAX_BUT) { burst_period++; f = 1; }
                        break;
		default:;
	}
	return f;
}

/*=========================================================================*/

void Wdt_Off(void)
{
    _WDR();
    MCUSR = 0x00;			/* Clear WDRF in MCUSR */
    WDTCR |= (1<<WDCE) | (1<<WDE); /* Write logical one to WDCE and WDE */
    WDTCR = 0x00;			/* Turn off WDT */
}


int main(void)
{
uint8_t f,c,oc,cnt;
        Wdt_Off();
	DDRA = 0xf8+3;
	DDRB = 0x38;
#ifdef INV_OUT
	PORTB |= 8;
#else
	PORTB &= ~(1 << 3);
#endif
	run = 1;
	load_params();
	init_timer1();
	set_timer();
	delay(500);
	display_init();
	clrscr(1);

        DIDR0 = 4;      // PA2
	ADCSRA = 0x87;	// ADC
        ADMUX = 2;

	for(oc=0,cnt=100,f=1,pos=0; ; ) {
		c = get_key();
		if(c != oc) {
			switch(c) {
				case RIGHT: f = dec();	break;
				case LEFT:  f = inc();	break;

				case UP:
					if(++pos > 4) pos = 0;
					f = 1;
					break;
				case DOWN:
					if(--pos > 4) pos = 4;
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

		if(!mode && (PINB & (1 << TRIG_KEY)) == 0) { // manual trigger
		    if(!run) run = 1;
		}

		delay(50);

		oc = c;
		if(cnt < 100) {
			if(++cnt == 100) save_params();  // 5sec
		}
	}
	return 0;
}

/*=========================================================================*/

