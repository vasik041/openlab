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

uint8_t year3;
uint8_t year10;
uint8_t sun3;
uint8_t sun10;
uint8_t dst;

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
	dst = 0;

	wr_clk(ALARM1,0x55);
	wr_clk(ALARM2,0x33);

	year3 = year10 = 0;
	sun3 = sun10 = 0;
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

	dst = is_dst();
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
/****************************************************************************/
/*
 * Returns day of week for any given date
 * PARAMS: year, month, day
 *
 * RETURNS: day of week (0-7 is Sun-Sat)
 * NOTES: Sakamoto's Algorithm
 *   http://en.wikipedia.org/wiki/Calculating_the_day_of_the_week#Sakamoto.27s_algorithm
 *
 */

prog_uint8_t t_dow[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

uint8_t dow(uint16_t y, uint8_t m, uint8_t d)
{
       y -= m < 3;
       return (y + y/4 - y/100 + y/400 + pgm_read_byte(t_dow+m-1) + d) % 7;
}
 
/*
 * Returns the date for Nth day of month. 
 * 
 * For instance,it will return the numeric date for the 2nd Sunday of April
 * 
 * PARAMS: year, month, day of week, Nth occurence of that day in that month
 * RETURNS: day
 * NOTES: There is no error checking for invalid inputs.
 */

uint8_t NthDate(int year, uint8_t month, uint8_t dow1, uint8_t NthWeek)
{
uint8_t targetDate = 1;
uint8_t firstDOW = dow(year,month,targetDate);
	while (firstDOW != dow1) {
		firstDOW = (firstDOW+1)%7;
		targetDate++;
	}
	//Adjust for weeks
	targetDate += (NthWeek-1)*7;
	return targetDate;
}


uint8_t find_last_sunday(uint8_t y,uint8_t m)
{
uint8_t d,week;

	for(week=6; week > 3; week--) {    	// can be up to 6 weeks
		d = NthDate(y+2000,m,0,week);
		if(d < 32) return d;
	}
	return 0;				// something gone wrong
}


uint8_t is_dst(void)
{
	if(month < 3 || month > 10) return 0;
	if(month > 3 && month < 10) return 1;

	if(month == 3) {
	        if(sun3 == 0 || year3 != year) {
			sun3 = find_last_sunday(year,3);
		}
		if(day < sun3) return 0;
		if(day > sun3) return 1;
		if(hour < 3) return 0;
		return 1;
	}
	if(sun10 == 0 || year10 != year) {
		sun10 = find_last_sunday(year,10);
	}
	if(day < sun10) return 1;
	if(day > sun10) return 0;
	if(hour < 3) return 1;
	return 0;
}
