//=========================================================================
//
// Power meter with MPPT
//
// LZ,2016
// This is free and unencumbered software released into the public domain.
//
//=========================================================================

#include "main.h"
#include "serial.h"
#include "keys.h"
#include "ili9341.h"

volatile uint16_t volt;
volatile uint16_t cur;
volatile uint32_t power;
volatile uint8_t tau;

volatile uint8_t rcvd_d;
volatile uint8_t rcvd_f;

volatile uint16_t pwr[20];

//=============================================================================

void get_meas(uint8_t n)
{
uint8_t j;
	volt = cur = 0;
	for(j=0; j < n; j++) {
		volt += ((uint32_t)get_aval(0) * 5000 / 1024);
		cur  += ((uint32_t)get_aval(1) * 5000 / 1024);
	}
	volt /= n;
	cur /= n;
	power = ((uint32_t)volt * (uint32_t)cur / 39000);
	cur /= 39;
}


void get_meas1()
{
uint16_t u0;
uint16_t i0;
uint8_t j;
	for(j=0; j < 10; j++) {
		get_meas(5);
		u0 = volt;
		i0 = cur;
		get_meas(5);
		if(ABS(volt-u0) < 10 && ABS(cur-i0) < 10) break;
		_delay_ms(1000);
	}
}

//=============================================================================

void send(uint8_t t1,uint16_t u1,uint16_t i1,uint16_t p1)
{
	cli();
	putd2_s(t1); putch_s(' ');
	putd4_s(u1); putch_s(' ');
	putd4_s(i1); putch_s(' ');
	putd4_s(p1);
	nl();
	sei();
}

//=============================================================================

void get_maxpwr()
{
uint16_t maxp,k,d;
uint8_t j;
	puts_s(PSTR("--\r\n"));
	for(j=0; j < 20; j++) {
		locXY(5,210);
		putd(j,2,0);

		OCR1B = 1+j*5;
		_delay_ms(1000);
		get_meas1();
		pwr[j] = power;
		send(1+j*5,volt,cur,power);
	}

	maxp = pwr[0];
	tau = 1;
	for(j=1; j < 20; j++) {
		if(pwr[j] > maxp) {
			maxp = pwr[j];
			tau = 1+5*j;
		}
	}

	d = 1;
	if(maxp < 25) k = 8;
	else if(maxp < 50) k = 4;
	else if(maxp < 100) k = 2;
	else if(maxp < 200) k = 1;
	else if(maxp < 400) { k = 1; d = 2; }
	else { k = 1; d = 4; }

	fillRect(0,0,205,240,ILI9341_DARKGREY);
        drawFastHLine(5,205,200,ILI9341_LIGHTGREY);
	drawFastVLine(5,5,200,ILI9341_LIGHTGREY);
	for(j=0; j < 200; j += 20) {
		drawPixel(5+j,206,ILI9341_LIGHTGREY);
		drawPixel(5+j,204,ILI9341_LIGHTGREY);
		drawPixel(6,5+j,ILI9341_LIGHTGREY);
		drawPixel(4,5+j,ILI9341_LIGHTGREY);
	}

	for(j=0; j < 20-1; j++) {
		drawLine(5+10*j,     205-pwr[j]*k/d,
		 	 5+10*(j+1), 205-pwr[j+1]*k/d,
			 ILI9341_CYAN);
	}

	OCR1B = tau;
}

//=============================================================================

ISR (INT0_vect)
{
uint8_t d,j,msk;
	if(rcvd_f) return;
	cli();
	_delay_us(105);
	d = 0;
	msk = 1;
	for(j=0; j < 8; j++) {
		_delay_us(55);
		if((PINB & 0x40) != 0) {
			d |= msk;
		}
		_delay_us(55);
		msk <<= 1;
	}
	_delay_us(200);

	rcvd_d = d;
	rcvd_f = 1;
	sei();
}

//=============================================================================

int main(void)
{
uint8_t c,oc,cnt;
	DDRA = 0xff-7;
	DDRB = 0xff-0x40;

	DIDR0 = 7;
	ADCSRA = 0x87;	// ADC
        ADMUX = 2;

	TCCR1A = 0x21;  // OC1B, PWM1B
	TCCR1B = 1;	// clk/8
	TCCR1D = 1;
	OCR1C = 100;	// top
	OCR1B = 1;	// value

	_delay_ms(500);

	initLcd();
	fillScreen(ILI9341_DARKGREY);
	setRotation(1);
	setColor(ILI9341_WHITE,ILI9341_DARKGREY,2);
	puts_s(PSTR("Hello world!\r\n"));

	MCUCR = 2;	// falling edge
	GIMSK = 0x40;	// int0	
	rcvd_f = 0;
	sei();

	tau = 1;
	for(cnt=oc=0;;) {
		c = get_key();
		if(c && c != oc) {
			if(c == KEY_1) {
				if(tau > 1) { tau--; cnt = 30; OCR1B = tau; }
			} else if(c == KEY_2) {
				if(tau < 99) { tau++; cnt = 30; OCR1B = tau; }
			} else if(c == KEY_3) {
//				if(tau > 10) { tau-=10; cnt = 30; OCR1B = tau; }
				get_maxpwr();
			} else if(c == KEY_4) {
//				if(tau < 89) { tau+=10; cnt = 30; OCR1B = tau; }
			}
		}
		oc = c;

		if(++cnt > 30) {
			get_meas(1);
			locXY(205,5);  putd(volt,4,0); putch('m'); putch('V');
			locXY(205,25); putd(cur,4,0); putch('m'); putch('A');
			locXY(205,45); putd(tau,4,0); putch('%');
			locXY(205,65); putd(power,4,0); putch('m'); putch('W');
			PORTB = PINB ^ LED;
			cnt = 0;
		}
		_delay_ms(10);

		cli();
		if(rcvd_f) {
			PORTB = PINB ^ LED;
			if(rcvd_d == 0) rcvd_d = 1;
			if(rcvd_d > 99) rcvd_d = 99;
			tau = rcvd_d;
			OCR1B = tau;
			get_meas1();
			send(tau,volt,cur,power);
			rcvd_f = 0;
		}
		sei();
	}
	return 0; // unreached
}

