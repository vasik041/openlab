/****************************************************************************/
/*
 * Pulse gen using atm88/T1
 * (c) LZs,2012-2014
 */
/****************************************************************************/

#include "main.h"
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <util/delay.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <math.h>

#include "display.h"

volatile uint32_t tau;
volatile uint16_t duty;
volatile uint16_t duty2;
volatile uint8_t f;

volatile uint8_t sw;		// 0-freq,1-dt1,2-dt2
volatile uint8_t step_fq;
volatile uint8_t step_dt1;
volatile uint8_t step_dt2;

//=========================================================================
// 1,8,64,256,1024
// 1 2 3  4   5

void setup_timer1()
{
uint8_t div = 1;
uint16_t t16 = 1;
uint64_t t1 = SCALE_1(tau);

	if(t1 < 0x10000l) {
		div = 1;
		t16 = (uint16_t)t1;
	} else if(t1 >= 0x10000l && t1 < 0x80000l) {
		div = 2;
		t16 = (uint16_t)(t1 >> 3);
	} else if(t1 >= 0x80000l && t1 < 0x400000l) {
		div = 3;
		t16 = (uint16_t)(t1 >> 6);
	}

	TCCR1A = 0xf2; //a +0x10
	TCCR1B = 0x18+div;
	ICR1 = t16 ? (t16-1) : t16;
	OCR1A = (uint16_t)((uint32_t)duty  * (uint32_t)t16 / 1000l);
	OCR1B = (uint16_t)((uint32_t)duty2 * (uint32_t)t16 / 1000l);
}

//=========================================================================

const prog_uint32_t steps_fq[] = {
	1,10,100,1000,10000,100000 // ns
};

const prog_uint32_t steps_dt[] = {
	1,10,100		// 0.1% 1% 10%
};


void keys()
{
	if(KF) {
		while(KF);
		sw = 0;
		f=1;
	} else if(KD1) {
		while(KD1);
		sw = 1;
		f=1;
	} else if(KD2) {
		while(KD2);
		sw = 2;
		f=1;
	} else if(KEY) {
		if(sw == 0) {
			step_fq++;
			if(step_fq >= NELEM(steps_fq)) step_fq=0;
		} else if(sw == 1) {
			step_dt1++;
			if(step_dt1 >= NELEM(steps_dt)) step_dt1=0;
		} else if(sw == 2) {
			step_dt2++;
			if(step_dt2 >= NELEM(steps_dt)) step_dt2=0;
		}
		f=1;
	}
}

volatile uint8_t drc;
volatile uint16_t w;


ISR (INT0_vect)
{
uint32_t step;
	for(w=0; w < 2000; w++);
	if((PIND & 4) != 0) return;	// int0
	drc = DRC;

	if(sw == 0) {
		step = pgm_read_dword(&steps_fq[step_fq]);
		if(drc) {
			if(tau+step < 20000000l) tau += step;	//25hz
		} else {
			if(tau > step) tau -= step;
		}
	} else if(sw == 1) {
		step = pgm_read_dword(&steps_dt[step_dt1]);
		if(drc) {
			if(duty+step < 999)  duty += step;
		} else {
			if(duty > step) duty -= step;
		}
	} else if(sw == 2) {
		step = pgm_read_dword(&steps_dt[step_dt2]);
		if(drc) {
			if(duty2+step < 999)  duty2 += step;
		} else {
			if(duty2 > step) duty2 -= step;
		}
	}
	f = 1;
}

//=========================================================================
// print procent 999 = 99.9%

void putp(uint16_t n)
{
	if(n > 999) n=999;
	putd(n/10,2,0);
	putch('.');
	putch('0'+n%10);
}


void put_step(uint32_t n)
{
    if(n < 1000) {
	putd(n,4,0);
    } else {
	putd(n/1000,3,0);
	putch('K');
    }
}


void show()
{
	clrscr(1);
	locx(0);
	if(sw == 0) {
		putch('F'); putch(' ');
		if(tau < 1000000) {
		    putd(tau,6,0);
		    putch('n');
		} else {
		    putd(tau/1000,6,0);
		    putch('u');
		}
		putch('s');	putch(' ');
		put_step(pgm_read_dword(&steps_fq[step_fq]));
	} else if(sw == 1) {
		putch('D'); putch('1'); putch(' ');
		putp(duty);
		putch('%'); putch(' ');
		putp(pgm_read_dword(&steps_dt[step_dt1]));
		putch('%');
	} else if(sw == 2) {
		putch('D'); putch('2'); putch(' ');
		putp(duty2);
		putch('%'); putch(' ');
		putp(pgm_read_dword(&steps_dt[step_dt2]));
		putch('%');
	}
}

//=========================================================================

void save_params()
{
	eeprom_write_dword((uint32_t*)0,tau);
	eeprom_write_word( (uint16_t*)4,duty) ;
	eeprom_write_word( (uint16_t*)6,duty2);

	eeprom_write_byte( (uint8_t*)8,step_fq);
	eeprom_write_byte( (uint8_t*)9,step_dt1);
	eeprom_write_byte( (uint8_t*)10,step_dt2);
}

//=========================================================================

void load_params()
{
	tau   = eeprom_read_dword((uint32_t*)0);
	duty  = eeprom_read_word( (uint16_t*)4);
	duty2 = eeprom_read_word( (uint16_t*)6);

	step_fq = eeprom_read_byte((uint8_t*)8);
	step_dt1 = eeprom_read_byte((uint8_t*)9);
	step_dt2 = eeprom_read_byte((uint8_t*)10);

	if(tau >= 20000000l) tau = 100000l;	//5khz
	if(tau == 0) tau = 10000l; 

	if(duty == 0) duty=1;
	if(duty > 999) duty=333;

	if(duty2 == 0) duty2=1;
	if(duty2 > 999) duty2=333;

	if(step_fq >= NELEM(steps_fq)) step_fq=0;
	if(step_dt1 >= NELEM(steps_dt)) step_dt1=0;
	if(step_dt2 >= NELEM(steps_dt)) step_dt2=0;
}

//=========================================================================

int main(void)
{
uint8_t savet;
	DDRD = 0;
	DDRB = 0x07;	// pb0-2
	DDRC = 0x3f;	// pc0-5

	EIMSK = 1;	// int0
	EICRA = 2;	// int0 falling edge

	display_init();
	clrscr(1);

	load_params();
	setup_timer1();
	sw = 0;
	f = 1;
	savet = 0;
	sei();

	for( ; ; ) {
		LED1;	delay(100);
		LED0;	delay(100);
		keys();
		if(f) {
			f = 0;
			show();
			setup_timer1();
			savet = 25;
		}
		if(savet) {
			if(--savet == 0) save_params();
		}
	}

	return 0;
}

//=========================================================================

