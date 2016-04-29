/*-------------------------------------------------------------------------*/
/*
 * Clock & Digital Thermometer
 * This is free and unencumbered software released into the public domain.
 * LZs,2002
 */
/*-------------------------------------------------------------------------*/

#include "iotn26.h"
#include <ina90.h>
#include "types.h"
#include "osc.h"

#include "display.h"
#include "i2c.h"
#include "time.h"
#include "ds1820.h"

/*-------------------------------------------------------------------------*/

extern	UCHR	e2err;

extern	UCHR	hour;
extern	UCHR	min;

extern	UINT	temp;

UCHR	pnt;

/*-------------------------------------------------------------------------*/

void wt1ms(void)
{
UINT w;
	_WDR();
	for(w=140; w; w--);
}

/*-------------------------------------------------------------------------*/

void delay(UINT n)
{
	while(n--)
		wt1ms();
}

/*-------------------------------------------------------------------------*/

void draw_temp(void)
{
int t;
	if(!pnt) {
		_CLI();
		PORTA = 0;
		readTemp();
		_SEI();
	}
	home();
	t = temp;
	if(temp & 0x100) t |= 0xfe00;
	if(t < 0) {
		putc('-');
		t = -t;
	} else {
		putc(' ');
	}
	putd(t >> 1);
	if(pnt & 1) putc('.');
	putc((t & 1) ? 5 : 0);
}

/*-------------------------------------------------------------------------*/

void draw_time(void)
{
	Get_Time();
	home();
	putd2(hour);
	if(pnt & 1) putc('.');
	putd2(min);
}

/*-------------------------------------------------------------------------*/

void RefTime(UCHR m)
{
	home();
	if(m & 1) {
		putd2(hour);
	} else {
		putc(' ');
		putc(' ');
	}

	if(m & 2) {
		putd2(min);
	} else {
		putc(' ');
		putc(' ');
	}
}

/*-------------------------------------------------------------------------*/

UINT timeout;
UINT pressed;
UCHR m;
UCHR f;
UCHR msk;


void set_clk(void)
{
	while(!DQX);
	m = 3;
	msk = 1;
	f = 0;
	RefTime(m);
	for(timeout=0; timeout < 10000; timeout++) {
		if(DQX) {
			if(f) {
				if(pressed > 1000) {
					if(msk == 1) msk = 2; else msk = 1;
					m = 3;
				} else {
					if(msk == 1) {
						if(++hour >= 24) hour = 0;
					} else {
						if(++min >= 60) min = 0;
					}
					timeout = 0;
				}
				f = 0;
			}
		}
		if(!DQX && !f) {
			f = 1;
			pressed = 0;
		}
		if((timeout & 0xff) == 0) {
			RefTime(m);
			m ^= msk;
		}
		if(f) pressed++;
		wt1ms();
	}
	Set_Time();
}

/*-------------------------------------------------------------------------*/

void C_task main(void)
{
	OSCCAL = _OSC_;
	_WDR();
	WDTCR = 0x0f;

	delay(1000);

	DDRA = 0xff;
	DDRB = 0x7f;
	PORTB = 0xff;

	Setup_Display();
	i2c_init();
	_SEI();
	for(pnt=0; ; ) {
		delay(1000);
		if(pnt & 0x10) {
			draw_time();
			if(!DQX) set_clk();
		} else {
			draw_temp();
		}
		++pnt;
		pnt &= 0x1f;
	}
}

/*-------------------------------------------------------------------------*/
