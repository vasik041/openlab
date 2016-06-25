/*=========================================================================*/
/*
 * C bank
 * This is free and unencumbered software released into the public domain.
 * LZs,2009
 */
/*=========================================================================*/

#include "cb.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>

#include "display.h"

#define VAL_MAX	64


uint16_t val;
uint8_t  f;

uint16_t val2;
uint8_t  f2;

/*=========================================================================*/

void putd(uint16_t n,uint8_t f)
{
uint16_t m;
uint8_t i,p;
	if(n > 9999) n = 9999;
	m = 1000;
	p = f;
	for(i=0; m != 0; i++) {
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

void putd1_3(uint16_t n)
{
	putch('0'+n/1000);
	putch('.');
	putd(n % 1000,0);
}

/*=========================================================================*/

ISR (INT0_vect)
{
	if((PINB & 0x40) != 0) {
		if((PINB & 8) != 0) {
			if(val+1 >= VAL_MAX) return;
			val++;
		} else {
			if(val < 1) return;
			val--;
		}
	} else {
		if((PINB & 8) == 0) {
			if(val+1 >= 63) return;
			val++;
		} else {
			if(val < 1) return;
			val--;
		}
	}
	f = 1;
}

/*=========================================================================*/

void set_val()
{
uint8_t i,d,msk;

	eeprom_write_word((uint16_t*)0,val);

	d = val;
	msk = 0x80;
	for(i=0; i < 8; i++) {
		if(d & msk) {
			sbi(PORTA,1);
		} else {
			cbi(PORTA,1);
		}
		sbi(PORTA,2);
		delay(1);
		cbi(PORTA,2);
//don't understand how it works :-)
		d >>= 1;
		msk >>= 1;
	}
}

/*=========================================================================*/

int main(void)
{
	DDRA = 0xff;
	DDRB = 0x30;

	_WDR();
	WDTCR = 0x0f;

	delay(100);
	display_init();

	MCUCR = 1;	// both edges on int0
	GIMSK = 0x40;	// int0
	_SEI();

	f = 1;
	val = eeprom_read_word((uint16_t*)0);
	if(val > VAL_MAX) val = 0;
	val2 = val-1;
	f2 = 0;

	for( ; ; ) {
		if(f) {
			clrscr(0);
			puts1(PSTR("C = "));
			putd1_3(val * 125);
			puts1(PSTR(" uF"));
			f = 0;
			f2 = 0;
		}
		delay(100);
		if(++f2 > 10 && val != val2) {
			set_val();
			f2 = 0;
			val2 = val;
		}
	}

	return 0;
}

/*=========================================================================*/


