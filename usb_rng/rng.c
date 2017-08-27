
#include <avr/io.h>
#include "rng.h"

uint8_t bit0;
uint8_t bit1;

uint8_t rnd;
uint8_t cnt;

void initRnd()
{
    bit0 = 0;
    bit1 = 0;
    rnd = 0;
    cnt = 0;
}


uint16_t getRnd()
{
uint8_t t;

    bit0 = getBit();
    bit1 = getBit();

    if(bit0 == bit1) {
	return 0x100;
    }

    rnd |= (bit1 << cnt);
    cnt++;

    if(cnt < 8) {
	return 0x100;
    }

    t = rnd;
    rnd = 0;
    cnt = 0;

    return t;
}
