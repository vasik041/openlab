/****************************************************************************/
/*
 * Clock ver.4
 * LZs,2016
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

//     a
//  f     b
//     g
//  e     c
//     d    dp


// PORTA
#define	segA	4
#define	segB	1
#define	segC	0x10
#define	segD	0x40
#define	segE	0x80
#define	segF	2
#define	segG	8
#define	segDP	0x20

// PORTB bits
#define	ka0	2
#define	ka1	1
#define	ka2	0
#define	ka3	3

#define	chrH	(segF+segB+segE+segC+segG)
#define	chrP	(segF+segE+segA+segB+segG)
#define chrDASH segG
#define chrSP	0

#define clrbit(X,Y)     X &= ~(1 << Y)
#define setbit(X,Y)     X |=  (1 << Y)

#define	NELEM(X)	(sizeof(X)/sizeof(X[0]))

void delay(uint16_t n);


/*=========================================================================*/
