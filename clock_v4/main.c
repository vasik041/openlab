/*=========================================================================*/
/*
 * Clock ver.4
 * LZs,2016
 */
/*=========================================================================*/

#include "main.h"
#include "ds3231.h"
#include "i2c.h"
#include "keys.h"
#include "bme280.h"


volatile uint8_t dsp_buf[4];
volatile uint8_t ref_pos;
volatile uint8_t skip;

extern uint8_t hour;
extern uint8_t minute;
extern uint8_t day;
extern uint8_t month;
extern uint8_t year;
extern uint8_t dst;

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

ISR (TIMER0_OVF_vect)
{
	TCNT0L = 256-5;
	PORTB |= 0x0f;

	if(++skip < 6) return;	// brightness
	skip = 0;

	ref_pos++;
	ref_pos &= 3;
	PORTA = dsp_buf[ref_pos];

	switch(ref_pos) {
		case 0: clrbit(PORTB,ka0); break;
		case 1: clrbit(PORTB,ka1); break;
		case 2: clrbit(PORTB,ka2); break;
		case 3: clrbit(PORTB,ka3); break;
	}
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

uint8_t swcnt,sw,mode;
uint32_t t,temp,hum,pres;


void show()
{
	if(++swcnt > (sw?10:30)) {
		swcnt = 0;
		sw++;
		sw &= 3;
	}
	switch(sw) {
		case 0:
			get_time();
			putchr(0,(hour+dst)/10);
			putchr(1,(hour+dst)%10);
			putchr(2,minute/10);
			putchr(3,minute%10);
			break;

		case 1:
			if(swcnt == 0) {
				temp = readTemperature() / 10;
			}
			putchr(2,temp%10);
			t = temp / 10;
			putchr(0,t/10);
			putchr(1,t%10);
			putchr(3,0x0c);
			break;

		case 2:
			if(swcnt == 0) {
				hum = readHumidity() * 5 / 512;
			}
			putchr(2,hum%10);
			t = hum / 10;
			putchr(0,t/10);
			putchr(1,t%10);
			dsp_buf[3] = chrH;
			break;

		case 3:
			if(swcnt == 0) {
				pres = readPressure() / 100;
			}
			t = pres;
			if(t > 999) {
				putchr(3,t%10); t /= 10;
				putchr(2,t%10); t /= 10;
				putchr(1,t%10); t /= 10;
				putchr(0,t);
			} else {
				dsp_buf[0] = chrP;
				putchr(3,t%10); t /= 10;
				putchr(2,t%10); t /= 10;
				putchr(1,t%10);
			}
			break;

		default:;
	}
}


uint8_t tocnt;

void setting()
{
	if(++tocnt > 30) {
		mode = 0;
		return;
	}
	switch(mode) {
		case 1:
			putchr(0,(hour+dst)/10);
			putchr(1,(hour+dst)%10);
			dsp_buf[2] = chrSP;
			dsp_buf[3] = chrSP;
			break;

		case 2:
			dsp_buf[0] = chrSP;
			dsp_buf[1] = chrSP;
			putchr(2,minute/10);
			putchr(3,minute%10);
			break;

		case 3:
			putchr(0,day/10);
			putchr(1,day%10);
			dsp_buf[2] = chrDASH;
			dsp_buf[3] = chrSP;
			break;

		case 4:
			dsp_buf[0] = chrDASH;
			putchr(1,month/10);
			putchr(2,month%10);
			dsp_buf[3] = chrDASH;
			break;

		case 5:
			putchr(0,2);
			putchr(1,0);
			putchr(2,year/10);
			putchr(3,year%10);
			break;

		default:;
	}
}


void inc()
{
	switch(mode) {
		case 1:
			if(++hour > 23) hour = 0;
			set_time();
			break;
		case 2:
			if(++minute > 59) minute = 0;
			set_time();
			break;
		case 3:
			if(++day > 31) day = 0;
			set_date();
			break;
		case 4:
			if(++month > 12) month = 1;
			set_date();
			break;
		case 5:
			if(++year > 26) year = 16;
			set_date();
			break;
		default:;
	}
}

/*=========================================================================*/

prog_uint8_t timer_vals[] = {
	3,6,10,13, 30,50,60
};


int main(void)
{
uint8_t dots,cnt,f;
uint8_t key,prev_key;
uint16_t dcnt;
	_WDR();
	WDTCR = 0x0f;

	DDRA = 0xff;	// porta 0-7 out
	PORTA = 0;
	DDRB = 0x0f;	// portb 0-3 out
	PORTB = 0x0f;

	ADCSRA = 0x87;	// ADC
        ADMUX = 9;
	i2c_init();
	clock_init();

	TCCR0B = 5;	// cpu clk / 1
	TIMSK = 2;
	ref_pos = 0;
	skip = 0;
	_SEI();
	bme_init();

//	wr_eeprom(11,0x55);

	prev_key = 0;
	timer = 0;
	timer_pos = 0;
	cnt = 0;
	f = 1;
	swcnt = sw = mode = 0;
	dcnt = 0;
	get_date();
	for(dots=0; ; ) {
		if(++cnt >= 10 || f) {
			if(!timer) {
				if(mode) setting(); else show();
			} else {
				uint8_t tm = timer/60;
				uint8_t ts = timer%60;
				putchr(0,tm/10);
				putchr(1,tm%10);
				putchr(2,ts/10);
				putchr(3,ts%10);
				timer--;
				if(timer == 300) beep(); //5min
				else if(!timer) beep();  //end
			}
			cnt = 0;
			f = 0;
			if(mode) {
				if(dots) {
					dsp_buf[0] = chrSP;
					dsp_buf[1] = chrSP;
					dsp_buf[2] = chrSP;
					dsp_buf[3] = chrSP;
				}
			} else {
				if(dots || sw == 1 || sw == 2) {
					dsp_buf[1] |= segDP;
				}
				if(sw == 3) {
					dsp_buf[1] &= ~segDP;
					dsp_buf[3] |= segDP;
				}
			}
			dots ^= 1;
		}
		
		delay(100);

		key = get_key();
		if(key == 0 && prev_key != 0) {
			tocnt = 0;
			switch(prev_key) {
				case KEY_1:
					if(timer) break;
					if(++mode > 5) mode = 0;
					f = 1;
					sw = swcnt = 0;
					break;

				case KEY_2:
					if(timer) break;
					inc();
					f = 1;
					sw = swcnt = 0;
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

		if(++dcnt > 10*60) {
			dcnt = 0;
			get_date();
		}
	}

	return 0;
}

/*=========================================================================*/


