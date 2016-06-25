/****************************************************************************/
/*
 * SinGen
 * This is free and unencumbered software released into the public domain.
 * LZs,2009
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

#include "display.h"

#include "sin.h"

//=========================================================================

uint8_t	 btn;

int32_t freq;
uint16_t freq_step;
int16_t vol;
uint16_t vol_step;
uint8_t	 vol_att;

uint8_t	 f;
uint8_t	 st;
uint8_t	 ost;
uint16_t t;
uint16_t t2;

//****************************************************************************
// 4us timer interrupt

ISR (TIMER0_OVF_vect)
{
//	TCNT0 = 256-64+24;

// r2,3,4,5
// r6,7,8,9
//zl r30
//zh r31

asm volatile(
	"push	r30         \n\t"
	"push	r31         \n\t"

       	"ldi	r30, 0xD8   \n\t"	//TCNT0=256-40
	"out	0x32, r30   \n\t"

	"add	r2,r6       \n\t"
	"adc	r3,r7       \n\t"
	"adc	r4,r8       \n\t"
	"adc	r5,r9       \n\t"

	"mov	r30,r4      \n\t"
	"mov	r31,r5      \n\t"

	"sbrc	r5,2        \n\t"
	"com	r30         \n\t"
	"sbrc	r5,2        \n\t"
	"com	r31         \n\t"

	"andi	r31,3       \n\t"
//	"ori	r31,4       \n\t" //0x400 sintab base
	"inc    r31         \n\t" //0x100
	"lpm                \n\t"

	"sbrc	r5,3        \n\t"
	"rjmp	11f         \n\t"

	"com	r0          \n\t"
	"cbi	59-32,7     \n\t" //PORTA.7
        "out	56-32,r0    \n\t" //PORTB
	"rjmp	12f         \n\t"
"11:                        \n\t"
	"sbi	59-32,7     \n\t" //PORTA.7
	"out	56-32,r0    \n\t" //PORTB
"12:                        \n\t"
	"pop    r31         \n\t"
	"pop	r30         \n\t"
	::);
}

//=========================================================================
// set attenuator

void set_att()
{
	cbi(PORTA,0);
	cbi(PORTA,1);
	cbi(PORTA,2);
	cbi(PORTA,3);
	switch(vol_att) {
		case 0:	sbi(PORTA,3);	break;
		case 1:	sbi(PORTA,2);	break;
		case 2:	sbi(PORTA,1);	break;
		case 3:	sbi(PORTA,0);	break;
		default:;
	}
}

//=========================================================================
// set volume

void set_vol()
{
	PORTC = (vol & 0xff);
	if(vol & 0x100) { sbi(PORTD,VOL8); } else { cbi(PORTD,VOL8); }
	if(vol & 0x200) { sbi(PORTD,VOL9); } else { cbi(PORTD,VOL9); }
}

//=========================================================================
// set frequency

void set_freq()
{
asm volatile(
	"push	r16	     \n\t"
	"lds	r16,freq     \n\t"
	"mov	r6,r16       \n\t"

	"lds	r16,freq+1   \n\t"
	"mov	r7,r16       \n\t"

	"lds	r16,freq+2   \n\t"
	"mov	r8,r16       \n\t"

	"lds	r16,freq+3   \n\t"
	"mov	r9,r16       \n\t"
	"pop	r16	     \n\t"
	::);
}

//=========================================================================

void putd(uint16_t n,uint8_t digs)
{
uint8_t i,p;
uint16_t m;

	switch(digs) {
		case 2: m = 10;   break;
		case 3: m = 100;  break;
		case 4: m = 1000; break;
		case 5:
		default: m = 10000;
	}

	p = 0;
	for(i=0; i < digs; i++) {
		if(m == 1) p = 1;
		if(n / m != 0 || p) {
			putch(n / m + '0');
			p = 1;
		} else {
			putch(' ');
		}
		n %= m;
		m /= 10;
	}
}

/*=========================================================================*/

prog_uint32_t	freq_steps[] = {
	100,		//0.1
	1000,           //1
	10000,          //10
	100000,         //100
	1000000         //1000
};

prog_char freq0[] = "0,1";
prog_char freq1[] = "  1";
prog_char freq2[] = " 10";
prog_char freq3[] = "100";
prog_char freq4[] = " 1K";

prog_char *freqX[] PROGMEM = {
	freq0,freq1,freq2,freq3,freq4
};

prog_uint16_t	vol_steps[] = {
	1,
	10,
	100
};

prog_char vol0[] = "10mV";
prog_char vol1[] = "0,1V";
prog_char vol2[] = "  1V";

prog_char *volX[] PROGMEM = {
	vol0,vol1,vol2
};

prog_char att0[] = "0db";
prog_char att1[] = "-20";
prog_char att2[] = "-40";
prog_char att3[] = "-60";

prog_char *attX[] PROGMEM = {
	att0,att1,att2,att3
};


//=========================================================================

void draw()
{
	switch(btn) {
		case P_FREQ:
			locxy(0,0);
			puts1(PSTR("Freq:"));
			locxy(5,0);
			putd(freq/1000,5);
			putch(',');
			putch('0' + (freq/100) % 10);
			locxy(5,0);
			break;

		case P_FREQ_STEP:
			locxy(13,0);
			puts1((prog_char *)pgm_read_word(&freqX[freq_step]));
			locxy(13,0);
			break;

		case P_VOL:
			locxy(0,1);
			puts1(PSTR("Vol:"));
			locxy(4,1);
			putd(vol,3);
			locxy(4,1);
			break;

		case P_VOL_STEP:
			locxy(8,1);
			puts1((prog_char *)pgm_read_word(&volX[vol_step]));
			locxy(8,1);
			break;

		case P_VOL_ATT:
			locxy(13,1);
			puts1((prog_char *)pgm_read_word(&attX[vol_att]));
			locxy(13,1);
			break;

		default:;
	}
}

//=========================================================================

void inc_param()
{
	switch(btn) {
		case P_FREQ:
			freq += pgm_read_dword(&freq_steps[freq_step]);
			if(freq > FREQ_MAX) freq = FREQ_MAX;
			set_freq();
			break;

		case P_FREQ_STEP:
			if(freq_step < NELEM(freq_steps)-1) freq_step++;
			else freq_step = 0;
			break;

		case P_VOL:
			vol += pgm_read_word(&vol_steps[vol_step]);
			if(vol > VOL_MAX) vol=VOL_MAX;
			set_vol();
			break;

		case P_VOL_STEP:
			if(vol_step < NELEM(vol_steps)-1) vol_step++;
			else vol_step = 0;
			break;

		case P_VOL_ATT:
			vol_att = (vol_att+1) & 3;
			set_att();
			break;

		default:;
	}
}

//=========================================================================

void dec_param()
{
	switch(btn) {
		case P_FREQ:
			freq -= pgm_read_dword(&freq_steps[freq_step]);
			if(freq < FREQ_MIN) freq = FREQ_MIN;
			set_freq();
			break;

		case P_FREQ_STEP:
			if(freq_step == 0) freq_step = NELEM(freq_steps)-1;
			else freq_step--;
			break;

		case P_VOL:
			vol -= pgm_read_word(&vol_steps[vol_step]);
			if(vol < VOL_MIN) vol=VOL_MIN;
			set_vol();
			break;

		case P_VOL_STEP:
			if(vol_step == 0) vol_step = NELEM(vol_steps)-1;
			else vol_step--;
			break;

		case P_VOL_ATT:
			vol_att = (vol_att-1) & 3;
			set_att();
			break;

		default:;
	}
}

//=========================================================================

void save_params()
{
	if(!t2) return;
	if(--t2 > 0) return;

	eeprom_write_dword((uint32_t*)0,freq);
	eeprom_write_word( (uint16_t*)4,freq_step);
	eeprom_write_word( (uint16_t*)6,vol) ;
	eeprom_write_byte( (uint8_t*) 8,vol_step);
	eeprom_write_byte( (uint8_t*) 9,vol_att);
}

//=========================================================================

void load_params()
{
	freq      = eeprom_read_dword((uint32_t*)0);
	freq_step = eeprom_read_word( (uint16_t*)4);
	vol       = eeprom_read_word( (uint16_t*)6);
	vol_step  = eeprom_read_byte( (uint8_t*) 8);
	vol_att   = eeprom_read_byte( (uint8_t*) 9);

	if(freq > FREQ_MAX || freq < FREQ_MIN) freq = 1000000;	//1KHz
//	if(freq > FREQ_MAX) freq = 50000000;

	if(freq_step > NELEM(freq_steps)-1) freq_step = 0;
	if(vol > VOL_MAX || vol < VOL_MIN) vol=VOL_MAX-1;
	if(vol_step > NELEM(vol_steps)-1) vol_step=0;
	if(vol_att > 3) vol_att=0;
}


//=========================================================================

int main(void)
{
	DDRA = 0x8f; //out, pa4..6 in
	DDRB = 0xff; //out
	DDRC = 0xff; //out
	DDRD = 0xff; //out

	delay(100);
	display_init();
	clrscr(1);

	TCCR0 = 1;
	TIMSK = 1;
	sei();

	f = 0;
	if((PINA & ENC0) == 0) st = 0; else st = 1;
	ost = st;
	t = 0;
	t2 = 0;

	load_params();
	set_vol();
	set_att();	
	set_freq();

	for(btn=0; btn < 5; btn++) draw();
	btn = P_FREQ;
	draw();

	for( ; ; ) {
		_WDR();

		if((PINA & KEY) == 0) {
			if(++btn >= 5) btn=0;
			f = 1;
			while((PINA & KEY) == 0);
		}

		if((PINA & ENC0) == 0) st = 0; else st = 1;
		if(st != ost) {
			if(st) {
				if((PINA & ENC1) != 0) inc_param(); else dec_param();
			} else {
				if((PINA & ENC1) != 0) dec_param(); else inc_param();
			}
			f = 1;
		}
		ost = st;

		if(f) {
			f = 0;
			t = 1000;
		}
		if(t) {
			if(--t == 0) {
				draw();
				t2 = 10000;
			}
		}
		save_params();
	}

	return 0;
}

//=========================================================================

