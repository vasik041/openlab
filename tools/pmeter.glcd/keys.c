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
        ADMUX = ch & 0x1f;
//	ADCSRB = (ch & 0x20) ? 0x10 : 0;
	_delay_ms(2);
	ADCSRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADSC)) != 0);
	aval = ADC;
	return aval;
}


uint8_t get_key()
{
int16_t a = get_aval(2);
	if(NEAROF(a,KEY_1_AVAL)) return KEY_1;
	if(NEAROF(a,KEY_2_AVAL)) return KEY_2;
	if(NEAROF(a,KEY_3_AVAL)) return KEY_3;
	if(NEAROF(a,KEY_4_AVAL)) return KEY_4;
	return 0;
}


