/*=========================================================================*/
/*
 * Receiver/logger for RF sensors
 * LZs,2016
 */
/*=========================================================================*/

#include "main.h"
#include "rf24l01.h"
#include "serial.h"
#include "ds3231.h"
#include "i2c.h"

extern uint8_t channel;
extern uint8_t payload;

extern uint8_t	hour;
extern uint8_t	minute;
extern uint8_t day;
extern uint8_t month;
extern uint8_t year; //20xx

extern uint8_t timeout;

uint8_t d[4];

uint8_t  log_rec[10];
uint8_t  log_rec2[10];
uint16_t log_wr_pos;
uint16_t log_rd_pos;

//=============================================================================
/*
 * delay for n ms
 */

void delay(uint16_t n)
{
	while(n--) {
		_WDR();
		_delay_us(1000);
	}
}

//=============================================================================

#define	EEPROM_SIZE	(4*1024)	//24c32


uint8_t make_log_cs()
{
uint8_t i,cs;
	for(i=0,cs=0x14; i < NELEM(log_rec)-1; i++) {
		cs ^= log_rec[i];
	}
	return cs;
}


uint8_t check_log_cs()
{
uint8_t i,cs;
	for(i=0,cs=0x14; i < NELEM(log_rec)-1; i++) {
		cs ^= log_rec[i];
	}
	return (cs == log_rec[NELEM(log_rec)-1]) ? 1 : 0;
}


void log_add(uint8_t *d)
{
uint8_t i;
	get_time();
	get_date();
	log_rec[0] = hour;
	log_rec[1] = minute;
	log_rec[2] = day;
	log_rec[3] = month;
	log_rec[4] = year;
	log_rec[5] = d[0];
	log_rec[6] = d[1];
	log_rec[7] = d[2];
	log_rec[8] = d[3];
	log_rec[9] = make_log_cs();

	for(i=0; i < NELEM(log_rec); i++) {
		_WDR();
		wr_eeprom(log_wr_pos+i,log_rec[i]);
	}
	log_wr_pos += 16;
	if(log_wr_pos >= EEPROM_SIZE) log_wr_pos = 0;
}


uint8_t log_get(uint16_t pos)
{
uint8_t i;
	for(i=0; i < NELEM(log_rec); i++) {
		_WDR();
		log_rec[i] = rd_eeprom(pos+i);
	}
	return check_log_cs();
}


uint8_t cmp_date(uint8_t *log_rec,uint8_t *logrec2)
{
	if(log_rec2[4] < log_rec[4]) return 1; //year
	if(log_rec2[3] < log_rec[3]) return 1; //month
	if(log_rec2[2] < log_rec[2]) return 1; //day
	if(log_rec2[0] < log_rec[0]) return 1; //hour
	if(log_rec2[1] < log_rec[1]) return 1; //minute
	return 0;
}


void log_init()
{
	log_rd_pos = 0;
	log_wr_pos = 0;
	for(;;) {
		if(!log_get(log_wr_pos)) break;
		memcpy(log_rec2,log_rec,sizeof(log_rec));
		log_wr_pos += 16;
		if(log_wr_pos >= EEPROM_SIZE) {
			log_wr_pos = 0; //should not happen
			break;
		}
	        if(!log_get(log_wr_pos) || cmp_date(log_rec,log_rec2))
			return;
		log_wr_pos += 16;
		if(log_wr_pos >= EEPROM_SIZE) {
			log_wr_pos = 0; //should not happen
			break;
		}
	}
	puts_s(PSTR("log wr pos ")); putxw(log_wr_pos); nl();
}

//=============================================================================

void dump_log()
{
uint8_t f;
	while(log_rd_pos != log_wr_pos) {
		f = log_get(log_rd_pos);
		putd2(log_rec[0]); putch_s(':');
		putd2(log_rec[1]); putch_s(' ');
		putd2(log_rec[2]); putch_s('-');
		putd2(log_rec[3]); putch_s('-');
		putd2(log_rec[4]); putch_s(' ');

		putx(log_rec[5]); putch_s(' ');
		putx(log_rec[6]); putch_s(' ');
		putx(log_rec[7]); putch_s(' ');
		putx(log_rec[8]);
		if(f) {
			putch_s('+');
		} else {
			putch_s('x');
		}
		nl();
		log_rd_pos += 16;
		if(log_rd_pos >= EEPROM_SIZE) log_rd_pos = 0;
	}
	puts_s(PSTR("ok\r\n"));
}


void clear_log()
{
	log_rd_pos = log_wr_pos;
	puts_s(PSTR("log cleared\r\n"));
}


void print_time()
{
	get_time();
	get_date();
	putd2(hour);	putch_s(':');
	putd2(minute);	putch_s(' ');
	putd2(day);	putch_s('-');
	putd2(month);	putch_s('-');
	putch_s('2');	putch_s('0');	putd2(year);	nl();
}


void set_date_time()
{
	puts_s(PSTR("hour:")); 	hour = rcvd2();		if(timeout) return;
	puts_s(PSTR("minute:"));minute = rcvd2();	if(timeout) return;
	puts_s(PSTR("day")); 	day = rcvd2();		if(timeout) return;
	puts_s(PSTR("month:")); month = rcvd2();	if(timeout) return;
	puts_s(PSTR("year: 20"));year = rcvd2();	if(timeout) return;
	set_time();
	set_date();
	puts_s(PSTR("ok\r\n"));
}


uint8_t check_data_cs()
{
uint8_t cs = (d[0] & 0x0f) ^ d[1] ^ d[2] ^ d[3];
	cs = (cs & 0x0f) ^ ((cs >> 4) & 0x0f) ^ 0x05;
	return ((d[0] & 0xf0) == (cs << 4)) ? 1 : 0;
}

//=============================================================================

int main(void)
{
uint16_t cnt;
uint8_t c;
	_WDR();
	WDTCR = 0x0f;

	DDRB = 0x30;	// portb 4,5 out
	PORTB = 0x30;

	puts_s(PSTR("Hello world!\r\n"));

	rf_init();
	setRADDR((uint8_t *)"serv1");
	payload = 4;
	channel = 10;
	config();

	i2c_init();
	clock_init();
	enable_rcvr();
	log_init();

	for(cnt=0; ; ) {
		if(!isSending() && dataReady()) {
			getData(d);
			puts_s(PSTR("rcvd: "));
			putx(d[0]); putch_s(' ');
			putx(d[1]); putch_s(' ');
			putx(d[2]); putch_s(' ');
			putx(d[3]);
			if(check_data_cs()) {
				log_add(d);
				putch_s('+');
			} else {
				putch_s('x');
			}
			nl();
			PORTB = PINB ^ (1 << LED);
		}

		delay(1);
		if(++cnt > 1000) {
			cnt = 0;
			PORTB = PINB ^ (1 << LED);
//putch_s(0x55);
		}

		c = rcvd();
		switch(c) {
	        	case '?':
			case '/': dump_log();	break;
			case 'c': clear_log();	break;
			case 't': print_time(); break;
			case 's': set_date_time(); break;
			case 0:   break;
			default: putch_s('?'); putch_s(c); nl();
		}
	}

	return 0;
}

/*=========================================================================*/
  
   
