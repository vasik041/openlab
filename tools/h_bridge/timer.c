/*=========================================================================*/
/**
 * Pulse generator using atm328p/Timer1
 *
 * FE R&D group,2014
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

#include "main.h"
#include "timer.h"


volatile uint32_t tau;		// in nanosecinds
volatile uint16_t duty;		// 0..999 (0..99.9%)
volatile uint16_t duty2;        // 0..999 (0..99.9%)

//=========================================================================
// 1,8,64,256,1024
// 1 2 3  4   5

void setup_timer1()
{
uint8_t div = 1;
uint16_t t16 = 1;
uint64_t t1 = SCALE_1(tau);

	if(t1 < 0x10000l) {
		div = 1;				// /1
		t16 = (uint16_t)t1;
	} else if(t1 >= 0x10000l && t1 < 0x80000l) {
		div = 2;                                // /8
		t16 = (uint16_t)(t1 >> 3);
	} else if(t1 >= 0x80000l && t1 < 0x400000l) {
		div = 3;                                // /64
		t16 = (uint16_t)(t1 >> 6);
	} else if(t1 >= 0x400000l) {
		div = 4;                                // /256
		t16 = (uint16_t)(t1 >> 8);
	}

	TCCR1A = 0xf2; //a +0x10
	TCCR1B = 0x18+div;
	ICR1 = t16 ? (t16-1) : t16;
	OCR1A = (uint16_t)((uint32_t)duty  * (uint32_t)t16 / 1000l);
	OCR1B = (uint16_t)((uint32_t)duty2 * (uint32_t)t16 / 1000l);
}

//=========================================================================

