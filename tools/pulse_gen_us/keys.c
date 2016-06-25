//=========================================================================
//
// keys with ADC
// This is free and unencumbered software released into the public domain.
// LZ,2015
//
//=========================================================================

#include "plsgen.h"
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>

#include "keys.h"
#include "display.h"

//=========================================================================

uint16_t get_aval(uint8_t ch)
{
uint16_t aval;
        ADMUX = ch;
	delay(1);
	ADCSR |= (1 << ADSC);
	while((ADCSR & (1 << ADSC)) != 0);
	aval = ADC;
	return aval;
}

//=========================================================================

uint8_t get_key()
{
int16_t a = get_aval(2);
	if(NEAROF(a,KEY_1_AVAL)) return KEY_1;
	if(NEAROF(a,KEY_2_AVAL)) return KEY_2;
	if(NEAROF(a,KEY_3_AVAL)) return KEY_3;
	if(NEAROF(a,KEY_4_AVAL)) return KEY_4;
	return 0;
}


