/*=========================================================================*/
/*
 * 9600 baud tx  (8-2-none)
 * This is free and unencumbered software released into the public domain.
 * LZs,2002
 */
/*=========================================================================*/

#include "main.h"
#include "serial.h"

uint8_t timeout;

/*==========================================================================*/

void sftbit(uint8_t c)
{
	if(c) sbi(TXPORT,TXD); else cbi(TXPORT,TXD);
	_delay_us(105);
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

//=============================================================================

volatile uint8_t rcvd_d;
volatile uint8_t rcvd_f;


ISR (PCINT_vect)
{
uint8_t d,j,msk;
	if(rcvd_f) return;
	cli();
	_delay_us(105);
	d = 0;
	msk = 1;
	for(j=0; j < 8; j++) {
		_delay_us(55);
		if((PINA & 0x80) != 0) {
			d |= msk;
		}
		_delay_us(55);
		msk <<= 1;
	}
	_delay_us(200);

	rcvd_d = d;
	rcvd_f = 1;
	sei();
}


void enable_rcvr(void)
{
	GIMSK = 0x20;	// pcie0
	PCMSK0 = 0x80;	// pa7
	PCMSK1 = 0;
	rcvd_f = 0;
	sei();
}


uint8_t rcvd(void)
{
uint8_t b = 0;
	if(rcvd_f) {
		b = rcvd_d;
		rcvd_f = 0;
	}
	return b;
}

//=============================================================================

void putd_s(uint8_t n)
{
uint8_t d3 = n / 100;
	if(d3) {
		putch_s(d3 + '0');	
		n %= 100;
	}
	putch_s(n / 10 + '0');
	putch_s(n % 10 + '0');
}

//=============================================================================

void px(uint8_t d)
{
	d &= 0x0f;
	if(d < 10) putch_s('0'+d); else putch_s('a'+d-10);
}


void putx(uint8_t d)
{
	px(d >> 4);
	px(d);
}


void putxw(uint16_t d)
{
	putx(d >> 8);
	putx(d);
}


void putd2(uint8_t d)
{
	if(d > 99) d = 99;
	putch_s('0'+d/10);
	putch_s('0'+d%10);
}

//=============================================================================

uint8_t rcvd2(void)
{
uint16_t i;
uint8_t c,c2;
	timeout = 1;
	for(i=0; i < 1000; i++) {
		_WDR();
		c = rcvd();
		if(c) break;
		_delay_ms(10);		
	}
	if(i >= 1000) return 0;
	for(i=0; i < 1000; i++) {
		_WDR();
		c2 = rcvd();
		if(c2) break;
		_delay_ms(10);
	}
	if(i >= 1000) return 0;
	if(c >= '0' && c <= '9' && c2 >= '0' && c2 <= '9') {
		timeout = 0;
		return 10*(c-'0')+(c2-'0');
	}
	return 0;
}

