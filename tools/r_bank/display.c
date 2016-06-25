/*=========================================================================*/
/**
 * 2 x 16 LCD driver
 * This is free and unencumbered software released into the public domain.
 * LZs,2000
 */
/*=========================================================================*/

#include "rb.h"

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
	_delay_us(10);
}

/***************************************************************************/
/*
 * delay for 1ms
 */

void wt1ms(void)
{
	_WDR();
	_delay_us(1000);
}

/***************************************************************************/
/*
 * delay for n ms
 */

void delay(uint16_t n)
{
	while(n--)
		wt1ms();
}

/***************************************************************************/

#define RS	4
#define E	5


void wrs(uint8_t adr,uint8_t d)
{
	PORTA = ((d << 4) & 0xf0);
	if(adr) {
		sbi(PORTB,RS);
	} else {
		cbi(PORTB,RS);
	}
	wx();
	sbi(PORTB,E);
	wx();
	cbi(PORTB,E);
}

/***************************************************************************/
/*
 *ÿinitialize LCD
 */

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
/*
 * write to LCD
 */

void wdispl(uint8_t b)
{
	wrs(DSP_C,b >> 4);		// *Dsp_Reg = hi b;
	wrs(DSP_C,b);	  		// *Dsp_Reg = lo b;
	wt1ms();
}

/*-------------------------------------------------------------------------*/
/*
 * print char on LCD
 */

void putch(uint8_t c)
{
	if(x >= SCR_W) x = 0;
	wrs(DSP_D,c >> 4);
	wrs(DSP_D,c);
	wt1ms();
	x++;
}

/****************************************************************************/
/**
 * Print a string
 */

void puts1(const prog_char *s)
{
char c;
	for( ; ; s++) {
		c = pgm_read_byte(s);
		if(c == '\0') break;
		putch(c);
	}
}

/*-------------------------------------------------------------------------*/
/*
 * clead LCD, m=1 - show cursor; 0 - no
 */

void clrscr(uint8_t m)
{
	if(m) wdispl(0x0e); else wdispl(0x0c);
	x = 0;
	y = 0;
	wdispl(1);
	delay(20);
}

/****************************************************************************/
/*
 * move cursor to a,b
 */

void locxy(uint8_t a,uint8_t b)
{
uint8_t q = 0;
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
 * clear line n (0 or 1)
 */

void clrstr(uint8_t n)
{
uint8_t x;
	locxy(0,n);
	for(x=0; x < SCR_W; x++)
		putch(' ');
}
