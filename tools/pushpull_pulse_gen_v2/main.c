/****************************************************************************/
/*
 * Pulse gen using atm88/T1
 * LZs,2012-2014,2020
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

volatile uint32_t freq;
volatile uint16_t duty;
volatile uint8_t f;

volatile uint8_t sw;		// 0-freq, 1-dt1
volatile uint8_t step_fq;
volatile uint8_t step_dt1;
volatile uint8_t sweep;

#define MAX_FREQ 100000		//1Khz
#define MIN_FREQ 1000		//10hz
#define DEF_FREQ 40000

//=========================================================================
// 1,8,64,256,1024
// 1 2 3  4   5

void setup_timer1()
{
uint8_t div = 1;
uint16_t t16 = 1;
uint64_t t1 = 50000000000/freq;	
	t1 = SCALE_1(t1);
	if(t1 < 0x10000l) {
		div = 1;
		t16 = (uint16_t)t1;
	} else if(t1 >= 0x10000l && t1 < 0x80000l) {
		div = 2;				// /8
		t16 = (uint16_t)(t1 >> 3);
	} else if(t1 >= 0x80000l && t1 < 0x400000l) {
		div = 3;				// /64
		t16 = (uint16_t)(t1 >> 6);
	} else if(t1 >= 0x400000l) {
		div = 4;                                // /256
		t16 = (uint16_t)(t1 >> 8);
	}
	TCCR1A = 0xf2; //a +0x10
	TCCR1B = 0x18+div;
	ICR1 = t16 ? (t16-1) : t16;
	OCR1A = (uint16_t)((uint32_t)duty * (uint32_t)t16 / 1000l);
	OCR1B = (uint16_t)((uint32_t)duty * (uint32_t)t16 / 1000l);
}

//=========================================================================

const prog_uint32_t steps_fq[] = {
	5,50,500		// 0.1 1 10 hz
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
                sw = 0;
                sweep++;
                if(sweep > 2) sweep=0;
		f=1;
	} else if(KEY) {
		if(sw == 0) {
			step_fq++;
			if(step_fq >= NELEM(steps_fq)) step_fq=0;
		} else if(sw == 1) {
			step_dt1++;
			if(step_dt1 >= NELEM(steps_dt)) step_dt1=0;
		}
		f=1;
	}
}


// inspired by https://forum.arduino.cc/index.php?topic=341974.30
const prog_int8_t encoderDirections[] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 };

volatile int8_t drc;
volatile uint8_t oldEncoderState;


ISR (PCINT2_vect)
{
uint32_t step;
    oldEncoderState <<= 2;
    oldEncoderState |= (((PIND & 4) >> 2) + ((PIND & 0x40) >> 5));
    drc = pgm_read_byte(&encoderDirections[oldEncoderState & 0x0f]);
    if(drc == 0) return;
    if(sw == 0) {
	step = pgm_read_dword(&steps_fq[step_fq]);
	if(drc == 1) {
		if(freq+step < MAX_FREQ) { freq += step; f = 1; }
	} else {
		if(freq-step > MIN_FREQ) { freq -= step; f = 1; }
	}
    } else if(sw == 1) {
	step = pgm_read_dword(&steps_dt[step_dt1]);
	if(drc == 1) {
		if(duty+step < 999) { duty += step; f = 1; }
	} else {
		if(duty > step) { duty -= step; f = 1; }
	}
    }
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
    switch(n) {
        case 5: 	putch('0');	putch('.');        putch('1'); break;
	case 50: 	putch(' ');	putch(' ');        putch('1'); break;
	case 500: 	putch(' ');	putch('1');        putch('0'); break;
    }
}


void show()
{
	clrscr(1);
	locx(0);
	if(sw == 0) {
		putch((sweep != 0) ? 'S' : 'F'); putch(' ');
		putd(freq/100,3,0);
		putch('.');
		putch('0'+(freq%100)/10);
   		putch(' ');
		put_step(pgm_read_dword(&steps_fq[step_fq]));
	} else if(sw == 1) {
		putch('D'); putch('1'); putch(' ');
		putp(duty);
		putch('%'); putch(' ');
		putp(pgm_read_dword(&steps_dt[step_dt1]));
		putch('%');
	}
}

//=========================================================================

void save_params()
{
	eeprom_write_dword((uint32_t*)0,freq);
	eeprom_write_word( (uint16_t*)4,duty) ;

	eeprom_write_byte( (uint8_t*)8,step_fq);
	eeprom_write_byte( (uint8_t*)9,step_dt1);
}

//=========================================================================

void load_params()
{
	freq  = eeprom_read_dword((uint32_t*)0);
	duty  = eeprom_read_word( (uint16_t*)4);

	step_fq = eeprom_read_byte((uint8_t*)8);
	step_dt1 = eeprom_read_byte((uint8_t*)9);

	if(freq >= MAX_FREQ) freq = DEF_FREQ;	//400hz
	if(freq < MIN_FREQ) freq = DEF_FREQ; 

	if(duty == 0) duty=1;
	if(duty > 999) duty=333;		//33.3%

	if(step_fq >= NELEM(steps_fq)) step_fq=0;
	if(step_dt1 >= NELEM(steps_dt)) step_dt1=0;
}

//=========================================================================

int main(void)
{
uint8_t savet,cnt;
uint32_t step;
	DDRD = 0;
	DDRB = 0x07;	// pb0-2
	DDRC = 0x3f;	// pc0-5

	PCICR = 4;	// PCIE2
	PCMSK2 = 0x44;	// PCINT22 PCINT18

	display_init();
	clrscr(1);

	load_params();
	setup_timer1();
        oldEncoderState = 0;
	sw = 0;
	f = 1;
	savet = 0;
        sweep = 0;
        cnt = 0;
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

		if(sweep) {
		    if(++cnt > 1) {
    			step = pgm_read_dword(&steps_fq[step_fq]);
			if(sweep == 1) {
			    if(freq+step < MAX_FREQ) freq += step; else sweep=0;
			} else if(sweep == 2) {
			    if(freq-step > MIN_FREQ) freq -= step; else sweep=0;
			}
			f = 1;
			cnt = 0;
		    }
		}
	}

	return 0;
}

//=========================================================================

