/****************************************************************************/
/*
 * Uimeter on uiptfg board
 * This is free and unencumbered software released into the public domain.
 * LZs,2008,2015
 */
/****************************************************************************/

//#define UI12	1	// 12v pwr suply

#ifdef UI12
	#define	USCALE	20
#else
	#define	USCALE	40
#endif


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

/*=========================================================================*/
