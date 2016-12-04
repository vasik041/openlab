/*=========================================================================*/
/*
 * Charge controller for solar panel
 * LZs,2016
 */
/*=========================================================================*/

#include "main.h"
#include "serial.h"


void delay(uint16_t n)
{
int i;
    for(i=0; i < n; i++) {
	_WDR();
	_delay_ms(1);
    }
}

/*=========================================================================*/

uint16_t get_aval(uint8_t ch)
{
    ADMUX = 0x40+ch;
    _delay_ms(1);
    ADCSRA |= (1 << ADSC);
    while((ADCSRA & (1 << ADIF)) == 0);
    return ADC;
}

/*=========================================================================*/

#define	ST_EVAL		0
#define	ST_CHARGING	1
#define	ST_IDLE		2
#define	ST_FULL		3

void puts_state(uint8_t s)
{
    switch(s) {
	case ST_EVAL:		puts_s(PSTR(" eval")); 		break;
	case ST_CHARGING:	puts_s(PSTR(" charging")); 	break;
	case ST_IDLE:		puts_s(PSTR(" idle")); 		break;
	case ST_FULL:		puts_s(PSTR(" full")); 		break;
	default:;
    }
}


int main(void)
{
uint16_t up,ub;
uint8_t f,state;
	DDRB = 0x3f;
	PORTB = 0xff;

	DDRC = 0x3f-0x03;
	PORTC = 0xff-0x03;	// adc0,1
	DIDR0 = 0x03;

	DDRD = 0xff;
	PORTD = 0xff;

	PRR = 0xe7;

	WDTCSR |= (1<<WDCE) | (1<<WDE);
	WDTCSR = (1<<WDE) | (1<<WDP3) | (1<<WDP2) | (1<<WDP0); //2s

	puts_s(PSTR("Hello world!"));
	LED0;
	CHARGE0;
	state = ST_EVAL;

	for(f=0; ; ) {
	    _WDR();
	    PRR &= ~1;
	    ADCSRA = 0x87;	// init ADC
	    ub = get_aval(0);
	    up = get_aval(1);
	    ADCSRA = 0;		// turn off ADC
	    PRR |= 1;

//	    puts_s(PSTR("Up ")); putxw_s(up); puts_s(PSTR(" Ub ")); putxw_s(ub); puts_state(state); nl();

	    switch(state) {
		case ST_EVAL:
		    if(ub > CH_OFF) state=ST_FULL; else state=ST_IDLE;
		    break;

		case ST_CHARGING:
		    if(ub > CH_OFF) state=ST_FULL;
		    if(up <= ub) state=ST_IDLE;
		    break;
 
		case ST_IDLE:
		    if(up > ub) state=ST_CHARGING; 
		    break;

		case ST_FULL:
		    if(ub < CH_ON) state=ST_CHARGING;
		    break;

		default:;
	    }

	    if(state == ST_CHARGING) {
		CHARGE1;
	    } else {
		CHARGE0;
	    }

	    switch(state) {
		case ST_FULL:
		     LED1;
		     break;
		case ST_CHARGING: 
		    if(f) LED0; else LED1;
		    f ^= 1;
		    break;
		case ST_IDLE:
		    LED0;
		    break;
		default:;
	    }

	    CLKPR = 0x80;	//10sec
	    CLKPR = 8;		// 8Mhz / 256
	    delay(40);
	    CLKPR = 0x80;
	    CLKPR = 0;
	}

	return 0;
}

/*=========================================================================*/



  
