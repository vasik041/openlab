/*=========================================================================*/
/*
 * Frequency meter
 * This is free and unencumbered software released into the public domain.
 * LZs,2008
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
uint8_t sw;

/*=========================================================================*/

ISR (TIMER0_OVF0_vect)
{
	freq++;
}

/*=========================================================================*/

void putd3(uint32_t n, uint8_t f)
{
uint32_t m;
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

void measure_freq()
{
	_CLI();
	TCNT0 = 0;
	freq = 0;
	_SEI();
	delay(1031); // calibrated with Rigol scope
	_CLI();
	freq = (freq << 8) | TCNT0;
	_SEI();
}

/*=========================================================================*/

int main(void)
{
	DDRA = 0xf8+3;
	DDRB = 0x38;

//	_WDR();
//	WDTCR = 0x0f;
//	sbi(PORTA,1);

	delay(500);
	display_init();

	TCCR0 = 7;	// timer0 - t0 in, rising edge
	TIMSK = 2;	// tcnt0 overflow interrupt

	for(sw=0; ; sw ^= 1) {
                if(sw) { sbi(PORTB, 3); } else { cbi(PORTB, 3); }
		measure_freq();
                if(sw) {
  		    clrstr(1);
 	    	    locxy(0, 1);    
                } else {
  		    clrstr(0);
 	    	    locxy(0, 0);    
                }
                puts1(PSTR("Freq: "));
		putd3(freq / 1000, 0);
                putch('.');
		putd3(freq % 1000, 1);
		puts1(PSTR(" KHz"));
	}
	return 0;
}

/*=========================================================================*/

