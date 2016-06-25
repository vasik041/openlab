/****************************************************************************/
/*
 * Osc
 * LZs,2009,2015
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
#include <util/delay.h>
#include "usart.h"

#define	WAVE_LENGTH	1000

uint16_t wave[WAVE_LENGTH];

//=========================================================================

void start_adc(uint8_t ch)
{
        ADMUX = ch;
	ADCSRA |= (1 << ADSC);
}

//=========================================================================

uint16_t get_adc()
{
	while((ADCSRA & (1 << ADSC)) != 0);
	return ADC;
}


//=========================================================================

void get_wave()
{
uint16_t i;
	for(i=0; i < WAVE_LENGTH; ) {
		start_adc(0);
		wave[i++] = get_adc();
		start_adc(1);
		wave[i++] = get_adc();
	}
}


void get_sync_wave(uint8_t ch)
{
uint16_t i,w1,w2;
	start_adc(0);
	w1 = get_adc();
	for(;;) {
		start_adc(0);
		w2 = get_adc();
		if(w1 < 512-10 && w2 > 512-10) break;
		w1 = w2;
	}
	for(i=0; i < WAVE_LENGTH; ) {
		start_adc(ch);
		wave[i++] = get_adc();
	}
}

//=========================================================================

void send_wave()
{
uint16_t i;
uint16_t cs;
	cs = 0;
	for(i=0; i < WAVE_LENGTH; i += 2) {
		thexw(wave[i]);
		thexw(wave[i+1]);
		cs ^= wave[i];
		cs ^= wave[i+1];
	}
	thexw(cs ^ 0x1234);
	USART_Transmit('\n');
}

//=========================================================================

ISR (TIMER0_OVF_vect)
{
	TCNT0 = 256-64+24;
}

//=========================================================================

int main(void)
{
uint16_t cnt=0;
uint8_t c,div;
	DDRA = 0;
	PORTA = 0xff;
	DDRB = (1 << 4);	//TXD
	DDRC = 0xff;
	PORTC = 0;
	DDRD = 0xfe;

	USART_Init(8);		// 115.2K
	ADCSRA = 0x87;		// init ADC

//	TCCR0 = 1;
//	TIMSK = 1;
//	sei();

	for( ; ; ) {
		_WDR();

		if(USART_Ready()) {
			c = USART_Receive(STD_TIMEOUT);
			switch(c) {
				case 'g':
					div = rhex();
					if(USART_Timeout()) break;
					ADCSRA = 0x80 | div;
					div = rhex();
					if(USART_Timeout()) break;
					PORTC = div;	// gain
					get_wave();
					send_wave();
					break;

				case 'q':
					div = rhex();
					if(USART_Timeout()) break;
					ADCSRA = 0x80 | div;
					div = rhex();
					if(USART_Timeout()) break;
					PORTC = div;	// gain
					div = rhex();	// ch
					if(USART_Timeout()) break;
					get_sync_wave(div);
					send_wave();
					break;

				default:;
			}
		}

		if(++cnt > 1000) {
			PORTB = PINB ^ (1<<4); // blink
			cnt = 0;
		}
	}

	return 0;
}

//=========================================================================

