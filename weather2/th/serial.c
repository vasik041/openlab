/*=========================================================================*/
/*
 * 9600 baud tx  (8-2-none)
 * This is free and unencumbered software released into the public domain.
 * LZs,2002
 */
/*=========================================================================*/

#include "main.h"
#include "serial.h"


/*==========================================================================*/

void sftbit(uint8_t c)
{
	if(c) sbi(TXPORT,TXD); else cbi(TXPORT,TXD);
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
	putch_s(n / 100 + '0');	
	n %= 100;
	putch_s(n / 10 + '0');
	putch_s(n % 10 + '0');
}

