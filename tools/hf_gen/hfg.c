/*=========================================================================*/
/*
 * HF Gen AD9850 based
 * This is free and unencumbered software released into the public domain.
 * LZs,2013
 */
/*=========================================================================*/

#include "hfg.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>

#include "display.h"
#include "ad9850.h"

volatile uint32_t f_val;
volatile uint8_t  f;
volatile uint32_t f_val2;
volatile uint8_t  f2;
volatile uint32_t step;

/*=========================================================================*/

void putd(uint32_t n, uint8_t f)
{
uint32_t m;
uint8_t i,p;
	if(n > 99999999) n = 99999999;
	m = 10000000;
	p = f;
	for(i=0; m != 0; i++) {
		if(m == 1) p = 1;
		if(n / m != 0 || p) {
			if(m == 100 || m == 100000) putch(' ');
			putch(n / m + '0');
			p = 1;
		}
		n %= m;
		m /= 10;
	}
}

/*=========================================================================*/

const prog_int8_t encoderDirections[] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 };

volatile int8_t drc;
volatile uint8_t oldEncoderState;


ISR (INT0_vect)
{
    oldEncoderState <<= 2;
    oldEncoderState |= (((PINA & 8) >> 3) + ((PINB & 0x40) >> 5));
    drc = pgm_read_byte(&encoderDirections[oldEncoderState & 0x0f]);
    if(drc == 0) return;

    if(drc == -1) {
        if(f_val + step >= F_MAX) return;
        f_val += step;
    } else {
        if(f_val - step <= F_MIN) return;
        f_val -= step;
    }
    f = 1;
}

/*=========================================================================*/

void set_val()
{
	eeprom_write_dword((uint32_t*)0, f_val);
	eeprom_write_dword((uint32_t*)4, step);
	ad9850_wr(0, f_val);
}

/*=========================================================================*/

int main(void)
{
	DDRA = 0xf7;	// d4..7, key(in), sd,sc,fq_ud
	DDRB = 0x32;	// rs, e, reset (out)
	DIDR0 = 0;	// PA0 as digital i/o

	delay(300);
	ad9850_reset();
	display_init();

	MCUCR = 1;	// both edges on int0
	GIMSK = 0x40;	// int0
	_SEI();

	f = 1;
	f_val = eeprom_read_dword((uint32_t*)0);
	if(f_val > F_MAX) f_val = F_MAX;
	if(f_val < F_MIN) f_val = F_MIN;

	step = eeprom_read_dword((uint32_t*)4);
	if(step == 0) step = 1;
	if(step > STEP_MAX) step = STEP_MAX;
	ad9850_wr(0, f_val);

	f_val2 = f_val-1;
	f2 = 0;

	for( ; ; ) {
		if(f) {
                        _CLI();
			clrscr(1);
			puts1(PSTR("F= "));
			putd(f_val,0);
			puts1(PSTR(" Hz"));

			locxy(0,1);
			puts1(PSTR("Step= "));
			if(step < 1000) putd(step, 0); else { putd(step/1000, 0); puts1(PSTR("K")); }
			f = 0;
			f2 = 0;
                        _SEI();
		}

                _CLI();
		if((PINB & 8) == 0) {
			if(step < STEP_MAX) step *= 10; else step = 1;
			while((PINB & 8) == 0);
			f = 1;
		}
                _SEI();

		delay(500);
		if(++f2 > 10 && f_val != f_val2) {
                        _CLI();
			set_val();
                        _SEI();
			f2 = 0;
			f_val2 = f_val;
		}
	}

	return 0;
}

/*=========================================================================*/

