
#include "main.h"
#include "usart.h"

volatile uint8_t sig[128];
volatile uint8_t delta;

/*=========================================================================*/

int main(void)
{
uint8_t pos;
	DDRC = 0;	// portc in
	PORTC = 0;
	DDRB = 0x07;	// LED, OC1A, OC1B
	DDRD = 0xe0;

        delta = 100;

	USART_Init(BR_9600);
	for(pos=0; pos < 128; pos++) {
		if(pos < 10) sig[pos] = 1; else sig[pos] = 7;
	}
	sei();

	for( ; ; ) {
		for(pos=0; pos < delta; pos++)
			PORTB = sig[pos];
	}

	return 0;
}

/*=========================================================================*/


