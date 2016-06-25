/*=========================================================================*/
/*
 * 9600 baud tx @ 1 MHz  (8-2-none)
 * This is free and unencumbered software released into the public domain.
 * LZs,2002
 */
/*=========================================================================*/

#include "uim.h"
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>

#include "serial.h"

/*==========================================================================*/

void sftbit(uint8_t c)
{
	if(c) cbi(PORTB,TXD); else sbi(PORTB,TXD);
	_delay_us(92);
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

