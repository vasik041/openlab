//=========================================================================
//
// keys with ADC
// LZ,2015
// This is free and unencumbered software released into the public domain.
//
//=========================================================================

#include "main.h"
#include "keys.h"

//=========================================================================

uint16_t get_aval(uint8_t ch)
{
uint16_t aval;
        ADMUX = 0x40 | ch;
	ADCSRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADSC)) != 0);
	aval = ADC;
	return aval;
}

//=========================================================================

uint8_t get_key()
{
int16_t a = get_aval(0);
	if(NEAROF(a,KEY_UP_AVAL)) return KEY_UP;
	if(NEAROF(a,KEY_DOWN_AVAL)) return KEY_DOWN;
	if(NEAROF(a,KEY_LEFT_AVAL)) return KEY_LEFT;
	if(NEAROF(a,KEY_RIGHT_AVAL)) return KEY_RIGHT;
	if(NEAROF(a,KEY_ENTER_AVAL)) return KEY_ENTER;
	return 0;
}


