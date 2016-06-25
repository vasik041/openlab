/****************************************************************************/
/*
 * ds1307 operations
 * LZs,2010
 * This is free and unencumbered software released into the public domain.
 */
/****************************************************************************/

#include "main.h"
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>

#include "ds1307.h"
#include "i2c.h"

/*=========================================================================*/

extern	uint8_t	e2err;

/*=========================================================================*/

uint8_t	hour;
uint8_t	minute;

/****************************************************************************/

int8_t clock_init()
{
	if(rd_clk(RTCRAM) == 0x55 && rd_clk(RTCRAM+1) == 0xaa) {
		return 0;
	}
	minute = 0;
	hour = 0;
	set_time();
	wr_clk(RTCRAM,0x55);
	wr_clk(RTCRAM+1,0xaa);
	return 1;
}

/****************************************************************************/

void set_time(void)
{
	wr_clk(CONTROL,0x10);		// SQWE 1Hz
	wr_clk(SECONDS,0);	        // CH=0 seconds=0
	wr_clk(MINUTES,b2bcd(minute));
	wr_clk(HOURS,b2bcd(hour));
}

/****************************************************************************/

void get_time(void)
{
	minute = bcd2b(rd_clk(MINUTES) & 0x7f);
	if(e2err) {
		hour = minute = 61;
		return;
	}
	hour = bcd2b(rd_clk(HOURS) & 0x3f);
	if(e2err) {
		hour = minute = 62;
		return;
	}
}

/****************************************************************************/

uint8_t b2bcd(uint8_t x)
{
uint8_t d1,d2;
	if(x > 99) x = 99;

	d1 = x / 10;
	d2 = x % 10;
	return d2 + (d1 << 4);
}

/****************************************************************************/

uint8_t bcd2b(uint8_t x)
{
	return (x & 0x0f) + ((x >> 4) & 0x0f) * 10;
}

/****************************************************************************/

