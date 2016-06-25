/*=========================================================================*/
/*
 * Free Energy EXperimentator
 * This is free and unencumbered software released into the public domain.
 * LZs,2010-2016
 */
/*=========================================================================*/

#include "main.h"
#include "usart.h"

uint32_t freq;
uint32_t pulse;
uint8_t psu;
uint8_t load;

/*=========================================================================*/

uint16_t get_aval(uint8_t ch)
{
uint16_t aval;
        ADMUX = ch | 0x40;       	//vcc with ext cap on AREF
	_delay_ms(5);
	ADCSRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADSC)) != 0);
	aval = ADC;
	return aval;
}


uint16_t get_meas(uint8_t ch)
{
uint8_t i;
uint16_t val = 0;
	for(i=0; i < 8; i++) {
		val += get_aval(ch);
	}
	val >>= 3;
	return val;
}

//=========================================================================
// 1,8,64,256,1024
// 1 2 3  4   5

void setup_timer1(uint32_t period,uint32_t pulse)
{
uint8_t div = 1;
uint16_t t16 = 1;
uint16_t p16 = 1;
uint64_t t1 = SCALE_1(period);
uint64_t p1 = SCALE_1(pulse);

	if(t1 < 0x10000l) {
		div = 1;
		t16 = (uint16_t)t1;
		p16 = (uint16_t)p1;
	} else if(t1 >= 0x10000l && t1 < 0x80000l) {
		div = 2;
		t16 = (uint16_t)(t1 >> 3);
		p16 = (uint16_t)(p1 >> 3);
	} else if(t1 >= 0x80000l && t1 < 0x400000l) {
		div = 3;
		t16 = (uint16_t)(t1 >> 6);
		p16 = (uint16_t)(p1 >> 6);
	}

	TCCR1A = 0xf2; //a +0x10
	TCCR1B = 0x18+div;
	ICR1 = t16 ? (t16-1) : t16;
	OCR1A = p16 ? p16 : 1;
	OCR1B = p16 ? p16 : 1;
}

void setup_timer2a(uint8_t v)
{
	OCR2A = 255-v;
}

void setup_timer2b(uint8_t v)
{
	OCR2B = 255-v;
}

//=========================================================================

void send_ok(uint8_t b)
{
	USART_Transmit('o');
	USART_Transmit('k');
	thex(b);
	USART_Transmit('\n');
}


void set_freq(void)
{
uint8_t h,l,u,v;
	l = rhex();	if(USART_Timeout()) return;
	h = rhex();	if(USART_Timeout()) return;
	u = rhex();	if(USART_Timeout()) return;
	v = rhex();	if(USART_Timeout()) return;
	freq = (uint32_t)l + ((uint32_t)h << 8) +
		 ((uint32_t)u << 16) + ((uint32_t)v << 24);
	setup_timer1(freq,pulse);
	send_ok(l^h^u^v);
}


void set_pulse_length(void)
{
uint8_t h,l,u,v;
	l = rhex();     if(USART_Timeout()) return;
	h = rhex();	if(USART_Timeout()) return;
	u = rhex();	if(USART_Timeout()) return;
	v = rhex();	if(USART_Timeout()) return;
	pulse = (uint32_t)l + ((uint32_t)h << 8) +
		 ((uint32_t)u << 16) + ((uint32_t)v << 24);
	setup_timer1(freq,pulse);
	send_ok(l^h^u^v);
}


void set_psu_voltage(void)
{
uint8_t u;
	u = rhex();
	if(USART_Timeout()) return;
	psu = u;
	setup_timer2b(u);
	send_ok(u^0x55);
}


void get_power_out(void)
{
uint16_t u,c;
	u = get_meas(0);
	c = get_meas(1);
	USART_Transmit('w');
	thex(u >> 8);
	thex(u);
	thex(c >> 8);
	thex(c);
	thex(u ^ (u >> 8) ^ c ^ (c >> 8) );
	USART_Transmit('\n');
}


void set_load(void)
{
uint8_t l;
	l = rhex();
	if(USART_Timeout()) return;
	load = l;
	setup_timer2a(l);
	send_ok(l^0x55);
}


void get_power_in(void)
{
uint16_t u,c;
	u = get_meas(2);
	c = get_meas(3);
	USART_Transmit('y');
	thex(u >> 8);
	thex(u);
	thex(c >> 8);
	thex(c);
	thex(u ^ (u >> 8) ^ c ^ (c >> 8) );
	USART_Transmit('\n');
}


uint8_t xor32(uint32_t x)
{
uint8_t a = x & 0xff;
uint8_t b = (x >> 8) & 0xff;
uint8_t c = (x >> 16) & 0xff;
uint8_t d = (x >> 24) & 0xff;
	return a ^ b ^ c ^ d;
}


void thex32(uint32_t x)
{
	thex(x >> 24);
	thex(x >> 16);
	thex(x >> 8);
	thex(x);
}


void get_settings(void)
{
	USART_Transmit('z');
	thex32(freq);
	thex32(pulse);
	thex(psu);
	thex(load);
	thex(xor32(freq) ^ xor32(pulse) ^ psu ^ load);
	USART_Transmit('\n');
}

/*=========================================================================*/

int main(void)
{
uint8_t c;
uint16_t cnt = 0;
	DDRC = 0;	// portc in
	PORTC = 0;

	DDRB = 0x0f;	// LED, OC1A, OC1B, OC2A
	DDRD = 0xe8;

	DIDR0 = 0x0f;
	ADCSRA = 0x87;	// ADC enable, /128

	TCCR2A = 0xf1;
	TCCR2B = 1;

	freq = 20000000;
	pulse = 10000;
	setup_timer1(20000000,10000);
	psu = load = 50;
	setup_timer2a(50);
	setup_timer2b(50);

	USART_Init(BR_9600);

	for( ; ; ) {
		if(USART_Ready()) {
			c = USART_Receive(STD_TIMEOUT);
			switch(c) {
				case 'f': set_freq();		break;
				case 't': set_pulse_length();	break;
				case 'u': set_psu_voltage();	break;
				case 'p': get_power_in();	break;
				case 'l': set_load();		break;
				case 'e': get_power_out();	break;
				case 's': get_settings();	break;
				default:;
			}
		}

		_delay_ms(1);
		if(++cnt > 300) {
			PORTB = PINB ^ 1;
			cnt = 0;
		}
	}

	return 0;
}

/*=========================================================================*/


