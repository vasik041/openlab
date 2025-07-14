/*=========================================================================*/
/*
 * Reading ds1820
 * This is free and unencumbered software released into the public domain.
 * LZs,2009,2015
 */
/*=========================================================================*/

#include "tm.h"

#include <avr/io.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>

#include "ds1820.h"

/*=========================================================================*/

uint8_t tmpb[10];
uint16_t temp;
uint8_t crc;

/*=========================================================================*/

uint8_t ow_reset(void)
{
uint8_t presence;
	DQ0;
	_delay_us(500);
	DQ1;
	cbi(DDRA,DQ);
	_delay_us(40);
	presence = DQX;
	_delay_us(450);
	sbi(DDRA,DQ);
	return presence ? 1 : 0;
}

/*=========================================================================*/

uint8_t read_bit(void)
{
uint8_t b;
	DQ0;
	_delay_us(10);
	DQ1;
	cbi(DDRA,DQ);
	_delay_us(10);
	b = DQX;
	sbi(DDRA,DQ);
	return b ? 1 : 0;
}

/*=========================================================================*/

uint8_t read_byte(void)
{
uint8_t i;
uint8_t value,msk;
	value = 0;
	for(i=0; i < 8; i++) {
		msk = (1 << i);
		if(read_bit()) value |= msk; else value |= 0;
		_delay_us(45);
	}
	return value;
}

/*=========================================================================*/

void write_bit(uint8_t b)
{
	DQ0;
	_delay_us(15);
	if(b) DQ1;
	_delay_us(45);
	DQ1;
	_delay_us(1);
}

/*=========================================================================*/

void write_byte(uint8_t b)
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

/*=========================================================================*/

void readTemp(void)
{
uint8_t k;
	if(ow_reset()) {
		temp = 16*99;
		return;
	}
	write_byte(0xcc);		// skip ROM
	write_byte(0x44);		// start conversion

	_delay_us(500);

	if(ow_reset()) {
		temp = 16*98;
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
		temp = 16*97;
		return;
	}
	temp = (tmpb[0] + (tmpb[1] << 8));
}

/*=========================================================================*/

void Do_CRC(uint8_t d)
{
uint8_t i,a,b0;
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
