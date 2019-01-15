/*=========================================================================*/
/*
 * UI meter on Uiptfg board
 * This is free and unencumbered software released into the public domain.
 * LZs,2008,2015
 */
/*=========================================================================*/

#include "main.h"

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

uint16_t aval;
uint32_t u;
uint32_t i;
uint8_t tau;

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

/*=========================================================================*/

void putval(uint16_t val,uint8_t chu)
{
	if(val < 1000) {
		putd3(val,0);
		putch('m');
		putch(chu);
		return;
	}
	putd3(val / 1000,0);
	putch('.');
	putd3(val % 1000,1);
	putch(chu);
}


/*=========================================================================*/

uint16_t get_aval(uint8_t ch)
{
        ADMUX = ch;
	wt1ms();
	wt1ms();
	ADCSR |= (1 << ADSC);
	while((ADCSR & (1 << ADSC)) != 0);
	return ADC;
}

uint8_t get_key()
{
int16_t a = get_aval(2);
    if(NEAROF(a,KEY_1_AVAL)) return KEY_1;
    if(NEAROF(a,KEY_2_AVAL)) return KEY_2;
    if(NEAROF(a,KEY_3_AVAL)) return KEY_3;
    if(NEAROF(a,KEY_4_AVAL)) return KEY_4;
    return 0;
}

/*=========================================================================*/

int main(void)
{
uint8_t cnt,c,oc,otau;

	DDRA = 0xf8;
	DDRB = 0x38;
//	OSCCAL = _OSC_;
//	_WDR();
//	WDTCR = 0x0f;

	delay(500);
	display_init();

	tau = eeprom_read_byte((uint8_t *)0);
	if(tau > 99) tau = 10;
	otau = tau;

	TCCR1A = 0x21;  // OC1B, PWM1B
	TCCR1B = 1;
	OCR1C = 100;
	OCR1B = tau;

	ADCSR = 0x87;	// ADC
        ADMUX = 0;

	c = oc = 0;
	for(cnt=10; ; ) {
	    c = get_key();
	    if(c != oc) {
		switch(c) {
		    case KEY_1:	if(tau > 1) tau--; break;
		    case KEY_2:	if(tau < 99) tau++; break;
		    case KEY_3:	if(tau > 5) tau-=5; break;
    		    case KEY_4:	if(tau < 99-5) tau+=5; break;
		}
		OCR1B = tau;
		oc = c;
		cnt = 10;
	    }

	    if(++cnt > 10) {
		aval = get_aval(0); 	u = aval * USCALE;
		aval = get_aval(1);	i = aval * 4;

		clrstr(0);
		locxy(0,0);	putval(u,'V');
		locxy(8,0);	putval(i,'A');

		clrstr(1);
		locxy(0,1);	putval(u / 1000 * i,'W');
		locxy(8,1);	putd3(tau,0); putch('%');

		if(tau != otau) {
		    eeprom_write_byte((uint8_t*)0,tau);
		    otau = tau;
		}
		cnt = 0;
	    }
	    delay(100);
	}
	return 0;
}

/*=========================================================================*/

