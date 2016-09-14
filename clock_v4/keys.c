//=========================================================================
//
// keys with ADC
// LZ,2015
//
//=========================================================================

#include "main.h"
#include "keys.h"

//=========================================================================

uint16_t get_aval(uint8_t ch)
{
uint16_t aval;
        ADMUX = ch;
	delay(1);
	ADCSRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADSC)) != 0);
	aval = ADC;
	return aval;
}

//=========================================================================

uint8_t get_key()
{
int16_t a = get_aval(9);
	if(NEAROF(a,KEY_1_AVAL)) return KEY_1;
	else if(NEAROF(a,KEY_2_AVAL)) return KEY_2;
	else if(NEAROF(a,KEY_3_AVAL)) return KEY_3;
	else if(NEAROF(a,KEY_4_AVAL)) return KEY_4;
	return 0;
}


