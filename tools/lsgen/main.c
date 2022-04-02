/****************************************************************************/
/*
 * L S gen
 * LZs,2016
 */
/****************************************************************************/

#include "main.h"

volatile uint8_t f;
volatile uint8_t fq;
volatile uint8_t div;

ISR (TIMER1_CMPB_vect)
{
    f++;
}

void next()
{
    TCCR1B = div;
    OCR1C = fq;
    f = 0;
    if(fq > 50) {
	fq--;
	return;
    }
    fq = 100;
    if(div > 10) {
	div--;
	return;
    }
    div = 15;
}

int main(void)
{
uint16_t cnt;
uint8_t sw;
    DDRA = 0x80;
    PORTA = 0x80;
    DDRB = 8;
    PORTB = 0;
    TCCR1A = 0x31;
    TCCR1B = 15;
    OCR1C = 100;
    OCR1B = 1;
    TIMSK = 0x20;
    f = 0;
    fq = 100;
    div = 15;
    sei();
    for(cnt=0,sw=0; ; ) {
	_delay_ms(1);
	if(++cnt > 300) {
	    if(sw) LED1; else LED0;
	    sw ^= 1;
	    cnt = 0;
	}
	if(f > 10) next();
    }
    return 0;
}
