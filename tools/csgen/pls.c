/*=========================================================================*/
/*
 * Cap S gen
 * LZs,2016
 */
/*=========================================================================*/

#include "pls.h"


/*=========================================================================*/

uint16_t seed;

void rand_delay()
{
uint16_t w;
        seed = (seed * 31415 + 12345) % 1000;
	for(w=0; w < seed; w++) {
		_delay_ms(10);
	}
}


int main(void)
{
	DDRA = (1 << RELAY);
	PORTA = 0;

	DDRB  = (1 << PWM) | (1 << LED);
	PORTB = (1 << PWM) | (1 << LED);

	ACSR = 0x44;	//ACME,ACBG 1.18v
	ADCSR = 0;
        ADMUX = 0;	//ADC0

	for(;;) {
		while((ACSR & (1<<ACO)) != 0) {
			cbi(PORTB,PWM);
			_delay_us(3);
			sbi(PORTB,PWM);
			_delay_us(200);
		}

		LED1;
		sbi(PORTA,RELAY);
		_delay_ms(100);
		cbi(PORTA,RELAY);
		_delay_ms(100);
		LED0;

		rand_delay();
	}
	return 0;
}

/*=========================================================================*/


