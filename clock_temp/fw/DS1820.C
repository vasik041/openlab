/*=========================================================================*/
/*
 * Reading ds1820
 * This is free and unencumbered software released into the public domain.
 * LZs,2005
 */
/*=========================================================================*/

#include "iotn26.h"
#include <ina90.h>
#include "types.h"

#include "ds1820.h"

/*=========================================================================*/

extern	UCHR	fInt;

UCHR tmpb[10];
UINT temp;
UCHR crc;

/*=========================================================================*/
/*=========================================================================*/

void u_delay(UCHR n)
{
	while(n--);
}

/*=========================================================================*/

UCHR ow_reset(void)
{
UCHR presence;
	DQ0;
	u_delay(100);
	DQ1;
	clrbit(DDRB,DQ);
	u_delay(4);
	presence = DQX;
	u_delay(90);
	setbit(DDRB,DQ);
	return presence ? 1 : 0;
}

/*=========================================================================*/

UCHR read_bit(void)
{
UCHR b;
	DQ0;
	_NOP();	_NOP();	_NOP(); _NOP();
	DQ1;
	clrbit(DDRB,DQ);
	_NOP(); _NOP();	_NOP();	_NOP();
	_NOP(); _NOP();	_NOP();	_NOP();
	b = DQX;
	setbit(DDRB,DQ);
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
		u_delay(3);
	}
	return value;
}

/*=========================================================================*/

void write_bit(UCHR b)
{
	DQ0;
	if(b) DQ1;
	u_delay(5);
	DQ1;
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

	u_delay(5);
}

/*=========================================================================*/

void readTemp(void)
{
UCHR k;
	if(ow_reset()) {
		temp = 2*99;
		return;
	}
	write_byte(0xcc);		// skip ROM
	write_byte(0x44);		// start conversion

	u_delay(100);

	if(ow_reset()) {
		temp = 2*98;
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
		temp = 2*97;
		return;
	}
	temp = (tmpb[0] + (tmpb[1] << 8));
}

/*=========================================================================*/

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

