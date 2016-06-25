/****************************************************************************/
/*
 * Osc
 * LZs,2009,2015
 * This is free and unencumbered software released into the public domain.
 */
/****************************************************************************/

#ifndef MAIN_H
#define MAIN_H

#define F_CPU 20000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include <inttypes.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>


#define	LED	1

#define	GAIN2	2
#define	GAIN1	3
#define	GAIN0	4


#define	NELEM(X)	(sizeof(X)/sizeof(X[0]))

#endif // MAIN_H
