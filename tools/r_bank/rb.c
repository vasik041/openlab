/*=========================================================================*/
/*
 * R bank
 * This is free and unencumbered software released into the public domain.
 * LZs,2009
 */
/*=========================================================================*/

#include "rb.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>

#include "display.h"



uint16_t val;
uint8_t  f;
uint8_t  btn;

uint16_t val2;
uint8_t  f2;

/*=========================================================================*/

uint16_t get_step()
{
	switch(btn) {
		case 0:	return 1;
		case 1:	return 10;
		case 2:	return 100;
	}
	return 0;
}

/*=========================================================================*/

void putd(uint16_t n,uint8_t f)
{
uint16_t m;
uint8_t i,p;
	if(n > 999) n = 999;
	m = 100;
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
	putd(n % 1000,1);
}

/*=========================================================================*/

ISR (INT0_vect)
{
uint16_t step = get_step();

	if((PINB & 0x40) != 0) {
		if((PINA & 8) != 0) {
			if(val+step >= 4096) return;
			val+=step;
		} else {
			if(val < step) return;
			val-=step;
		}
	} else {
		if((PINA & 8) == 0) {
			if(val+step >= 4096) return;
			val+=step;
		} else {
			if(val < step) return;
			val-=step;
		}
	}
	f = 1;
}

/*=========================================================================*/

void set_val()
{
uint8_t i;
uint16_t d,msk;

	eeprom_write_word((uint16_t*)0,val);
	eeprom_write_word((uint16_t*)2,btn);

	d = val;
	msk = 0x800;
	for(i=0; i < 12; i++) {
		if((d & msk) != 0) {
			sbi(PORTA,0);
		} else {
			cbi(PORTA,0);
		}
		sbi(PORTA,1);
		delay(1);
		cbi(PORTA,1);
		delay(1);
//		d >>= 1;
		msk >>= 1;
	}
}

/*=========================================================================*/

int main(void)
{
	DDRA = 0xf3;	// d4..7, sd,sc
	DDRB = 0x30;	// rs, e

	_WDR();
	WDTCR = 0x0f;

	delay(100);
	display_init();

	MCUCR = 1;	// both edges on int0
	GIMSK = 0x40;	// int0
	_SEI();

	f = 1;
	val = eeprom_read_word((uint16_t*)0);
	if(val > 4095) val = 0;
	btn = eeprom_read_word((uint16_t*)2);
	if(btn > 2) btn = 0;

	val2 = val-1;
	f2 = 0;
//	btn = 0;

	for( ; ; ) {
		if(f) {
			clrscr(0);
			puts1(PSTR("R = "));
			putd1_3(val);
			puts1(PSTR(" Kohm"));

			locxy(0,1);
			puts1(PSTR("step = "));
			putd(get_step(),0);
			f = 0;
			f2 = 0;
		}

		if((PINB & 8) == 0) {
			if(++btn >= 3) btn=0;
			f = 1;
			while((PINB & 8) == 0);
			f = 1;
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


