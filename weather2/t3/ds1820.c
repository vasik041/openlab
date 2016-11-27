/*=========================================================================*/
/*
 * ds1820 operations
 * LZ, 2001-2016
 */
/*=========================================================================*/

#include "main.h"
#include "ds1820.h"

/*=========================================================================*/

volatile uint8_t tmpb[10];
volatile uint8_t devid[8];
volatile uint16_t temp;
volatile uint8_t crc;

/*=========================================================================*/

uint8_t ow_reset(void)
{
uint8_t presence;
	DQ0;
	_delay_us(500);
	DQ1;
	clrbit(DDRC,DQ);
	_delay_us(40);
	presence = DQX;
	_delay_us(450);
	setbit(DDRC,DQ);
	return presence ? 0 : 1;
}

/*=========================================================================*/

uint8_t read_bit(void)
{
uint8_t b;
	DQ0;
	_delay_us(10);
	DQ1;
	clrbit(DDRC,DQ);
	_delay_us(10);
	b = DQX;
	setbit(DDRC,DQ);
	return b ? 1 : 0;
}

/*=========================================================================*/

uint8_t read_byte(void)
{
uint8_t i,val,msk;
	val = 0;
	msk = 1;
	for(i=0; i < 8; i++) {
		if(read_bit()) { val |= msk; } else { val |= 0; }
		_delay_us(45);
		msk <<= 1;
	}
	return val;
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

//=========================================================================

void readTemp(void)
{
uint8_t k;
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

//=========================================================================

void readROM(void)
{
uint8_t k;
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
uint8_t k;

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
	for(k=0; k < 9; k++) {
		tmpb[k] = read_byte();
	}

	crc = 0;
	for(k=0; k < 8; k++) {
		Do_CRC(tmpb[k]);
	}

	if(crc != tmpb[8]) {
		temp = OW_BAD_CRC;
		return;
	}
	
	temp = (tmpb[0] + (tmpb[1] << 8));
}
