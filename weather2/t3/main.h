/****************************************************************************/
/*
 * 3x temp sensor
 * LZs,2016
 */
/****************************************************************************/

/* CPU frequency */
#define F_CPU 8000000UL

#define	NELEM(X)	(sizeof(X)/sizeof(X[0]))

#include <inttypes.h>
#include <avr/io.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
//#include <avr/eeprom.h>

/*=========================================================================*/
