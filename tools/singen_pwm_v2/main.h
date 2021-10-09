/****************************************************************************/
/*
 * PWM Sin gen with volume control
 * LZs,2010-2018
 */
/****************************************************************************/

/* CPU frequency */
#define F_CPU 16000000UL

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>
#include <math.h>

#define	LED	4	//pb4

#define	NELEM(X)	(sizeof(X)/sizeof(X[0]))

/*=========================================================================*/
