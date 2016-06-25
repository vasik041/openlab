/****************************************************************************/
/*
 * Clock + thermometer
 * LZs,2010-2012
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

#include "glcd.h"
#include "ds1820.h"

volatile int tcnt;
volatile uint8_t f1;
volatile int scale;
volatile int today[24];

//=========================================================================

void clrt()
{
int i;
	for(i=0; i < NELEM(today); i++) {
		today[i] = 0;
	}
	tcnt = 0;
	scale = 1;
	f1 = 1;
}

//=========================================================================

void addt(int t)
{
int i;
	if(++tcnt < 12*60 && !f1) return; // once in a hour
	tcnt = 0;
	
	for(i=0; i < NELEM(today)-1; i++) {
		today[i] = today[i+1];
	}

	t /= 16;
	if(t < -60) t = -60;
	if(t > 60) t = 60;
	today[NELEM(today)-1] = t;

	if(f1 && t != 60 && t != -60) {
		for(i=0; i < NELEM(today)-1; i++) {
			today[i] = t;
		}
		f1 = 0;
	}

	scale = 1;
	for(i=0; i < NELEM(today); i++) {
		if(today[i] > 30 || today[i] < -30) {
			scale = 2;		
		}
	}
}

//=========================================================================

void avg()
{
int i;
int val;
int y;
	val = 0;
	y = 0; 						// avg temp on top
	for(i=0; i < NELEM(today); i++) {
		val += today[i];
		if(i > NELEM(today)-5) {
			if(today[i]/scale > 30-8) y = 6; // avg temp on bottom
		}
	}

	locxy(11,y);
	if(val < 0) {
		val = -val;
		locxy(10,y);
		putc1('-');
	}
	putd3(val/NELEM(today),0);
	putc1('.');
	putc1('0' + (val % NELEM(today)) * 10 / NELEM(today));
	putc1(DEG);
}

//=========================================================================

void drawt()
{
int i;
int step = 128/NELEM(today);
int step2 = 1 + step/2;

	clear();
	drawline(1, 0,1,62,1);	// |
	drawline(0,15,2,15,1);	// +
	drawline(0,30,2,30,1);	// +
	drawline(0,45,2,45,1);	// +

	drawline(0,62,127,62,1); // ---
	
	for(i=0; i < NELEM(today)-1; i++) {
		drawline(step2+i*step,
				61,
			 	step2+i*step,
				63,
				1); 	// +

		drawline(step2+(i+1)*step,
				61,
				step2+(i+1)*step,
				63,
				1);     // +
	
		drawline(step2+i*step,
				30-today[i]/scale,
				step2+(i+1)*step,
				30-today[i+1]/scale,
				1);
	}
	show();	
	avg();
}

//=========================================================================

