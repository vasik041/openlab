/*=========================================================================*/
/*
 * Humidity & Temperature sensor with RF link
 * LZs,2016
 */
/*=========================================================================*/

#include "main.h"
#include "rf24l01.h"
#include "serial.h"
#include "dht.h"

extern uint8_t channel;
extern uint8_t payload;

uint8_t buf[4];

//=============================================================================
/*
 * delay for n ms
 */

void delay(uint16_t n)
{
	while(n--) {
		_WDR();
		_delay_us(1000);
	}
}

//=============================================================================

void make_cs()
{
uint8_t cs = buf[0] ^ buf[1] ^ buf[2] ^ buf[3];
	cs = (cs & 0x0f) ^ ((cs >> 4) & 0x0f) ^ 0x05;
	buf[0] |= (cs << 4);
}

//=============================================================================

void wdt_off(void)
{
	_WDR();
	MCUSR = 0;			// Clear WDRF in MCUSR 
	WDTCR |= (1<<WDCE) | (1<<WDE);	// Write logical one to WDCE and WDE
	WDTCR = 0;			// Turn off WDT
}

//=============================================================================

ISR (TIMER1_OVF_vect)
{
	OCR1B = 0xff;
}

//=============================================================================

uint8_t get_psu(void)
{
uint8_t aval;
	ADCSRA = 0x80;
        ADMUX = 6;
	ADCSRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADSC)) != 0);
	aval = (ADC >> 2);
	ADCSRA = 0;
	return aval;
}

//=============================================================================

int main(void)
{
volatile uint16_t i;
//	_WDR();
//	WDTCR = 0x0f;
	wdt_off();

	DDRA = 0xff-0x80;
	PORTA = 0xff-0x80;
	DDRB = 0xff;
	PORTB = 0xff;

//	DDRB = 0x30;	// portb 4,5 out
//	PORTB = 0x30;

	TCCR1A = 0; 
	TCCR1B = 0x0f;	// clk/16384
	OCR1C = 0xff;	// top
        TIMSK = 4;	// TOIE1
	MCUCR = 0x20;
	
//	puts_s(PSTR("Hello world!\r\n"));

	rf_init();
	setRADDR((uint8_t *)"clie1");
	payload = 4;
	channel = 10;
	config();

        dht_init();

	for( ; ; ) {
		LED1;
		buf[0] = 1;
		buf[1] = get_psu();
		i = dht_read();
		if(!i) {
			buf[2] = getTemperature();
			buf[3] = getHumidity();
//			putd_s(buf[2]);
//			putch_s(' ');
//			putd_s(buf[3]);
		} else {
			buf[2] = i;
			buf[3] = 0xff;
//			puts_s(PSTR("error"));
		}
		make_cs();
//		nl();

		setTADDR((uint8_t *)"serv1");
		send(buf);
		while(isSending());
		powerDown();
		LED0;

		_SEI();
		for(i=0; i < 600; i++) {
			_SLEEP();
		}
		_CLI();
	}

	return 0;
}

/*=========================================================================*/
  

