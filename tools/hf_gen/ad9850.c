/*=========================================================================*/
/**
 * AD9850 serial driver
 * This is free and unencumbered software released into the public domain.
 * LZ,2013
 */
/*=========================================================================*/

#include "hfg.h"

#include <avr/io.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>

#include "ad9850.h"

/*=========================================================================*/

void dly()
{
	_delay_us(50);
}

/*=========================================================================*/

void ad9850_reset()
{
	cbi(PORTA,W_CLK);
	dly();
	cbi(PORTA,FQ_UD);
	dly();

	cbi(PORTB,RESET);
	dly();
	sbi(PORTB,RESET); _delay_us(10);
	cbi(PORTB,RESET);
	dly();

	cbi(PORTA,W_CLK);
	dly();
	sbi(PORTA,W_CLK);
	dly();
	cbi(PORTA,W_CLK);
	dly();

	cbi(PORTA,FQ_UD);
	dly();
	sbi(PORTA,FQ_UD);
	dly();
	cbi(PORTA,FQ_UD);
	dly();
}

/*=========================================================================*/
// n = freq * 2^32 / 125 Mhz = freq * 34.36

void ad9850_wr(uint8_t w0,uint32_t freq)
{
uint8_t i,w;
uint32_t y;

	y = freq * 34 + (freq * 36 / 100);

	w = y & 0xff;
	for(i=0; i < 8; i++) {
		if(((w >> i) & 1) != 0) { sbi(PORTA,SDATA); } else { cbi(PORTA,SDATA); }
		sbi(PORTA,W_CLK);
		dly();
		cbi(PORTA,W_CLK);
		dly();
	}

	w = (y >> 8) & 0xff;
	for(i=0; i < 8; i++) {
		if(((w >> i) & 1) != 0) { sbi(PORTA,SDATA); } else { cbi(PORTA,SDATA); }
		sbi(PORTA,W_CLK);
		dly();
		cbi(PORTA,W_CLK);
		dly();
	}

	w = (y >> 16) & 0xff;
	for(i=0; i < 8; i++) {
		if(((w >> i) & 1) != 0) { sbi(PORTA,SDATA); } else { cbi(PORTA,SDATA); }
		sbi(PORTA,W_CLK);
		dly();
		cbi(PORTA,W_CLK);
		dly();
	}

	w = (y >> 24) & 0xff;
	for(i=0; i < 8; i++) {
		if(((w >> i) & 1) != 0) { sbi(PORTA,SDATA); } else { cbi(PORTA,SDATA); }
		sbi(PORTA,W_CLK);
		dly();
		cbi(PORTA,W_CLK);
		dly();
	}

	w = w0;   
	for(i=0; i < 8; i++) {
		if(((w >> i) & 1) != 0) { sbi(PORTA,SDATA); } else { cbi(PORTA,SDATA); }
		sbi(PORTA,W_CLK);
		dly();
		cbi(PORTA,W_CLK);
		dly();
	}

	sbi(PORTA,FQ_UD);
	dly();
	cbi(PORTA,FQ_UD);
	dly();
}

/*=========================================================================*/
