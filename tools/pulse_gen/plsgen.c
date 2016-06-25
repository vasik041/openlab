/*=========================================================================*/
/*
 * PlsGen on Uiptfg board
 * This is free and unencumbered software released into the public domain.
 * LZs,2015
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

#include "display.h"
#include "keys.h"

uint8_t div;
uint8_t period;
uint8_t tau;
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


prog_uint16_t divk[] = {
	16, 31, 62, 125, 250, 0, 1, 2, 4, 8, 16, 32, 64, 128, 256
};


void putt(uint8_t t)
{
uint16_t val = 0;
uint16_t k = pgm_read_word(&divk[div]);
	if(k) val = t*k; else val = t/2;
	if(val < 1000) {
		putd3(val,0);
		if(div == 5) {
			if((t & 1) == 1) puts1(PSTR(".5"));
		}
		if(div < 5) puts1(PSTR("ns")); else puts1(PSTR("us"));
	} else {
		putd3(val/1000,0);
		putch('.');
		putch('0'+(val%1000)/100);
		if(div < 5) puts1(PSTR("us")); else puts1(PSTR("ms"));
	}
}

/*=========================================================================*/

void init_timer1()
{
	PLLCSR = (1<<PLLE); // Enable PLL
	// Wait for PLL to lock (approx. 100ms)
	while((PLLCSR & (1 << PLOCK)) == 0);
	PLLCSR |= (1<<PCKE); // Set PLL as PWM clock source 

	TCCR1A = 0x21;  // OC1B, PWM1B
}


void set_timer()
{
	TCCR1B = 1 + div;
	OCR1C = period;
	OCR1B = tau;
}


/*=========================================================================*/

prog_char ds0[] = "15.6ns";
prog_char ds1[] = "31.2ns";
prog_char ds2[] = "62.4ns";
prog_char ds3[] = "125ns";

prog_char ds4[] = "250ns";
prog_char ds5[] = "500us";
prog_char ds6[] = "1us";
prog_char ds7[] = "2us";

prog_char ds8[] = "4us";
prog_char ds9[] = "8us";
prog_char ds10[] = "16us";
prog_char ds11[] = "32us";

prog_char ds12[] = "64us";
prog_char ds13[] = "128us";
prog_char ds14[] = "256us";


prog_char *divstr[] PROGMEM = {
	ds0,ds1,ds2,ds3,   ds4,ds5,ds6,ds7,
	ds8,ds9,ds10,ds11, ds12,ds13,ds14
};


uint8_t step_up()
{
	switch(pos) {
		case 1:
			if(tau < period-1) { tau++; return 1; }
			break;
		case 0:
			if(period < 255) { period++; return 1; }
			break;
		case 2:
			if(div < 14) { div++; return 1; }
			break;
	}
	return 0;
}


uint8_t step_down()
{
	switch(pos) {
		case 1:
			if(tau > 1) { tau--; return 1; }
			break;
		case 0:
			if(period > 10) {
				period--;
			        if(tau >= period) tau=period-1;
				return 1;
			}
			break;
		case 2:
			if(div > 0) { div--; return 1; }
			break;
	}
	return 0;
}

//=========================================================================

void save_params()
{
	eeprom_write_byte( (uint8_t*) 0, div);
	eeprom_write_byte( (uint8_t*) 1, period);
	eeprom_write_byte( (uint8_t*) 2, tau);
}

//=========================================================================

void load_params()
{
	div = eeprom_read_byte((uint8_t*)0);
	if(div > 14) div = 0;
	period = eeprom_read_byte((uint8_t*)1);
	tau = eeprom_read_byte((uint8_t*)2);
	if(period == 0xff && tau == 0xff) {
		period = 200;
		tau = 10;
	}
	if(tau >= period) tau = period-1;
}

/*=========================================================================*/

int main(void)
{
uint8_t f,c,oc,cnt;
	DDRA = 0xf8+3;
	DDRB = 0x38;
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
				case LEFT:
					if(++pos == 3) pos=0; 
					f = 1;
					break;
				case RIGHT:
					if(--pos == 0xff) pos=2;
					f = 1;
					break;
				case UP:
					f = step_up();
					break;
				case DOWN:
					f = step_down();
					break;
			}
		}
		if(f) {
			clrstr(0);
			locxy(0,0);	puts1(PSTR("t")); putt(period);
			locxy(8,0);	puts1(PSTR("w")); putt(tau);
			clrstr(1);
			locxy(0,1);	puts1(PSTR("scale: ")); puts1((prog_char *)pgm_read_word(&divstr[div]));
			set_timer();
			switch(pos) {
				case 0: locxy(1,0); break;
				case 1: locxy(9,0); break;
				case 2: locxy(7,1); break;
			}
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

