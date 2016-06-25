/****************************************************************************/
/*
 * ds1820 operations
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

#include "ds1820.h"

/*=========================================================================*/

volatile UCHR tmpb[10];
volatile UCHR devid[8];
volatile UINT temp;
volatile UCHR crc;

/*=========================================================================*/

UCHR ow_reset(void)
{
UCHR presence;

	DQ0;
	_delay_us(500);
	DQ1;
	clrbit(DDRD,DQ);
	_delay_us(40);
	presence = DQX;
	_delay_us(450);
	setbit(DDRD,DQ);
	return presence ? 0 : 1;
}

/*=========================================================================*/

UCHR read_bit(void)
{
UCHR b;
	DQ0;
	_delay_us(10);
	DQ1;
	clrbit(DDRD,DQ);
	_delay_us(10);
	b = DQX;
	setbit(DDRD,DQ);
	return b ? 1 : 0;
}

/*=========================================================================*/

UCHR read_byte(void)
{
UCHR i;
UCHR value,msk;
	value = 0;
	for(i=0; i < 8; i++) {
		msk = (1 << i);
		if(read_bit()) value |= msk; else value |= 0;
		_delay_us(45);
	}
	return value;
}

/*=========================================================================*/

void write_bit(UCHR b)
{
	DQ0;
	_delay_us(15);
	if(b) DQ1;
	_delay_us(45);
	DQ1;
	_delay_us(1);
}

/*=========================================================================*/

void write_byte(UCHR b)
{
	write_bit(b & 1);
	write_bit(b & 2);
	write_bit(b & 4);
	write_bit(b & 8);

	write_bit(b & 0x10);
	write_bit(b & 0x20);
	write_bit(b & 0x40);
	write_bit(b & 0x80);

	_delay_us(25);
}

//=========================================================================

void readTemp(void)
{
UCHR k;
	temp = 0;
	if(!ow_reset()) {
		temp = OW_NO_DEVICE;
		return;
	}
	write_byte(0xcc);		// skip ROM
	write_byte(0x44);		// start conversion

	_delay_us(500);

	if(!ow_reset()) {
		temp = OW_NO_DEVICE2;
		return;
	}
	write_byte(0xcc);		// skip ROM
	write_byte(0xbe);		// read scratch pad
	for(k=0; k < 9; k++)
		tmpb[k] = read_byte();

	crc = 0;
	for(k=0; k < 8; k++)
		Do_CRC(tmpb[k]);

	if(crc != tmpb[8]) {
		temp = OW_BAD_CRC;
		return;
	}
	temp = (tmpb[0] + (tmpb[1] << 8));
}

/*=========================================================================*/
/*
 * Посчитать 1байт к crc
 */

void Do_CRC(UCHR d)
{
UCHR i,a,b0;
	for(i=0; i < 8; i++) {
		a = d ^ crc;
 		if((a & 1) == 0) {
			a = crc;
			b0 = 0;
		} else {
			a = crc ^ 0x18;
			b0 = 1;
		}
		crc = (a >> 1) | (b0      ? 0x80 : 0);
		d   = (d >> 1) | ((d & 1) ? 0x80 : 0);
	}
}

/*=========================================================================*/
/*
 * read temperature with 0.1' accuracy
 */

void readTemp1(void)
{
	readTemp();
	temp *= 50;
	temp += (75 - 100 * tmpb[6] / tmpb[7]);
}

//=========================================================================

void readROM(void)
{
UCHR k;
	temp = 0;
	if(!ow_reset()) {
		temp = OW_NO_DEVICE;
		return;
	}

	write_byte(0x33);		// read ROM
	for(k=0; k < 8; k++)
		tmpb[k] = read_byte();

	crc = 0;
	for(k=0; k < 7; k++)
		Do_CRC(tmpb[k]);

	if(crc != tmpb[7]) {
		temp = OW_BAD_CRC;
		return;
	}
}

//=========================================================================

void readTempBus()
{
UCHR k;
	temp = 0;
	if(!ow_reset()) {
		temp = OW_NO_DEVICE;
		return;
	}
	write_byte(0xcc);		// skip ROM
	write_byte(0x44);		// start conversion 

	_delay_us(500);

	if(!ow_reset()) {
		temp = OW_NO_DEVICE2;
		return;
	}

	write_byte(0x55);		// match ROM
	write_byte(devid[0]);	
	write_byte(devid[1]);	
	write_byte(devid[2]);	
	write_byte(devid[3]);	
	write_byte(devid[4]);	
	write_byte(devid[5]);	
	write_byte(devid[6]);	
	write_byte(devid[7]);	

	write_byte(0xbe);		// read scratch pad
	for(k=0; k < 9; k++)
		tmpb[k] = read_byte();

	crc = 0;
	for(k=0; k < 8; k++)
		Do_CRC(tmpb[k]);

	if(crc != tmpb[8]) {
		temp = OW_BAD_CRC;
		return;
	}
	
	temp = (tmpb[0] + (tmpb[1] << 8));
}
