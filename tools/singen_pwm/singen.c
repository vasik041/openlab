/*=========================================================================*/
/*
 * SinGen on Uiptfg board
 * This is free and unencumbered software released into the public domain.
 * LZs,2015
 */
/*=========================================================================*/

#include "singen.h"

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

volatile uint16_t phase;
volatile uint16_t step;

#define	F10	(2*10)

// 256
prog_char sine[] = {
	128,131,134,137,140,143,146,149,152,155,158,161,164,167,170,172,
	175,178,181,184,186,189,192,194,197,199,202,204,207,209,211,214,
	216,218,220,222,224,226,228,230,231,233,235,236,238,239,240,242,
	243,244,245,246,247,248,249,249,250,251,251,252,252,252,252,252,
	253,252,252,252,252,252,251,251,250,249,249,248,247,246,245,244,
	243,242,240,239,238,236,235,233,231,230,228,226,224,222,220,218,
	216,214,211,209,207,204,202,199,197,194,192,189,186,184,181,178,
	175,172,170,167,164,161,158,155,152,149,146,143,140,137,134,131,
	128,124,121,118,115,112,109,106,103,100, 97, 94, 91, 88, 85, 83,
	 80, 77, 74, 71, 69, 66, 63, 61, 58, 56, 53, 51, 48, 46, 44, 41,
	 39, 37, 35, 33, 31, 29, 27, 25, 24, 22, 20, 19, 17, 16, 15, 13,
	 12, 11, 10,  9,  8,  7,  6,  6,  5,  4,  4,  3,  3,  3,  3,  3,
	  3,  3,  3,  3,  3,  3,  4,  4,  5,  6,  6,  7,  8,  9, 10, 11,
	 12, 13, 15, 16, 17, 19, 20, 22, 24, 25, 27, 29, 31, 33, 35, 37,
	 39, 41, 44, 46, 48, 51, 53, 56, 58, 61, 63, 66, 69, 71, 74, 77,
	 80, 83, 85, 88, 91, 94, 97,100,103,106,109,112,115,118,121,124
};

/*=========================================================================*/

ISR (TIMER1_OVF1_vect)
{
	OCR1B = pgm_read_byte(sine+(phase >> 8));
	phase += step;
}

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

//=========================================================================

void save_params()
{
	eeprom_write_word( (uint16_t*) 0, step);
}

//=========================================================================

void load_params()
{
	step = eeprom_read_word((uint16_t*)0);
	if(step > MAX_FREQ || step < MIN_FREQ) step = (MAX_FREQ+MIN_FREQ)/2;
}

/*=========================================================================*/

int main(void)
{
uint8_t f,c,oc,cnt;
	DDRA = 0xf8+3;
	DDRB = 0x38;
	OSCCAL = _OSC_;
//	_WDR();
//	WDTCR = 0x0f;

	load_params();
	delay(500);
	display_init();
	clrscr(0);

	PLLCSR = (1<<PLLE); // Enable PLL
	// Wait for PLL to lock (approx. 100ms)
	while((PLLCSR & (1 << PLOCK)) == 0);
	PLLCSR |= (1<<PCKE); // Set PLL as PWM clock source 

	TCCR1A = 0x21;  // OC1B, PWM1B
	TCCR1B = 4;	// clk/8
//	TCCR1D = 1;
	OCR1C = 0xff;	// top
//	OCR1B = 10;	// value

        TIMSK = 4;	// TOIE1
	sei();

	ADCSR = 0x87;	// ADC
        ADMUX = 2;

	for(oc=0,cnt=100,f=1; ; ) {
		c = get_key();
		if(c != oc) {
			switch(c) {
				case LEFT:
					if(step-F10 > MIN_FREQ) { step-=F10; f = 1; }
					break;
				case RIGHT:
					if(step+F10 < MAX_FREQ) { step+=F10; f = 1; }
					break;
				case DOWN:
					if(step > MIN_FREQ) { step--; f = 1; }
					break;
				case UP:
					if(step < MAX_FREQ) { step++; f = 1; }
					break;
			}
		}
		if(f) {
			clrstr(0);
			locxy(0,0);
			puts1(PSTR("Fq: "));
			putd3(step>>1,0); putch('.'); putch((step & 1)?'5':'0');
			f = 0;
			cnt = 0;
		}
		delay(50);
		oc = c;
		if(cnt < 100) {
			if(++cnt == 100) save_params();
		}
		PORTB = PINB ^ (1 << LED);
	}
	return 0;
}

/*=========================================================================*/

