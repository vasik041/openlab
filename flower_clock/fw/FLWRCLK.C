//=============================================================================
//
// Flower Clock vers. 2.0
// This is free and unencumbered software released into the public domain.
// LZs, 2015
//
//=============================================================================

#include <msp430.h>
#include <stdint.h>
#include "flwrclk.h"

//#define	SHOW_TEMP

volatile uint8_t seconds;
volatile uint8_t minutes;
volatile uint8_t hours;
volatile uint8_t marker_55;
volatile uint8_t marker_AA;

volatile uint16_t t;
volatile uint8_t ft;


const uint8_t digs[] = {
	0, xA, xB, xA+xB,
	xC,xC+xA,xC+xB,xC+xB+xA,
	xD,xD+xA,xA+xB+xC+xD,xA+xB+xC+xD,
	xA+xB+xC+xD,xA+xB+xC+xD,xA+xB+xC+xD,xA+xB+xC+xD
};


void display(uint8_t n1,uint8_t n2,uint8_t n3,uint8_t n4)
{
	P1OUT |= LD;
	P1OUT |= LC; P1OUT &= ~LC;
	P1OUT = digs[n2];
	P1OUT |= LC; P1OUT &= ~LC;
	P1OUT = digs[n1];
	P1OUT |= LC; P1OUT &= ~LC;
	P1OUT = digs[n4];
	P1OUT |= LC; P1OUT &= ~LC;
	P1OUT = digs[n3];
	P1OUT |= LC; P1OUT &= ~LC;
}



void main(void)
{
uint8_t f;

	BCSCTL3 = 0;            // Use Xtal, lowest 1 pf stablity
	IFG1 &= ~OFIFG;		// Clear OSCFault flag
	_BIS_SR(SCG1 + SCG0);	// Stop DCO
	BCSCTL2 |= SELM_3 + DIVM_3; 	// MCLK = LFXT1/8
	WDTCTL = WDT_ADLY_1000;	// Interval timer 1s @ 32768hz

	IE1 |= WDTIE;           // Enable timer interrupts when GIE enabled
	_BIS_SR(GIE);           // Enable the interrupts

	P1DIR = 0x7f;		// All p1.x outputs except p1.7
	P1OUT = 0;
	P1SEL = 4;		// P1.2 pwm output

	CCR0 = 800;		// PWM Period
	CCTL1 = OUTMOD_6;
	CCR1 = 400;		// CCR1 PWM duty cycle
	TACTL = TASSEL_1 + MC_3; // ACLK,

	P1OUT = 0;
	P1OUT |= LC; P1OUT &= ~LC; // clear display
	P1OUT |= LC; P1OUT &= ~LC;
	P1OUT |= LC; P1OUT &= ~LC;
	P1OUT |= LC; P1OUT &= ~LC;

	if(marker_55 != 0x55 || marker_AA != 0xaa) {
		seconds = 0;
		minutes = 34;
		hours = 12;
		marker_55 = 0x55;
		marker_AA = 0xaa;
	}
	f = 1;
	t = 0;
	ft = 0;

	while (1) {
		if(!KEY) {
			uint16_t pressed = 0;
			while(!KEY) {
				if(pressed < MAX_KEY_DLY) pressed++;
			}
			while(!KEY);
			if(pressed > MAX_KEY_DLY/2) {
				if(++hours >= 24) hours = 0;
			} else {
				if(++minutes >= 60) minutes = 0;
			}
			f = 1;
		}
#ifdef SHOW_TEMP
		if(seconds == 30) {
			SD16CTL = SD16REFON + SD16SSEL_2;         // 1.2V ref, ACLK
			SD16INCTL0 = SD16INCH_6;                  // A6+/-
			SD16CCTL0 = SD16SNGL + SD16IE +  SD16DF;
			SD16CCTL0 |= SD16SC;
		}
#endif
		if(++seconds >= 60) {
			seconds = 0;
			f = 1;
			if(++minutes >= 60) {
				minutes = 0;
				if(++hours >= 24) hours = 0;
			}
		}
		if(f) {
			display(hours/10,hours%10,minutes/10,minutes%10);
			f = 0;
		}
		if(ft) {
			display(BLANK,t/10,t%10,BLANK);
			ft = 0;
		}
		_BIS_SR(LPM3_bits + GIE); // Enter LPM3
	}
}



interrupt(WDT_VECTOR) watchdog_timer(void)
{
	_BIC_SR_IRQ(LPM0_bits); // Clear LPM3 bits from 0(SR)
}


interrupt(SD16_VECTOR) sd_isr(void)
{
uint16_t temp = SD16MEM0;
	t = ((temp/55) * 100) / 135 - 273;
	ft = 1;
	SD16CTL = 0;
}

