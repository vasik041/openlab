/****************************************************************************/
/*
 * Free Energy EXperimentator
 * This is free and unencumbered software released into the public domain.
 * LZs,2010-2016
 */
/****************************************************************************/

/* CPU frequency */
#define F_CPU 16000000UL

#define K1M	625UL	// 62.5ns = 1/16MHz
#define K1D	10UL

#define	SCALE(X)	(K1M*X/K1D)
#define	SCALE_1(X)	(K1D*X/K1M)


#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>


#define	NELEM(X)	(sizeof(X)/sizeof(X[0]))

/*=========================================================================*/
