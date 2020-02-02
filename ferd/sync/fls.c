/*=========================================================================*/
/*
 * Extractor sync for FeR
 *
 * LZs,2020
 */
/*=========================================================================*/

#include "fls.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>
#include <math.h>

void delay_us(uint16_t n)
{
uint16_t i;
    for(i=0; i < n; i++) {
	_delay_us(1);
    }
}

/*=========================================================================*/

uint8_t get_aval(uint8_t ch)
{
uint16_t aval;
    ADMUX = ch;
    _delay_us(100);
    ADCSRA |= (1 << ADSC);
    while((ADCSRA & (1 << ADIF)) == 0);
    aval = ADC;
    aval /= 100;
    if(!aval) aval=1;
    return aval;
}

/*=========================================================================*/

void waitp()
{
uint16_t w;
    for(w=0; w < 0xffff; w++) {
	_WDR();
    	if((PINB & (1 << INT0)) != 0) break;
    }
}

void waitn()
{
uint16_t w;
    for(w=0; w < 0xffff; w++) {
	_WDR();
    	if((PINB & (1 << INT0)) == 0) break;
    }
}


int main(void)
{
uint16_t d,p,cnt;
uint8_t m,led;
	DDRA = 0xc0;	// porta 0-5 in,6-7 out
	PORTA = 0;
	DIDR0 = 3;	// PA0,1 as analog input
	ADCSRA = 0x82;	// ADC enable, clk/8

	DDRB = 0x20;	// LED out
	led = 0;

	_WDR();
        cnt = 0;
        d = p = 10;
	d = eeprom_read_word((uint16_t *)0);
	if(d == 0 || d > 3000) d = 10;
        p = eeprom_read_word((uint16_t *)2);
	if(p == 0 || p > 3000) p = 10;

	for(m=0; ; ) {
	    if(m == 0) {
    		if(KEY1 == 0) { if(d < 3000) d+=10; m = 1; }
		if(KEY2 == 0) { if(d > 10) d-=10; m = 1; }
		if(KEY3 == 0) { if(p < 3000) p+=10; m = 1; }
		if(KEY4 == 0) { if(p > 10) p-=10; m = 1; }
	    }

            if(led) { cbi(PORTB,LED); led=0; } else { sbi(PORTB,LED); led = 1; }

	    waitp();
	    delay_us(d);
	    CH21;
	    delay_us(p);
	    CH20;

	    waitn();
	    delay_us(d);
	    CH21;
	    delay_us(p);
	    CH20;

	    if(m) {
		if(KEY1 != 0 && KEY2 != 0 && KEY3 != 0 && KEY4 != 0) {
		    m = 0;
    		    cnt = 1000;
		}
	    }
            if(cnt > 0) {
                if(--cnt == 0) {
		    eeprom_write_word((uint16_t *)0, d);
		    eeprom_write_word((uint16_t *)2, p);
                }
            }
	}

	return 0;
}



