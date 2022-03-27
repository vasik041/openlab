/*=========================================================================*/
/*
 * 2 x 16 LCD driver
 * This is free and unencumbered software released into the public domain.
 * LZs,2000-2022
 */
/*=========================================================================*/

#include "include/due_sam3x.h"
#include "display.h"

uint8_t	x,y;

/*=========================================================================*/

void wx(void)
{
volatile uint16_t w;
    for(w=0; w < 1000; w++);
}

/***************************************************************************/
/*
 * delay for 1ms
 */

void wt1ms(void)
{
    Sleep(1);
}

/***************************************************************************/
/*
 * delay for n ms
 */

void delay(uint16_t n)
{
    Sleep(n);
}

/***************************************************************************/

void wrs(uint8_t adr,uint8_t d)
{
    if((d & 1) != 0) { PIOC->PIO_SODR = PIO_PB26; /* DB4=1 */ } else { PIOC->PIO_CODR = PIO_PB26; /* DB4=0 */ }
    if((d & 2) != 0) { PIOC->PIO_SODR = PIO_PB25; /* DB5=1 */ } else { PIOC->PIO_CODR = PIO_PB25; /* DB5=0 */ }
    if((d & 4) != 0) { PIOC->PIO_SODR = PIO_PB24; /* DB6=1 */ } else { PIOC->PIO_CODR = PIO_PB24; /* DB6=0 */ }
    if((d & 8) != 0) { PIOC->PIO_SODR = PIO_PB23; /* DB7=1 */ } else { PIOC->PIO_CODR = PIO_PB23; /* DB7=0 */ }
    if(adr) {
	PIOC->PIO_SODR = PIO_PB22; // RS=1
    } else {
	PIOC->PIO_CODR = PIO_PB22; // RS=0
    }
    wx();
    PIOC->PIO_SODR = PIO_PB21; // E=1
    wx();
    PIOC->PIO_CODR = PIO_PB21; // E=0
}

/***************************************************************************/
/*
 * Initialize LCD
 */

void display_init(void)
{
    PIO_Configure(PIOC, PIO_OUTPUT_1, PIO_PC26, PIO_DEFAULT);	/* Board pin 4 == PC26 DB4 */
    PIO_Configure(PIOC, PIO_OUTPUT_1, PIO_PC25, PIO_DEFAULT);	/* Board pin 5 == PC25 DB5 */
    PIO_Configure(PIOC, PIO_OUTPUT_1, PIO_PC24, PIO_DEFAULT);	/* Board pin 6 == PC24 DB6 */
    PIO_Configure(PIOC, PIO_OUTPUT_1, PIO_PC23, PIO_DEFAULT);	/* Board pin 7 == PC23 DB7 */

    PIO_Configure(PIOC, PIO_OUTPUT_1, PIO_PC22, PIO_DEFAULT);	/* Board pin 8 == PC22 RS */
    PIO_Configure(PIOC, PIO_OUTPUT_1, PIO_PC21, PIO_DEFAULT);	/* Board pin 9 == PC21 E */

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
	wrs(DSP_C, b >> 4);		// *Dsp_Reg = hi b;
	wrs(DSP_C, b);	  		// *Dsp_Reg = lo b;
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

void puts1(const char *s)
{
char c;
	for( ; ; s++) {
		c = *s;
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

//=========================================================================
/*
 * print n as decimal with digs digits, skip leading 0s if f=0
 */

void putd(uint32_t n,uint8_t digs,uint8_t f)
{
uint8_t i,p;
uint32_t m;

    switch(digs) {
	case 2: m = 10;     break;
	case 3: m = 100;    break;
	case 4: m = 1000;   break;
	case 5: m = 10000;  break;
	case 6: m = 100000; break;
	case 7: 
	default: m = 1000000; 
    }

    p = f;
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
