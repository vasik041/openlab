/*=========================================================================*/
/*
 * 2 x 16 LCD driver
 * This is free and unencumbered software released into the public domain.
 * LZs,2000
 */
/*=========================================================================*/

#include "tm.h"

#include <avr/io.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>

#include "display.h"

uint8_t	x,y;

/*=========================================================================*/

void wx(void)
{
	_WDR();
	_delay_us(10);
}

/***************************************************************************/

void wt1ms(void)
{
	_WDR();
	_delay_us(1000);
}

/***************************************************************************/

void delay(uint16_t n)
{
	while(n--)
		wt1ms();
}

/***************************************************************************/

void wrs(uint8_t adr,uint8_t d)
{
	if(adr) { sbi(PORTB,RS); } else { cbi(PORTB,RS); }
	PORTA = ((d << 4) & 0xf0) | 3;
	wx();
	sbi(PORTB,E);
	wx();
	cbi(PORTB,E);
}

/***************************************************************************/

void display_init(void)
{
	wrs(DSP_C,3);	delay(15);
	wrs(DSP_C,3);	delay(5);
	wrs(DSP_C,3);	delay(1);
	wrs(DSP_C,2);	delay(1);

	wdispl(0x28);
	wdispl(0x08);
	wdispl(0x01);
	wdispl(0x06);

	clrscr(0);
}

/*-------------------------------------------------------------------------*/

void wdispl(uint8_t b)
{
	wrs(DSP_C,b >> 4);		// *Dsp_Reg = hi b;
	wrs(DSP_C,b);	  		// *Dsp_Reg = lo b;
	wt1ms();
}

/*-------------------------------------------------------------------------*/

void putch(uint8_t c)
{
	if(x >= SCR_W) x = 0;
	wrs(DSP_D,c >> 4);
	wrs(DSP_D,c);
	wt1ms();
	x++;
}

//=============================================================================
// Print a string

/*
void puts1(const prog_char *s)
{
uint8_t c;
	for( ; ; s++) {
		c = pgm_read_byte(s);
		if(c == '\0') break;
		putch(c);
	}
}
*/

/*-------------------------------------------------------------------------*/

void clrscr(uint8_t m)
{
	if(m) wdispl(0x0e); else wdispl(0x0c);
	x = 0;
	y = 0;
	wdispl(1);
	delay(20);
}

/****************************************************************************/

void locxy(uint8_t a,uint8_t b)
{
uint8_t q = x;
	x = a;
	y = b;
	switch(y) {
		case 0: q = x;  	break;
		case 1: q = x | 0x40;  	break;
	}
	wdispl(0x80 | q);
}

/***************************************************************************/
/*
void clrstr(uint8_t n)
{
uint8_t x;
	locxy(0,n);
	for(x=0; x < SCR_W; x++)
		putch(' ');
}
*/
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

