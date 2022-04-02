/****************************************************************************/
/*
 * L S gen
 * LZs,2016
 */
/****************************************************************************/

/* CPU frequency */
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

#define	LED	7	//pa7
#define	LED0	sbi(PORTA,LED)
#define	LED1	cbi(PORTA,LED)

#define	PWM	3	//pb3

/*=========================================================================*/
