/*=========================================================================*/
/*
 * 50...500hz sin PWM gen vith volume control
 * LZs,2010-2018
 */
/*=========================================================================*/

#include "main.h"
#include "keys.h"
#include "display.h"

volatile uint16_t freq;
volatile uint16_t phase;
volatile uint16_t step;
volatile uint16_t gain;
volatile uint8_t scale[256];


// 256
const prog_char sine[] = {
	128,131,134,137,140,143,146,149,152,155,158,161,164,167,170,172,
	175,178,181,184,186,189,192,194,197,199,202,204,207,209,211,214,
	216,218,220,222,224,226,228,230,231,233,235,236,238,239,240,242,
	243,244,245,246,247,248,249,249,250,251,251,252,252,252,252,252,
	253,252,252,252,252,252,251,251,250,249,249,248,247,246,245,244,
	243,242,240,239,238,236,235,233,231,230,228,226,224,222,220,218,
	216,214,211,209,207,204,202,199,197,194,192,189,186,184,181,178,
	175,172,170,167,164,161,158,155,152,149,146,143,140,137,134,131,
	128,124,121,118,115,112,109,106,103,100, 97, 94, 91, 88, 85, 83,
	 80, 77, 74, 71, 69, 66, 63, 61, 58, 56, 53, 51, 48, 46, 44, 41,
	 39, 37, 35, 33, 31, 29, 27, 25, 24, 22, 20, 19, 17, 16, 15, 13,
	 12, 11, 10,  9,  8,  7,  6,  6,  5,  4,  4,  3,  3,  3,  3,  3,
	  3,  3,  3,  3,  3,  3,  4,  4,  5,  6,  6,  7,  8,  9, 10, 11,
	 12, 13, 15, 16, 17, 19, 20, 22, 24, 25, 27, 29, 31, 33, 35, 37,
	 39, 41, 44, 46, 48, 51, 53, 56, 58, 61, 63, 66, 69, 71, 74, 77,
	 80, 83, 85, 88, 91, 94, 97,100,103,106,109,112,115,118,121,124
};

/*=========================================================================*/

ISR (TIMER1_OVF_vect)
{
	OCR1B = scale[pgm_read_byte(sine+(phase >> 8))];
	phase += step;
}


void set_gain()
{
uint16_t i,tmp;
	tmp = gain+1;
	for(i=0; i < 256; i++) {
	    scale[i] = i * tmp / 16;
	}
}

void putd3(uint16_t n,uint8_t f)
{
uint16_t m;
uint8_t i,p;
    if(n > 999) n = 999;
    m = 100;
    p = f;
    for(i=0; i < 3; i++) {
	if(m == 1) p = 1;
	if(n / m != 0 || p) {
	    putch(n / m + '0');
	    p = 1;
	}
	n %= m;
	m /= 10;
    }
}


void refresh()
{
    clrstr(0);
    locxy(0,0);
    puts1(PSTR("Freq:"));
    putd3(step,0);

    locxy(9,0);
    puts1(PSTR("Vol:"));
    putd3(gain,0);
}

/*=========================================================================*/

#define MAX_FREQ	500
#define	MIN_FREQ	60
#define	MAX_TMR		10	//cycles before increase step


int main(void)
{
uint8_t c,oc,cnt,stp,k1,k2;
	DDRA = 0xf8+3;
	PORTA = 0;
	DIDR0 = 0x04;		// PA2 (ADC2) as analog input
	ADCSRA = 0x87;		// ADC enable
	ADMUX = 2;
	DDRB = 0x38;

	PLLCSR = (1<<PLLE);	// Enable PLL
	while((PLLCSR & (1 << PLOCK)) == 0); // Wait for PLL to lock (approx. 100ms)
	PLLCSR |= (1 << PCKE);	// Set PLL as PWM clock source 

	TCCR1A = 0x21;  // OC1B, PWM1B
	TCCR1B = 2;	// clk/2
	TCCR1D = 1;
	OCR1C = 0xff;	// top
//	OCR1B = 10;	// value

        TIMSK = 4;	// TOIE1
	sei();

	delay(500);
	display_init();
	clrscr(1);

	phase = 0;
	freq = eeprom_read_word((uint16_t *)0);
	if(freq > MAX_FREQ-MIN_FREQ) freq = 0;
	step = MIN_FREQ + freq;

	gain = eeprom_read_byte((uint8_t *)2);
	gain &= 0x0f;
	set_gain();

	refresh();

	for(c=oc=cnt=stp=k1=k2=0; ; ) {
		c = get_key();
		if(c != oc) {
		    switch(c) {
			case KEY_1:
				stp = (++k1 > 5) ? 10 : 1;
				if(freq > stp) {
				    freq -= stp;
				    cnt = 0;
				}
				k2 = 0;
				step = MIN_FREQ + freq;
				break;
			case KEY_2:
				stp = (++k2 > 5) ? 10 : 1;
			    	if(freq < MAX_FREQ-MIN_FREQ-stp) {
				    freq += stp;
				    cnt = 0;
				}
				k1 = 0;
				step = MIN_FREQ + freq;
				break;
			case KEY_3:
				if(gain != 0) {
				    gain--;
			 	    set_gain();
				    cnt = 0;
				}
				break;
			case KEY_4:
				if(gain < 15) {
    				    gain++;
			 	    set_gain();
				    cnt = 0;
				}
				break;
			default:;
		    }
		    oc = c;
		    refresh();
		}
		_delay_ms(50);

		if(cnt < MAX_TMR) {
		    if(++cnt == MAX_TMR) {
			eeprom_write_word((uint16_t *)0,freq);
			eeprom_write_byte((uint8_t *)2,gain);
		    }
		}
	}

	return 0;
}

/*=========================================================================*/


