/****************************************************************************/
/*
 * Charge controller for solar panel
 * LZs,2016
 */
/****************************************************************************/

/* CPU frequency */
#define F_CPU 8000000UL

#define	NELEM(X)	(sizeof(X)/sizeof(X[0]))

#define	setbit(X,Y)	X |= (1 << Y)
#define	clrbit(X,Y)	X &= ~(1 << Y)


#include <inttypes.h>
#include <avr/io.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
//#include <avr/eeprom.h>

#define	LED1	clrbit(PORTB,0)
#define	LED0	setbit(PORTB,0)

#define	CHARGE1	setbit(PORTD,0)
#define	CHARGE0	clrbit(PORTD,0)

#define	CH_ON	180	//13.5v
#define CH_OFF	205	//14v

/*=========================================================================*/
