/****************************************************************************/
/*
 * Clock ver.3
 * LZs,2015
 * This is free and unencumbered software released into the public domain.
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


// PORTA
#define	segA	1
#define	segB	4
#define	segC	0x10
#define	segD	0x40
#define	segE	0x80
#define	segF	8
#define	segG	2
#define	segDP	0x20

// PORTB bits
#define	ka0	2
#define	ka1	1
#define	ka2	0
#define	ka3	3

#define clrbit(X,Y)     X &= ~(1 << Y)
#define setbit(X,Y)     X |=  (1 << Y)

#define	NELEM(X)	(sizeof(X)/sizeof(X[0]))

void delay(uint16_t n);


/*=========================================================================*/
