/*=========================================================================*/
/**
 * Pulse generator using atm328p/Timer1 (Arduino UNO + ARD LCD216)
 *
 * FE R&D group,2012-2014
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

/* CPU frequency */
#define F_CPU	16000000UL

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <util/delay.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <math.h>


#define	TAU_MAX	100000000	//10hz
#define	TAU_MIN	5000		//200khz
#define	DT_MAX	950		//50%

#define NELEM(X)	(sizeof(X)/sizeof(X[0]))


#define	LED0	cbi(PORTB,5)
#define	LED1	sbi(PORTB,5)


// rot. enc. on INT0
#define	DRC	((PINC & 0x04) == 0)

// rot. enc. on INT1
#define	DRC2	((PINC & 0x02) == 0)

/*=========================================================================*/
