/****************************************************************************/
/*
 * ds3231
 * LZ,2016
 */
/****************************************************************************/

#include "main.h"
#include "ds3231.h"
#include "i2c.h"

/*=========================================================================*/

extern	uint8_t	e2err;

/*=========================================================================*/

uint8_t	hour;
uint8_t	minute;

uint8_t day;
uint8_t month;
uint8_t year; //20xx

/****************************************************************************/

int8_t clock_init()
{
	if(rd_clk(ALARM1) == 0x55 && rd_clk(ALARM2) == 0x33 &&
			((rd_clk(CTRL_STAT) & 0x80) == 0x80)) {
		return 0;
	}

	minute = 0;
	hour = 0;
	set_time();
	day = 1;
	month = 1;
	year = 16;
	set_date();

	wr_clk(ALARM1,0x55);
	wr_clk(ALARM2,0x33);
	return 1;
}

/****************************************************************************/

void set_time(void)
{
	wr_clk(CONTROL,0);		// SQWE 1Hz
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


uint16_t get_temp()
{
	return (rd_clk(TEMP_H) << 2) + (rd_clk(TEMP_L) >> 6);
}

/****************************************************************************/

void get_date(void)
{
	day = 32;
	month = 13;
	year = 99;

	day = bcd2b(rd_clk(DATE) & 0x7f);
	if(e2err) return;

	month = bcd2b(rd_clk(MONTH) & 0x7f);
	if(e2err) return;

	year = bcd2b(rd_clk(YEAR));
	if(e2err) return;
}

/****************************************************************************/

void set_date(void)
{
	wr_clk(DATE,b2bcd(day));
	wr_clk(MONTH,b2bcd(month));
	wr_clk(YEAR,b2bcd(year));
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
