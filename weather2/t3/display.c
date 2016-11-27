/*=========================================================================*/
/**
 * 2 x 16 LCD driver
 * FE R&D group,2000-2014
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

#include "main.h"
#include "display.h"

volatile uint8_t x;
volatile uint8_t y;

/*=========================================================================*/

void wx(void)
{
	_WDR();
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

void wrs(uint8_t adr,uint8_t d)
{
	PORTD = (d & 0x0f) << 4;
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

/***************************************************************************/
/*
 *ÿinitialize LCD
 */

void display_init(void)
{
	wrs(DSP_C,3);	delay(15); //home
	wrs(DSP_C,3);	delay(5);  //home
	wrs(DSP_C,3);	delay(5);  //home
	wrs(DSP_C,2);	delay(5);  //home

	wdispl(0x28); // function set 4bit, 1line, 5x8
	wdispl(0x08); // display off, cursor off, no blinking
	wdispl(0x01); // clear
	wdispl(0x06); // entry mode set, cursor moving right, no shift

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
	if(m) wdispl(0x0e); else wdispl(0x0c); // display on, cursor on/off
	x = 0;
	wdispl(1); // clear
	delay(20);
}

/****************************************************************************/
/*
 * move cursor to a
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

//=========================================================================
/*
 * print n as decimal with digs digits, skip leading 0s if f=0
 */

void putd3(uint16_t n,uint8_t f)
{
uint8_t i,p;
uint16_t m;
	m = 100;
	p = f;
	for(i=0; i < 3; i++) {
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

//=========================================================================

void px(uint8_t n)
{               
	n &= 0x0f;
	if(n < 10) putch('0'+n); else putch('a'+n-10);
}


void putx(uint8_t n)
{
	px(n >> 4);
	px(n);
}


//=========================================================================
