/****************************************************************************/
/*
 * Osc
 * LZs,2009,2015
 * This is free and unencumbered software released into the public domain.
 */
/****************************************************************************/

#include "main.h"
#include "ili9341.h"
#include "keys.h"

#define	WAVE_LENGTH	120

#define	POS_MAX		5
int8_t pos;

int8_t gain;
int8_t offset;
int8_t fdiv;
uint8_t trigphase;
uint16_t trigger;


uint16_t wave[WAVE_LENGTH];


void get_wave()
{
uint16_t i;
	ADCSRA = 0x82+fdiv;
	if(trigphase) {
		for(i=0; i < 1000; i++) {
			if(get_aval(1) < trigger) break;
		}
		for(i=0; i < 1000; i++) {
			if(get_aval(1) > trigger) break;
		}
	} else {
		for(i=0; i < 1000; i++) {
			if(get_aval(1) > trigger) break;
		}
		for(i=0; i < 1000; i++) {
			if(get_aval(1) < trigger) break;
		}
	}
	for(i=0; i < WAVE_LENGTH; ) {
		wave[i++] = get_aval(1);
	}
}


void setup_timer1()
{
	TCCR1A = 0xf2; //a +0x10
	TCCR1B = 0x18+1;
	ICR1 = 1000;
	OCR1A = 500;
	OCR1B = 500;
}

//=============================================================================

void dec(void)
{
	switch(pos) {
		case 0:	if(gain > 0) gain--; break;
		case 1: if(offset > 0) offset--; break;
		case 2: if(fdiv > 0) fdiv--; break;
		case 3: trigphase ^= 1; break;
		case 4: if(trigger > 16) trigger-=16; break;
	}
}


void inc(void)
{
	switch(pos) {
		case 0:	if(gain < 7) gain++; break;
		case 1: if(offset < 100) offset++; break;
		case 2: if(fdiv < 5) fdiv++; break;
		case 3: trigphase ^= 1; break;
		case 4: if(trigger < 1023) trigger+=16; break;
	}
}

//=============================================================================

prog_char gs0[] = " 1v  ";
prog_char gs1[] = "500mv";
prog_char gs2[] = "250mv";
prog_char gs3[] = "100mv";
prog_char gs4[] = " 50mv";
prog_char gs5[] = " 25mv";
prog_char gs6[] = " 10mv";
prog_char gs7[] = "  5mv";

prog_char *gainstr[] PROGMEM = {
	gs0,gs1,gs2,gs3, gs4,gs5,gs6,gs7
};


prog_char ds0[] = " 1us";
prog_char ds1[] = " 2us";
prog_char ds2[] = " 4us";
prog_char ds3[] = " 8us";
prog_char ds4[] = "16us";
prog_char ds5[] = "32us";

prog_char *divstr[] PROGMEM = {
	ds0,ds1,ds2,ds3, ds4,ds5
};


void draw()
{
	drawFastVLine(240,0,240,ILI9341_LIGHTGREY);

	locXY(243,4);	puts1(PSTR("Gain"));
	if(pos == 0) inv(1);
	locXY(243,4+16); puts1((prog_char *)pgm_read_word(&gainstr[gain]));
	inv(0);
	drawFastHLine(243,4+32,74,ILI9341_LIGHTGREY);

	locXY(243,8+32); puts1(PSTR("Offset"));
	if(pos == 1) inv(1);
	locXY(243,8+48); putd(offset-50,3,0);
	inv(0);
	drawFastHLine(243,8+64,74,ILI9341_LIGHTGREY);

	locXY(243,12+64); puts1(PSTR("Div"));
	if(pos == 2) inv(1);
	locXY(243,12+80); puts1((prog_char *)pgm_read_word(&divstr[fdiv]));
	inv(0);
	drawFastHLine(243,12+96,74,ILI9341_LIGHTGREY);

	locXY(243,16+96); puts1(PSTR("TrgPh"));
	if(pos == 3) inv(1);
	locXY(243,16+112); putch('0'+trigphase);
	inv(0);
	drawFastHLine(243,16+128,74,ILI9341_LIGHTGREY);

	locXY(243,20+128); puts1(PSTR("TrgLvl"));
	if(pos == 4) inv(1);
	locXY(243,20+144); putd(trigger,3,0);
	inv(0);
}


void set_gain()
{
uint8_t g = gain;
        if(gain == 6) g = 7;
        else if(gain == 7) g = 6;
	if(g & 1) PORTD |= (1 << GAIN0); else PORTD &= ~(1 << GAIN0);
	if(g & 2) PORTD |= (1 << GAIN1); else PORTD &= ~(1 << GAIN1);
	if(g & 4) PORTD |= (1 << GAIN2); else PORTD &= ~(1 << GAIN2);
}

void set_offset()
{
	OCR1A = 10*offset;
}


void draw_wave()
{
int16_t i;
//	fillRect(0,0,239,239,ILI9341_DARKGREY);
//	drawFastHLine(0,120,240,ILI9341_LIGHTGREY);
	for(i=0; i < 240; i+=4) {
		drawPixel(i,0,ILI9341_LIGHTGREY);
		drawPixel(i,24,ILI9341_LIGHTGREY);
		drawPixel(i,48,ILI9341_LIGHTGREY);
		drawPixel(i,72,ILI9341_LIGHTGREY);
		drawPixel(i,96,ILI9341_LIGHTGREY);
		drawPixel(i,120-1,ILI9341_LIGHTGREY);
		drawPixel(i,120,ILI9341_LIGHTGREY);
		drawPixel(i,120+1,ILI9341_LIGHTGREY);
		drawPixel(i,144,ILI9341_LIGHTGREY);
		drawPixel(i,168,ILI9341_LIGHTGREY);
		drawPixel(i,192,ILI9341_LIGHTGREY);
		drawPixel(i,216,ILI9341_LIGHTGREY);
		drawPixel(i,239,ILI9341_LIGHTGREY);

		drawPixel(0,i,ILI9341_LIGHTGREY);
		drawPixel(24,i,ILI9341_LIGHTGREY);
		drawPixel(48,i,ILI9341_LIGHTGREY);
		drawPixel(72,i,ILI9341_LIGHTGREY);
		drawPixel(96,i,ILI9341_LIGHTGREY);
		drawPixel(120-1,i,ILI9341_LIGHTGREY);
		drawPixel(120,i,ILI9341_LIGHTGREY);
		drawPixel(120+1,i,ILI9341_LIGHTGREY);
		drawPixel(144,i,ILI9341_LIGHTGREY);
		drawPixel(168,i,ILI9341_LIGHTGREY);
		drawPixel(192,i,ILI9341_LIGHTGREY);
		drawPixel(216,i,ILI9341_LIGHTGREY);
		drawPixel(239,i,ILI9341_LIGHTGREY);
	}

	for(i=0; i < WAVE_LENGTH-1; i++) {
		drawLine(i+i,240-(wave[i]>>2),
		 	 2+i+i,240-(wave[i+1]>>2),
			 ILI9341_CYAN);
	}
}

void clear_wave()
{
int16_t i;
	for(i=0; i < WAVE_LENGTH-1; i++) {
		drawLine(i+i,240-(wave[i]>>2),
		 	 2+i+i,240-(wave[i+1]>>2),
			 ILI9341_DARKGREY);
	}
}

//=========================================================================

void save_params()
{
	eeprom_write_byte( (uint8_t*) 0, gain);
	eeprom_write_byte( (uint8_t*) 1, offset);
	eeprom_write_byte( (uint8_t*) 2, fdiv);
	eeprom_write_byte( (uint8_t*) 3, trigphase);
	eeprom_write_word( (uint16_t*)4, trigger);
}


void load_params()
{
	gain = eeprom_read_byte((uint8_t*)0);
	if(gain < 0 || gain > 7) gain = 0;
	offset = eeprom_read_byte((uint8_t*)1);
	if(offset < 0 || offset > 100) offset = 50;
	fdiv = eeprom_read_byte((uint8_t*)2);
	if(fdiv < 0 || fdiv > 5) fdiv = 2;
	trigphase = eeprom_read_byte((uint8_t*)3);
	if(trigphase != 0 && trigphase != 1) trigphase = 0;
	trigger = eeprom_read_word((uint16_t*)4);
	if(trigger > 1023) trigger = 512;
}

//=============================================================================

int main(void)
{
uint8_t c,oc,blink;
uint16_t dly;
	DDRD = (1 << GAIN0)|(1 << GAIN1)|(1 << GAIN2);
	DDRB = 1+2+4; // LED OC1A,B
	DDRC = 0;
	DIDR0 = 3;
	ADCSRA = 0x84;	// ADC
	_delay_ms(500);

	initLcd();
	fillScreen(ILI9341_DARKGREY);
	setRotation(1);
	setColor(ILI9341_WHITE,ILI9341_DARKGREY,2);

	setup_timer1();
	load_params();

	for(dly=1000,oc=0xff,blink = 0,pos=0; ; ) {
		c = get_key();
		if(oc != c) {
			switch(c) {
				case KEY_DOWN:	if(++pos > POS_MAX) pos=0; break;
				case KEY_UP:	if(--pos < 0) pos=POS_MAX-1; break;
				case KEY_LEFT:  dec(); break;
				case KEY_RIGHT: inc(); break;
			}
			draw();
			set_gain();
			set_offset();
			dly = 0;
		}
		if(++blink > 10) {
			PORTB = PINB ^ LED;
			blink = 0;
			clear_wave();
			get_wave();
			draw_wave();
		}
		if(dly < 1000) {
			if(++dly >= 1000) save_params();
		}
		_delay_ms(30);
		oc = c;
	}
	return 0; // unreached
}
