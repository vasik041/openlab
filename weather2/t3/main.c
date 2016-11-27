/*=========================================================================*/
/*
 * 3x temp sensor with LCD and RF channel
 * LZs,2016
 */
/*=========================================================================*/

#include "main.h"
#include "display.h"
#include "ds1820.h"
#include "rf24l01.h"

extern volatile uint8_t tmpb[];
extern volatile uint8_t devid[];
extern volatile uint16_t temp;

#define	LOG_LEN		12	// back log len
#define LOG_FREQ	(60*2)	// log every hour

volatile uint16_t tlog[LOG_LEN];
volatile uint8_t tcnt;
volatile uint8_t tpos;
volatile uint8_t tfiled;

uint16_t tmin;
uint16_t tmax;
uint16_t tavg;

extern uint8_t channel;
extern uint8_t payload;

uint8_t buf[4];

/*=========================================================================*/

prog_uint8_t ids[] = {
	0x51,0x8a,0x1e,0x02,0,0,0x13,
	0x2b,0x85,0x1e,0x02,0,0,0x2b,
	0xe1,0xad,0x1e,0x02,0,0,0xbd
};


void set_id(int8_t n)
{
uint8_t i;
	devid[0] = 0x28;
        for(i=0; i < 7; i++) {
		devid[i+1] = pgm_read_byte(ids+n*7+i);
	}
}

//=========================================================================

int16_t gett(int16_t t)
{
uint8_t s = 0;
	if(t < 0) {
		t = -t;
		s = 1;
	}
	t = (t >> 4)*10 + (t & 0x0f)*10/16;
	return s ? (-t) : t;
}


void putt(int16_t val)
{
	if(val < 0) {
		val = -val;
		putch('-');
	}
	putd3(val/10,0);
	putch('.');
	putch('0' + val%10);
}

/*=========================================================================*/
/*
void get_id()
{
uint8_t i;
	readROM();
	if(!temp) {
		locxy(0,0);
		for(i=0; i < 8; i++)
			putx(tmpb[i]);

		for(i=0; i < 8; i++)
			devid[i] = tmpb[i];
		readTempBus();
		locxy(0,1);
		putt(temp);
	}
}
*/

/*=========================================================================*/

uint8_t get_psu()
{
uint8_t aval;
	PRR &= ~1;
	ADCSRA = 0x87;		// init ADC
        ADMUX = 0x45;
	ADCSRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADIF)) == 0);
	aval = ADC >> 2;
	ADCSRA = 0;		// turn off ADC
	PRR |= 1;
	return aval;
}

/*=========================================================================*/

void init_log(void)
{
uint8_t i;
	tpos = 0;
	tfiled = 0;
	tcnt = LOG_FREQ;
	tmin = tmax = tavg = 0;
	for(i=0; i < LOG_LEN; i++)
		tlog[i] = 0;
}


void make_cs()
{
uint8_t cs = buf[0] ^ buf[1] ^ buf[2] ^ buf[3];
	cs = (cs & 0x0f) ^ ((cs >> 4) & 0x0f) ^ 0x05;
	buf[0] |= (cs << 4);
}


void log_temp(int16_t t)
{
	if(++tcnt < LOG_FREQ) return;
	tcnt = 0;

	buf[0] = 2;
	buf[1] = get_psu();
	buf[2] = t;
	buf[3] = (t >> 8);

	make_cs();
	setTADDR((uint8_t *)"serv1");
	send(buf);
	while(isSending());
	powerDown();

	tlog[tpos] = t;
	if(++tpos >= LOG_LEN) {
		tpos = 0;
		tfiled = 1;
	}
}


void calc_stat(void)
{
uint8_t i,m,n;
	if(tfiled) m = LOG_LEN; else m = tpos;
	tmin = tlog[0];
	tmax = tlog[0];
	for(i=1; i < m; i++) {
		if(tlog[i] < tmin) tmin = tlog[i];
		if(tlog[i] > tmax) tmax = tlog[i];
	}

	tavg = 0;
	for(i=n=0; i < m; i++) {
		tavg += tlog[i];
		n++;
	}
	if(n) tavg /= n;
}

/*=========================================================================*/
/*
ISR (TIMER1_OVF_vect)
{
	_WDR();
}

void timer_delay()
{
	TCCR1A = 0;
	TCCR1B = 1;
	TIMSK1 = 1;
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sei();
	sleep_cpu();	sleep_cpu();	sleep_cpu();
	sleep_cpu();	sleep_cpu();	sleep_cpu();
	sleep_cpu();	sleep_cpu();	sleep_cpu();
	sleep_cpu();
	sleep_disable();
	cli();
}
*/
/*=========================================================================*/

int main(void)
{
uint8_t f;
	DDRB = 0x3f;
	PORTB = 0xff;

	DDRC = 0x1e;	// pc0 = dq, pc5,6,7 in
	PORTC = 0xff-0x20;
	DIDR0 = 0x20;

	DDRD = 0xff;
	PORTD = 0xff;

	PRR = 0xe7;

	WDTCSR |= (1<<WDCE) | (1<<WDE);
	WDTCSR = (1<<WDE) | (1<<WDP3) | (1<<WDP2) | (1<<WDP0); //2s

	delay(300);
	display_init();
	init_log();
	
	rf_init();
	setRADDR((uint8_t *)"clie1");
	payload = 4;
	channel = 10;
	config();
	
	clrbit(PORTB,0); // LED off

	for(f=0; ; f ^= 1) {
//		get_id();

		_WDR();
//		setbit(PORTB,0); // LED on
		set_id(0); readTempBus(); temp = gett(temp);
		locxy(0,0); putt(temp); 
//		clrbit(PORTB,0); // LED off

		set_id(1); readTempBus(); temp = gett(temp);
		locxy(5,0);  putt(temp);
		log_temp(temp);

		set_id(2); readTempBus(); temp = gett(temp);
		locxy(10,0); putt(temp); 

		calc_stat();
		locxy(0,1);  putt(tmin);
		locxy(5,1);  putt(tavg);
		locxy(10,1);  putt(tmax);

		locxy(15,1);
		putch(f ? '.' : ' ');

     		CLKPR = 0x80;
     		CLKPR = 8;
		delay(100);
//		timer_delay();
		CLKPR = 0x80;
		CLKPR = 0;
	}

	return 0;
}

/*=========================================================================*/



  
