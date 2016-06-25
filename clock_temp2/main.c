/****************************************************************************/
/*
 * Clock + thermometer
 * LZs,2010-2012
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
#include <util/delay.h>

#include "glcd.h"
#include "ds1820.h"
#include "graph.h"
#include "i2c.h"
#include "ds1307.h"

extern UINT temp;
extern UCHR tmpb[];
extern UCHR devid[];

volatile int8_t mode;

volatile int t[2];
extern uint8_t hour;
extern uint8_t minute;


//=========================================================================

int is_key_pressed()
{
 	return ((PIND & (1 << KEY)) == 0) ? 1 : 0;
}

//=========================================================================

void delay_ms(int n)
{
int i;
	for(i=0; i < n; i++)
		_delay_us(1000);
}

//=========================================================================

void putt(int val)
{
	if(val < 0) {
		val = -val;
		putc1('-');
	}
	putd3(val >> 4,0);
	putc1('.');
	putc1('0' + (val & 0x0f) * 10 / 16 );
	putc1(DEG);
}

//=========================================================================

void set_id(int n)
{
	devid[0] = 0x28;

	switch(n) {
		case 0:
			devid[1] = 0x04;	//outside
			devid[2] = 0xb5;
			devid[3] = 0x1e;
			devid[4] = 0x02;
			devid[5] = 0;
			devid[6] = 0;
			devid[7] = 0x3d;
			break;

		case 1:
			devid[1] = 0x03;	//internal
			devid[2] = 0xab;
			devid[3] = 0x1e;
			devid[4] = 0x02;
			devid[5] = 0;
			devid[6] = 0;
			devid[7] = 0x66;
			break;
	}
}

//=========================================================================

void get_temp()
{
int i;
	for(i=0; i < 2; i++) {
		set_id(i);
		readTempBus();
		t[i] = temp;
	}
}

//=========================================================================

extern uint8_t e2err;
extern uint8_t e2plc;

// mode= 0-temp(i) 1-temp(o) 2-clock 3-graph

void view(int8_t mode)
{
	cls();
	if(mode == 3) {
		drawt();
		return;
	}

	set_font(1);
 	locxy(4,2);
	if(mode < 2) {
		putt(t[mode]);
	} else {
		putd3(hour,0);
		putc1(':');
		putd2(minute);
	}

	set_font(0);
	locxy(0,7);
	if(mode == 0 || mode == 2) {
		putc1('i');
		putt(t[1]);
		putc1(' ');
	}
	if(mode == 1 || mode == 2) {
		putc1('o');
		putt(t[0]);
		putc1(' ');
	}
	if(mode != 2) {
		putd3(hour,0);
		putc1(':');
		putd2(minute);
	}
	if(e2err) {
		locxy(0,6);
		putc1('p');
		putd2(e2plc);
		putc1(' ');
		putc1('p');
		putd2(e2err);
	}
}

//=========================================================================

void view_time(int8_t time_mask)
{
	cls();
	set_font(0);
 	locxy(4,3);
	if(time_mask & 1) {
		putd2(hour);
	} else {
		putc1(' ');
		putc1(' ');
	}
	putc1(':');
	if(time_mask & 2) {
		putd2(minute);
	} else {
		putc1(' ');
		putc1(' ');
	}
}

//=========================================================================

void set_clk(void)
{
volatile uint16_t timeout;
volatile uint16_t pressed;
volatile uint8_t m;
volatile uint8_t f;
volatile uint8_t msk;

	m = 3;
	f = 0;
	msk = 1;
	pressed = 0;

	view_time(m);

	for(timeout=0; timeout < 10000; timeout++) {
		if(!is_key_pressed()) {
			if(f) {
				if(pressed > 1000) {
					if(msk == 1) msk = 2; else msk = 1;
					m = 3;
				} else {
					if(msk == 1) {
						if(++hour >= 24) hour = 0;
					} else {
						if(++minute >= 60) minute = 0;
					}
					timeout = 0;
				}
				f = 0;
			}
		}
		if(is_key_pressed() && !f) {
			f = 1;
			pressed = 0;
		}
		if((timeout & 0xff) == 0) {
			view_time(m);
			m ^= msk;
		}
		if(f) pressed++;
		delay_ms(2);
	}
	set_time();
}

//=========================================================================

ISR (INT0_vect)
{
}

ISR (INT1_vect)
{
}

//=========================================================================

ISR (TIMER0_OVF_vect)
{
	TCNT0 = 256-2*100;
}

//=========================================================================

int main(void)
{
volatile int8_t j,cnt;

	WDTCR = 0x18;		// Write logical one to WDTOE and WDE
	WDTCR = 0;		// Turn off WDT

	MCUCR = 0x8a;		// SE + idle, INT0,1 falling edge
        GICR = 0x80+0x40;	// int1,int0

//	TCCR0 = 5;		// clk/1024
//	TIMSK = 1;

	lcd_init();
	clrbit(DDRD,KEY);
	clrbit(DDRD,SQW);
	i2c_init();
	j = clock_init();
	cls();
	clrt();

	mode = 0;
	hour = 12;
	minute = 34;

	if(j) {
		set_clk();
	}

	for( ; ; ) {
		cli();
		get_temp();
		get_time();
		sei();
		addt(t[0]);
		view(mode);
		if(++mode > 3) mode = 0;

		for(j=0; j < 5; j++) {
			_SLEEP();
			if(is_key_pressed()) {
				for(cnt=0; is_key_pressed(); cnt++) {
					delay_ms(50);
				}
				if(cnt > 5) {
					set_clk();
				}
				break;
			}
		}

	}
	return 0;
}

//=========================================================================

