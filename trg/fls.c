/*=========================================================================*/
/*
 * sin/cos PWM gen
 * LZs,2010-2014
 */
/*=========================================================================*/

#include "fls.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>
#include <math.h>


volatile uint16_t aval;
volatile uint16_t phase;
volatile uint16_t phase2;
volatile uint16_t step;


// 32
prog_char sine[] = {
	255,230,205,180,157,134,113, 93,
	 74, 57, 42, 30, 19, 10,  4,  1,  
	  0,  1,  4, 10, 19, 30, 42, 57,
	 74, 93,113,134,157,180,205,230
};

/*=========================================================================*/

void get_aval(uint8_t ch)
{
        ADMUX = ch | 0x40;
	ADCSRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADSC)) != 0);
	aval = ADC;
}

/*=========================================================================*/

ISR (TIMER1_OVF_vect)
{
uint8_t b;
	cbi(PORTD,5);
	b = (phase >> 7) & 0x1f;
	sbi(PORTD,5);
	OCR1A = pgm_read_byte(sine+b);
	if(phase & 0x1000) {
		cbi(PORTD,6);
	} else {
		sbi(PORTD,6);
	}

	b = (phase2 >> 7) & 0x1f;
	OCR1B = pgm_read_byte(sine+b);
	if(phase2 & 0x1000) {
		cbi(PORTD,7);
	} else {
		sbi(PORTD,7);
	}
	phase += step;
	phase2 += step;
}


/*=========================================================================*/

int main(void)
{
int16_t w;
	DDRC = 0;	// portc in
	PORTC = 0;
	DIDR0 = 1;
	ADCSRA = 0x83;	// ADC enable, clk/8

	DDRB = 0x07;	// LED, OC1A, OC1B
	DDRD = 0xe0;

	TCCR1A = 0xf2;
	TCCR1B = 0x18+1;// clk
	ICR1 = 0x100;	// top
	OCR1A = 0x80;	// value
	OCR1B = 0x80;

	TIMSK1 = 1;	// TOIE1

	phase = 0;
	phase2 = (8 << 7);
	step = 4;
	sei();

	for( ; ; ) {
		get_aval(0);
		w = aval >> 4;
		step = 4+w;

		_delay_ms(100);
		PORTB = PINB ^ 1;
	}

	return 0;
}

/*=========================================================================*/


