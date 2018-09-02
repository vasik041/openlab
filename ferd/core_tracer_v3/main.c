/*=========================================================================*/
/*
 * Core tracer based on Free Energy EXperimentator board
 * LZs,2018
 */
/*=========================================================================*/

#include "main.h"
#include "usart.h"

uint32_t tau;
uint32_t pulse;
uint8_t dac;

uint8_t buf[8];
uint8_t pos;

/*=========================================================================*/
/*
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
*/

//=========================================================================
// 1,8,64,256,1024
// 1 2 3  4   5

uint8_t div = 1;

void setup_timer1(uint32_t period,uint32_t pulse)
{
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

uint32_t scan_num()
{
uint8_t i;
uint32_t n = 0;
    for(i=1; buf[i] != '\0'; i++) {
	n *= 10;
	n += (buf[i] - '0');
    }
    return n;
}

void puts1(const prog_char *s)
{
char c;
    for( ; ; s++) {
	c = pgm_read_byte(s);
	if(c == '\0') break;
	USART_Transmit(c);
    }
}


void putd(uint32_t n)
{
uint32_t m;
uint8_t i,p;
    if(n > 99999) n = 99999;
    m = 10000;
    p = 0;
    for(i=0; i < 5; i++) {
	if(m == 1) p = 1;
	if(n / m != 0 || p) {
	    USART_Transmit(n / m + '0');
	    p = 1;
	}
	n %= m;
	m /= 10;
    }
}


void nl(void)
{
    USART_Transmit('\r');
    USART_Transmit('\n');
}


void print_ft()
{
    puts1(PSTR("t=")); putd(tau/1000000);
    puts1(PSTR(" p=")); putd(pulse/1000);
    puts1(PSTR(" div=")); putd(div);
    puts1(PSTR(" dac=")); putd(dac);
    nl();
}


void set_tau(void)
{
    uint32_t temp = scan_num();
    if(temp < 5 || temp > 50) {
	puts1(PSTR("?"));
	return;
    }
    tau = temp * 1000000;
    setup_timer1(tau,pulse);
}


void set_pulse_length(void)
{
    uint32_t temp = scan_num();
    if(temp < 1 || temp > 100) {
	puts1(PSTR("?"));
	return;
    }
    pulse = temp * 1000;
    setup_timer1(tau,pulse);
}


void set_dac_voltage(void)
{
    uint32_t temp = scan_num();
    if(temp > 255) {
	puts1(PSTR("?"));
	return;
    }
    dac = temp;
    setup_timer2b(dac);
}


uint16_t get_dt(void)
{
uint32_t i;
uint16_t delta = 0;
    for(;;) {				//wait for 1
	if((PINB & 2) != 0) break;
    }
    for(;;) {				//wait for 0
	if((PINB & 2) == 0) break;
    }
    _delay_us(3);

//    PORTB = PINB & 0xfe;
    for(i=0; i < 1000000; i++) {	//wait for comp 0
	if((PIND & 4) == 0) break;
    }
    if((PIND & 4) != 0) return 0;

    delta = TCNT1;
//    PORTB = PINB | 1;
    return delta;
}


void get_pulse_len(void)
{
//    for(;;) {
        uint16_t delta = get_dt();
//    }
    nl();
    puts1(PSTR("d="));
    putd(delta);
}


void get_meas(void)
{
uint16_t delta;
uint16_t i;
    nl();
    puts1(PSTR("@"));
    for(i=5; i < 255; i+=10) {
	setup_timer2b(i);
	_delay_ms(300);

	delta = get_dt();
	puts1(PSTR("dac="));
	putd(i);
	puts1(PSTR(" delta="));
	putd(delta);
	nl();

	if(!delta) break;
    }
    setup_timer2b(dac);
    puts1(PSTR("-"));
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

	tau = 20000000;
	pulse = 10000;
	setup_timer1(20000000,10000);
	dac = 50;
	setup_timer2a(50);
	setup_timer2b(50);

	USART_Init(BR_9600);
	puts1(PSTR("Hello tracer\r\n"));
	pos = 0;

	for( ; ; ) {
		if(USART_Ready()) {
			c = USART_Receive(STD_TIMEOUT);
			if(c != '\r' && pos < sizeof(buf)-1) {
			    USART_Transmit(c);
			    buf[pos++] = c;
			} else { 
			    buf[pos++] = '\0';
    			    switch(buf[0]) {
				case 't': set_tau();		break;
				case 'p': set_pulse_length();	break;
				case 'd': set_dac_voltage();	break;
				case 'x': get_pulse_len();	break;
				case 'm': get_meas();		break;
				default:;
			    }
			    nl();
			    print_ft();
			    pos = 0;
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


