/*=========================================================================*/
/*
 * Free Energy EXperimentator
 * LZs,2010-2016
 */
/*=========================================================================*/

#include "main.h"
#include "usart.h"

uint32_t freq;
uint32_t pulse;
uint8_t psu;
uint8_t load;

uint8_t buf[32];
uint8_t pos;
uint8_t ready;

/*=========================================================================*/

void poll()
{
uint8_t c;
    if(ready) return;
    if(!USART_Ready()) return;
    c = UDR0;
    if(c == '\n') {
        ready = 1;
        return;
    }
    if(pos < sizeof(buf)-1) {
	buf[pos++] = c;
    }
}


void tx(uint8_t d)
{
    while ( !( UCSR0A & (1<<UDRE0)) )
	poll();
    UDR0 = d;
}


void delay(uint16_t n)
{
uint16_t i;
    for(i=0; i < n; i++) {
	_delay_ms(1);
	poll();
    }
}

/*=========================================================================*/

uint16_t get_aval(uint8_t ch)
{
uint16_t aval;
        ADMUX = ch | 0x40;       	//vcc with ext cap on AREF
	delay(5);
	ADCSRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADSC)) != 0)
	    poll();
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

uint8_t rhex(void)
{
uint8_t h,l;
    h = buf[pos++];
    l = buf[pos++];
    return (h2d(h) << 4) + h2d(l);
}


void set_freq(void)
{
uint8_t h,l,u,v,cs;
	l = rhex();
	h = rhex();
	u = rhex();
	v = rhex();
	cs = rhex();
	if((l ^ h ^ u ^ v) != cs) return;
	freq = (uint32_t)l + ((uint32_t)h << 8) +
		 ((uint32_t)u << 16) + ((uint32_t)v << 24);
	setup_timer1(freq,pulse);
}


void set_pulse_length(void)
{
uint8_t h,l,u,v,cs;
	l = rhex();
	h = rhex();
	u = rhex();
	v = rhex();
	cs = rhex();
	if((l ^ h ^ u ^ v) != cs) return;
	pulse = (uint32_t)l + ((uint32_t)h << 8) +
		 ((uint32_t)u << 16) + ((uint32_t)v << 24);
	setup_timer1(freq,pulse);
}


void set_psu_voltage(void)
{
uint8_t u,cs;
	u = rhex();
	cs = rhex();
	if((u ^ 0x55) != cs) return;
	psu = u;
	setup_timer2b(u);
}

void set_load(void)
{
uint8_t l,cs;
	l = rhex();
	cs = rhex();
	if((l ^ 0x55) != cs) return;
	load = l;
	setup_timer2a(l);
}

/*=========================================================================*/

void thex(uint8_t b)
{
    tx(d2h(b >> 4));
    tx(d2h(b));
}


void send_power(void)
{
uint16_t u,c,u2,c2;
	u = get_meas(0);	//out
	c = get_meas(1);
	u2 = get_meas(2);	// in
	c2 = get_meas(3);

	tx('p');
	thex(u >> 8);
	thex(u);
	thex(c >> 8);
	thex(c);

	thex(u2 >> 8);
	thex(u2);
	thex(c2 >> 8);
	thex(c2);

	thex(u ^ (u >> 8) ^ c ^ (c >> 8) ^ u2 ^ (u2 >> 8) ^ c2 ^ (c2 >> 8));
	tx('\n');
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


void send_settings(void)
{
	tx('z');
	thex32(freq);
	thex32(pulse);
	thex(psu);
	thex(load);
	thex(xor32(freq) ^ xor32(pulse) ^ psu ^ load);
	tx('\n');
}


void process()
{
    if(!ready) return;
    pos = 0;
    switch(buf[pos++]) {
	case 'f': set_freq();		break;
	case 't': set_pulse_length();	break;
	case 'u': set_psu_voltage();	break;
	case 'l': set_load();		break;
	default:;
    }
    pos = 0;
    ready = 0;
    PORTB = PINB ^ 1;
}

/*=========================================================================*/

int main(void)
{
uint16_t cnt1 = 0;
uint16_t cnt10 = 0;

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
	ready = 0;
	pos = 0;

	for( ; ; ) {
		poll();
		process();
		if(++cnt1 > 1000) {
		    send_power();
		    cnt1 = 0;
		    PORTB = PINB ^ 1;
		}
		if(++cnt10 > 10000) {
		    send_settings();
		    cnt10 = 0;
		}
		_delay_ms(1);
	}

	return 0;
}

/*=========================================================================*/


