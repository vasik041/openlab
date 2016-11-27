/****************************************************************************/
/*
 * Temp/Humidity RF sensor
 * LZs,2015
 */
/****************************************************************************/

// CPU frequency
#define F_CPU 1000000UL

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>

#define	LED	4 //PORTB

#define	LED1	clrbit(PORTB,LED)
#define	LED0	setbit(PORTB,LED)

#define clrbit(X,Y)     X &= ~(1 << Y)
#define setbit(X,Y)     X |=  (1 << Y)

#define	NELEM(X)	(sizeof(X)/sizeof(X[0]))

void delay(uint16_t n);


/*=========================================================================*/
