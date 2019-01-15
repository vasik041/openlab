/*=========================================================================*/
/*
 * 9600 baud tx  (8-2-none)
 * LZs,2002
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

#include "main.h"
#include "serial.h"


/*==========================================================================*/

void sftbit(uint8_t c)
{
	if(c) sbi(PORTA,TXD); else cbi(PORTA,TXD);
	_delay_us(100);
}

/*=========================================================================*/
/*
 * print char
 */

void putch_s(uint8_t c)
{
uint8_t msk,i;

	sftbit(0);
	msk = 1;
	for(i=0; i < 8; i++) {
		sftbit(c & msk);
		msk <<= 1;
	}
	sftbit(1);
	sftbit(1);
	_delay_ms(5);
}

/*==========================================================================*/

void nl(void)
{
	putch_s('\r');
	putch_s('\n');
}

/*==========================================================================*/

void px(uint8_t n)
{
	n &= 0x0f;
	if(n < 10) putch_s('0'+n); else putch_s('a'+n-10);
}

/*==========================================================================*/

void putx(uint8_t n)
{
	px(n >> 4);
	px(n);
}


/*=========================================================================*/

void putxw(uint16_t n)
{
	px(n >> 12);
	px(n >> 8);
	px(n >> 4);
	px(n);
}

/*=========================================================================*/

void putxq(uint32_t n)
{
	putxw(n >> 16);
	putxw(n);
}

/*=========================================================================*/

void puts_s(const prog_char *s)
{
char c;
	for( ; ; s++) {
		c = pgm_read_byte(s);
		if(c == '\0') break;
		putch_s(c);
	}
}

/*=========================================================================*/

void putd2_s(uint8_t n)
{
	if(n > 99) n = 99;
	putch_s('0'+n/10);
	putch_s('0'+n%10);
}

void putd4_s(uint16_t n)
{
	if(n > 9999) n = 9999;
	putd2_s(n/100);
	putd2_s(n%99);
}
