/*=========================================================================*/
/*
 * Temp meter
 * LZs,2010,2015
 */
/*=========================================================================*/

#include "tm.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>

#include "ds1820.h"
#include "display.h"

extern uint16_t temp;
uint16_t temp0;
uint16_t temp1;
uint16_t temp0o;
uint16_t temp1o;

/*=========================================================================*/

uint16_t get_temp(uint16_t val)
{
	return (val >> 4)*100 + (val & 0x0f)*100/16;
}

/*=========================================================================*/

void putt(uint16_t val)
{
	putd3(val/100,0);
	putch('.');
	val %= 100;
	putch('0'+val/10);
	putch('0'+val%10);
	putch('C');
}

/*=========================================================================*/

void putdt(uint16_t val,uint16_t val2)
{
	if(val == val2) return;
	if(val < val2) {
		putch('-');
		putt(val2-val);
	} else {
		putch('+');
		putt(val-val2);
	}
}

/*=========================================================================*/

int main(void)
{
	DDRA = 0xf0;
	DDRB = 0x30;
//	_WDR();
//	WDTCR = 0x0f;

	delay(500);
	display_init();

	temp0 = temp1 = temp0o = temp1o = 0;
	for( ; ; ) {
		readTemp();	temp0 = get_temp(temp);
		readTemp2();	temp1 = get_temp(temp);

		clrscr(0);
		locxy(0,0);	putt(temp0);
		locxy(8,0);	putdt(temp0,temp0o);
		locxy(0,1);	putt(temp1);
		locxy(8,1);	putdt(temp1,temp1o);

		delay(1000);
		temp0o = temp0;
		temp1o = temp1;
	}
	return 0;
}

/*=========================================================================*/

