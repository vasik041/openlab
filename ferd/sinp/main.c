/****************************************************************************/
/*
 * sin/pls gen - atm32 
 * LZs,2011
 * This is free and unencumbered software released into the public domain.
 */
/****************************************************************************/

#include "main.h"
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include "cfg.h"
#include "display.h"
#include "sin.h"

volatile uint16_t freq;
volatile uint16_t freq_step;
volatile uint16_t dly;
volatile uint16_t tau;

volatile uint8_t key;
volatile uint8_t f;

volatile uint8_t sig[256];
volatile uint16_t phase;
volatile uint16_t tstep;

volatile uint16_t ophase;

//=========================================================================
// encoder

volatile uint8_t rk1;
volatile uint16_t rw;


ISR (INT1_vect)
{
	for(rw=0; rw < 2000; rw++);
	if((PIND & 0x08) != 0) return;	// int1 rk0
	rk1 = (PIND & 0x40) ? 1 : 0;	// pd6  rk1

	switch(key) {
		case 0:
			if(rk1) {
				if(freq+freq_step < 5000) freq += freq_step;
			} else {
				if(freq > freq_step+500) freq -= freq_step;
			}
			break;

		case 1:
			if(rk1) {
				if(freq_step < 100) freq_step *= 10;
			} else {
				if(freq_step > 1) freq_step /= 10;
			}
			break;

		case 2:
			if(rk1) {
				if(dly < 250) dly++;
			} else {
				if(dly > 1) dly--;
			}
			break;

		case 3:
			if(rk1) {
				if(tau < 250) tau++;
			} else {
				if(tau > 10) tau--;
			}
			break;
	}
	f = 1;
}

//=========================================================================
// key PB2 falling edge

volatile uint16_t kw;

ISR (INT2_vect)
{
	for(kw=0; kw < 2000; kw++);
	if((PINB & 4) != 0) return;	// too short

	if(++key > 3) key=0;

	for(kw=0; kw < 2000; kw++) {	// wait for un-pressed
		if((PINB & 4) != 0) break;
	}
	f = 1;
}

//=========================================================================
// start timer

volatile uint16_t timer_n;
volatile uint8_t dlycnt;


void timer1_start(uint16_t n)
{
	timer_n = 0x10000-n;
	TCNT1 = timer_n;
}

// periodic timer int 30us
ISR (TIMER1_OVF_vect)
{
	ophase = phase;
	phase += tstep;
	PORTC = sig[(phase >> 8) & 0xff];

	if(phase < ophase) {
		dlycnt = dly;
	}

	if(dlycnt) {
		dlycnt--;
		if(!dlycnt) {
			TCNT0 = 256-tau;
			TCCR0 = 2;
			OUT0;
		}
	}

	TCNT1 = timer_n;
}

//=========================================================================
// pls length


ISR (TIMER0_OVF_vect)
{
	TCCR0 = 0;
	OUT1;
}

//=========================================================================

prog_char freqm[] = "fq:";
prog_char dfm[]   = "df:";
prog_char dlym[]  = "d:";
prog_char taum[]  = "t:";


void show()
{
	locxy(0,0);
	puts1(freqm);
	putd(freq/10,3);	putch('.');	putch('0'+freq%10);

	locxy(9,0);
	puts1(dfm);
	if(freq_step > 1) {
		putd(freq_step/10,3);
	} else {
		putch('0');	putch('.');	putch('1');		
	}

	locxy(0,1);
	puts1(dlym);		putd(dly,3);

	locxy(8,1);
	puts1(taum);		putd(tau,3);

	switch(key) {
		case 0:	locxy(3,0);	break;
		case 1:	locxy(12,0);	break;
		case 2:	locxy(2,1);	break;
		case 3:	locxy(10,1);	break;
	}
}

//=========================================================================

void init()
{
uint16_t i;
	for(i=0; i < 256; i++) {
		sig[i] = pgm_read_byte(sine+i);
//		sig[i] = (i < 128) ? 0 : 0xff;
	}
	phase = 0;

	freq = 4000;
	freq_step = 10;
	dly = 10;
	tau = 10;

	tstep = freq/5;
	load_params();
}

//=========================================================================

volatile uint16_t cnt;


int main(void)
{
	DDRA = 0xff-0xf;	// out, pa0..3 in
	DDRB = 0xff-5;		// out 0,2 in
	DDRC = 0xff;		// out
	DDRD = 0xff-0x6c; 	// in 2,3,5,6
	PORTD = 0x6c;

	WDTCR = 0x18;		// Write logical one to WDTOE and WDE
	WDTCR = 0;		// Turn off WDT

	delay(100);
	display_init();
	clrscr(1);
        
	GICR = 0x80+0x20;	// int1, int2
	MCUCR = 0x08;		// int1 falling edge
				// int2  falling edge see MCUCSR
	TCCR1B = 1;		// osc/1
	TIMSK = 4+1;		// TOIE1 TOIE0
	timer1_start(430);
	init();
	sei();

	key = 0;
	f = 1;
	for(cnt=0; ; ) {
		if(f) {
			show();
			LED0;
			delay(100);
			LED1;
			tstep = freq/5;
			f = 0;
			cnt = 50000;
		}

		if(cnt) {
			cnt--;
			if(!cnt) {
				LED0;
				save_params();
				LED1;
			}
		}
	}

	return 0;
}

//=========================================================================

