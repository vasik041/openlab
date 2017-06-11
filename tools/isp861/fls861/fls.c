/*=========================================================================*/
/*
 * test: toggle PA0
 * This is free and unencumbered software released into the public domain.
 * LZs,2010
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

/*=========================================================================*/
/*
 * delay for 1ms
 */

void wt1ms(void)
{
	_WDR();
	_delay_us(1000);
}

/*=========================================================================*/
/*
 * delay for n ms
 */

void delay(uint16_t n)
{
	while(n--)
		wt1ms();
}

/*=========================================================================*/

int main(void)
{
	DDRA = 1;	// porta 0 out, 1-7 in
	PORTA = 1;

	for( ; ; ) {
		delay(1000);
		cbi(PORTA,0);

		delay(1000);
		sbi(PORTA,0);
	}

	return 0;
}

/*=========================================================================*/


