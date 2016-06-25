/*=========================================================================*/
/*
 * Clock ver.3
 * LZs,2015
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

#include "main.h"
#include "ds1307.h"
#include "i2c.h"
#include "keys.h"


volatile uint8_t dsp_buf[4];
volatile uint8_t ref_pos;
volatile uint8_t skip;

extern uint8_t	hour;
extern uint8_t	minute;

volatile uint16_t timer;
volatile uint8_t timer_pos;

//=============================================================================
//     a
//  f     b
//     g
//  e     c
//     d    dp

prog_uint8_t digmsk[] = {
	segA + segB + segC + segD + segE + segF,	//0
	segB + segC,					//1
	segA + segB + segG + segE + segD,     		//2
	segA + segB + segC + segD + segG,       	//3
	segF + segG + segB + segC,       		//4
	segA + segF + segG + segC + segD,       	//5

	segC + segD + segA + segG + segE + segF,	//6
	segC + segA + segB,		       		//7
	segA + segB + segC + segD + segE + segF + segG,	//8
	segB + segC + segD + segA + segF + segG,	//9
	segA + segB + segC + segF + segE + segG,	//a

	segF + segE + segG + segC + segD,	        //b
	segD + segE + segA + segF,		        //c
	segB + segC + segG + segE + segD,	        //d
	segA + segF + segE + segD + segG,	        //e
	segE + segG + segF + segA,			//f
};

//=============================================================================
/*
 * delay for n ms
 */

void delay(uint16_t n)
{
	while(n--) {
		_WDR();
		_delay_us(1000);
	}
}

//=============================================================================

ISR (TIMER0_OVF0_vect)
{
	TCNT0 = 256-5;
	PORTB |= 0x0f;

	if(++skip < 6) return;	// brightness
	skip = 0;

	ref_pos++;
	ref_pos &= 3;
	PORTA = dsp_buf[ref_pos];
/*
	switch(ref_pos) {
		case 0: clrbit(PORTB,ka0); break;
		case 1: clrbit(PORTB,ka1); break;
		case 2: clrbit(PORTB,ka2); break;
		case 3: clrbit(PORTB,ka3); break;
	}
*/
	if(ref_pos == 0) { clrbit(PORTB,ka0); }
	else if(ref_pos == 1) { clrbit(PORTB,ka1); }
	else if(ref_pos == 2) { clrbit(PORTB,ka2); }
	else if(ref_pos == 3) { clrbit(PORTB,ka3); }
}


void putchr(uint8_t pos,uint8_t ch)
{
	dsp_buf[pos] = pgm_read_byte(digmsk+ch);
}

/*=========================================================================*/

void beep()
{
uint16_t i;
	DDRB |= (1<<6);
	for(i=0; i < 3000; i++) {
		clrbit(PORTB,6);
		_delay_us(100);		
		setbit(PORTB,6);
		_delay_us(100);
	}
	DDRB &= ~(1<<6);
}

/*=========================================================================*/

prog_uint8_t timer_vals[] = {
	3,6,10,13, 30,50,60
};


int main(void)
{
uint8_t dots;
uint8_t cnt;
uint8_t f;
uint8_t key;
uint8_t prev_key;
	_WDR();
	WDTCR = 0x0f;

	DDRA = 0xff;	// porta 0-7 out
	PORTA = 0;
	DDRB = 0x0f;	// portb 0-3 out
	PORTB = 0x0f;

	ADCSR = 0x87;	// ADC
        ADMUX = 9;
	i2c_init();
	clock_init();

	TCCR0 = 5;	// cpu clk / 1
	TIMSK = 2;
	ref_pos = 0;
	skip = 0;
//	dsp_buf[0] = 0xff;
//	dsp_buf[1] = 0xff;
//	dsp_buf[2] = 0xff;
//	dsp_buf[3] = 0xff;
	_SEI();

	prev_key = 0;
	timer = 0;
	timer_pos = 0;
	cnt = 0;
	f = 1;
	for(dots=0; ; ) {
		if(++cnt >= 10 || f) {
			if(!timer) {
				get_time();
				putchr(0,hour/10);
				putchr(1,hour%10);
				putchr(2,minute/10);
				putchr(3,minute%10);
			} else {
				uint8_t tm = timer/60;
				uint8_t ts = timer%60;
				putchr(0,tm/10);
				putchr(1,tm%10);
				putchr(2,ts/10);
				putchr(3,ts%10);
				timer--;
				if(timer == 300) beep();
				else if(!timer) beep();
			}
			cnt = 0;
			f = 0;
			if(dots) {
				dsp_buf[1] |= segDP;
			}
			dots ^= 1;
		}
		
		delay(100);

		key = get_key();
		if(key == 0 && prev_key != 0) {
			switch(prev_key) {
				case KEY_1:
					if(timer) break;
					hour++;
					if(hour > 23) hour = 0;
					set_time();
					f = 1;
					break;

				case KEY_2:
					if(timer) break;
					minute++;
					if(minute > 59) minute = 0;
					set_time();
					f = 1;
					break;

				case KEY_3:
					if(timer_pos) {
						timer_pos--;
						timer = 60*pgm_read_byte(timer_vals+timer_pos);
					} else {
						timer = 0;
					}
					f = 1;
					break;

				case KEY_4:
					if(timer_pos < NELEM(timer_vals)-1) {
						if(timer != 0) timer_pos++;
						timer = 60*pgm_read_byte(timer_vals+timer_pos);
						f = 1;
					}
					break;

				default:;
			}
		}
		prev_key = key;
	}

	return 0;
}

/*=========================================================================*/


