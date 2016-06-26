
#ifndef TEST_H
#define TEST_H

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>

#include <inttypes.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>


#define	LED	0x10

#define	NELEM(X)	(sizeof(X)/sizeof(X[0]))

#endif // TEST_H
