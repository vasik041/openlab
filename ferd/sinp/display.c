/*=========================================================================*/
/*
 * 2 x 16 LCD driver
 * LZs,2000
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

#include "main.h"
#include <avr/io.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>

#include "display.h"

volatile uint8_t x,y;

/*=========================================================================*/
/*
 * small delay
 */
void wx(void)
{
	_delay_us(50);
}

/*=========================================================================*/
/*
 * delay for 1ms
 */

void wt1ms(void)
{
	_WDR();
	_delay_us(1000);
}

/*=========================================================================*/
/*
 * delay for n ms
 */

void delay(uint16_t n)
{
	while(n--)
		wt1ms();
}

/*=========================================================================*/

#define RS	1	// PD1
#define E	0	// PD0


void wrs(uint8_t adr,uint8_t d)
{
	PORTA = (d << 4) & 0xf0;
	if(adr) {
		sbi(PORTD,RS);
	} else {
		cbi(PORTD,RS);
	}
	wx();
	sbi(PORTD,E);
	wx();
	cbi(PORTD,E);
}

/*=========================================================================*/
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

/*=========================================================================*/
/*
 * write to LCD
 */

void wdispl(uint8_t b)
{
	wrs(DSP_C,b >> 4);		// *Dsp_Reg = hi b;
	wrs(DSP_C,b);	  		// *Dsp_Reg = lo b;
	wt1ms();
}

/*=========================================================================*/
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

/*=========================================================================*/
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

/*=========================================================================*/
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

/*=========================================================================*/
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

/*=========================================================================*/
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

/*=========================================================================*/

void putd(uint16_t n,uint8_t digs)
{
uint8_t i,p;
uint16_t m;

	switch(digs) {
		case 1: m = 1;	  if(n > 9) n=9; 	break;
		case 2: m = 10;   if(n > 99) n=99;  	break;
		case 3: m = 100;  if(n > 999) n=999;	break;
		case 4: m = 1000; if(n > 9999) n=9999;  break;
		case 5:
		default: m = 10000;
	}

	p = 0;
	for(i=0; i < digs; i++) {
		if(m == 1) p = 1;
		if(n / m != 0 || p) {
			putch(n / m + '0');
			p = 1;
		} else {
			putch(' ');
		}
		n %= m;
		m /= 10;
	}
}

/*=========================================================================*/
