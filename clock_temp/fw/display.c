/*=========================================================================*/
/*
 * 4 x 7seg LED display driver
 * This is free and unencumbered software released into the public domain.
 * LZs,2005
 */
/*=========================================================================*/

#include "iotn26.h"
#include <ina90.h>
#include "types.h"
#include "display.h"

/*-------------------------------------------------------------------------*/

UCHR	dsp_buf[4];
UCHR	ref_pos;
UCHR	dsp_pos;
UCHR	fInt;

/*-------------------------------------------------------------------------*/
//     a
//  f     b
//     g
//  e     c
//     d    dp

UCHR flash digmsk[] = {
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

/*-------------------------------------------------------------------------*/

void px(UCHR b)
{
	b &= 0x0f;
	putc(b);
}

/*-------------------------------------------------------------------------*/

void putc(UCHR b)
{
	if(b == ' ') {
		dsp_buf[dsp_pos++] = 0;
		return;
	}
	if(b == '-') {
		dsp_buf[dsp_pos++] = segG;	//-
		return;
	}
	if(b == '.') {
		dsp_buf[dsp_pos-1] |= segDP;
		return;
	}
	dsp_buf[dsp_pos++] = digmsk[b];
}

/*-------------------------------------------------------------------------*/

void putx(UCHR b)
{
	px(b >> 4);
	px(b);
}

/*-------------------------------------------------------------------------*/

void putd2(UCHR n)
{
	if(n > 99) n = 99;
	px(n / 10);
	px(n % 10);
}

void putd(UCHR n)
{
	if(n > 99) n = 99;
	if(n > 9) {
		px(n / 10);
	} else {
		putc(' ');
	}
	px(n % 10);
}

/*-------------------------------------------------------------------------*/

void home(void)
{
	dsp_pos = 0;
}

/*--------------------------------------------------------------------------*/

void cls(void)
{
	dsp_buf[0] = 0;
	dsp_buf[1] = 0;
	dsp_buf[2] = 0;
	dsp_buf[3] = 0;
	dsp_pos = 0;
}

/*=========================================================================*/
/*=========================================================================*/

void Setup_Display(void)
{
	setbit(PORTB,cat0);
	setbit(PORTB,cat1);
	setbit(PORTB,cat2);
	setbit(PORTB,cat3);
	ref_pos = 0;
	cls();
	TCCR0 = 5;			/* cpu clk / 1 */
	TIMSK = 2;
}

/*=========================================================================*/

interrupt [TIMER0_OVF_vect] void TIMER0_OVF_interrupt(void)
{
	TCNT0 = 256-5;

	setbit(PORTB,cat0);
	setbit(PORTB,cat1);
	setbit(PORTB,cat2);
	setbit(PORTB,cat3);
	ref_pos++;
	ref_pos &= 3;
	PORTA = dsp_buf[ref_pos];
	switch(ref_pos) {
		case 0: clrbit(PORTB,cat0); break;
		case 1: clrbit(PORTB,cat1); break;
		case 2: clrbit(PORTB,cat2); break;
		case 3: clrbit(PORTB,cat3); break;
	}
	fInt = 1;
}

/*=========================================================================*/

