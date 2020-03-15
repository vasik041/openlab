/****************************************************************************/
/*
 * sin/pls gen
 * LZs,2011
 */
/****************************************************************************/

#include "main.h"
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>


extern uint16_t freq;
extern uint16_t freq_step;
extern uint16_t dly;
extern uint16_t tau;
extern uint8_t key;

//=========================================================================

void write_word(uint16_t p,uint16_t d)
{
uint16_t x;
	x = eeprom_read_word((uint16_t*)p);
	if(x != d) {
		eeprom_write_word((uint16_t*)p,d);
	}
}

//=========================================================================

void save_params()
{
uint16_t p;
	p = 0;
	write_word(p,freq);
	p += 2;
	write_word(p,freq_step);
	p += 2;
	write_word(p,dly);
	p += 2;
	write_word(p,tau);
	p += 2;
	write_word(p,key);
}

//=========================================================================
//=========================================================================

uint16_t read_word(uint16_t p,uint16_t min_val,uint16_t max_val)
{
uint16_t x;
	x = eeprom_read_word((uint16_t*)p);
	if(x > max_val || x < min_val) {
		return min_val;
	}
	return x;
}

//=========================================================================

void load_params()
{
uint16_t p;
	p = 0;
	freq = read_word(p,500,5000);
	p += 2;
	freq_step = read_word(p,1,100);
	if(freq_step != 1 && freq_step != 10 && freq_step != 100) freq_step = 100;
	p += 2;
	dly = read_word(p,1,100);
	p += 2;
	tau = read_word(p,5,250);
	p += 2;
	key = read_word(p,0,3);
}
