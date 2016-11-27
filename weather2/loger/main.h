/****************************************************************************/
/*
 * RF sensors loger
 * LZs,2015
 */
/****************************************************************************/

// CPU frequency
#define F_CPU 8000000UL

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>
#include <string.h>

#define	LED	4 //PORTB

#define clrbit(X,Y)     X &= ~(1 << Y)
#define setbit(X,Y)     X |=  (1 << Y)

#define	NELEM(X)	(sizeof(X)/sizeof(X[0]))

void delay(uint16_t n);


/*=========================================================================*/
