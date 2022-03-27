//=========================================================================
//
// keys with ADC
// This is free and unencumbered software released into the public domain.
// LZ,2015-2020
//
//=========================================================================

#include "adc.h"
#include "keys.h"

//=========================================================================

uint8_t get_key()
{
int16_t a = adc_get1(7);
	if(NEAROF(a, KEY_UP_AVAL)) return KEY_UP;
	if(NEAROF(a, KEY_DOWN_AVAL)) return KEY_DOWN;
	if(NEAROF(a, KEY_LEFT_AVAL)) return KEY_LEFT;
	if(NEAROF(a, KEY_RIGHT_AVAL)) return KEY_RIGHT;
	if(NEAROF(a, KEY_SELECT_AVAL)) return KEY_SELECT;
	return 0;
}

