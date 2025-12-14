/*=========================================================================*/
/*
 * 2 channel pulse generator
 * LZs,2010-2023
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

#include "main.h"
#include "usart.h"
#include "keys.h"
#include "wire.h"
#include "rgb_lcd.h"

uint32_t period;
uint32_t pulse;
uint32_t pulse2;

// all in nanoseconds
#define MIN_PULSE 500          // 0.5us
#define MAX_PULSE (100000-500)
#define DEF_PULSE 1000         // 1us
#define DEF_PULSE2 2000        // 2us

#define MIN_PERIOD 16000       // 60KHz (for /2 setup)
#define MAX_PERIOD 100000      // 10KHz
#define DEF_PERIOD 50000       // 20KHz

#define STEP 100               // 0.1us

/*=========================================================================*/

void delay_ms(uint16_t n)
{
uint16_t i;
    for(i=0; i < n; i++) {
	_delay_ms(1);
    }
}

/*=========================================================================*/

uint16_t get_aval(uint8_t ch)
{
uint16_t aval;
        ADMUX = ch | 0x40;       	//vcc with ext cap on AREF
	delay_ms(5);
	ADCSRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADSC)) != 0);
	aval = ADC;
	return aval;
}

uint8_t get_key()
{
int16_t a = get_aval(0);
    if(NEAROF(a, KEY_1_AVAL)) return KEY_1;
    if(NEAROF(a, KEY_2_AVAL)) return KEY_2;
    if(NEAROF(a, KEY_3_AVAL)) return KEY_3;
    if(NEAROF(a, KEY_4_AVAL)) return KEY_4;
    return 0;
}

//=========================================================================
// 1,8,64,256,1024
// 1 2 3  4   5

void setup_timer1()
{
uint8_t div = 1;
uint16_t t16 = 1;
uint16_t p16 = 1;
uint16_t p17 = 1;
uint64_t t1 = SCALE_1(period);
uint64_t p1 = SCALE_1(pulse);
uint64_t p2 = SCALE_1(pulse2);

	if(t1 < 0x10000l) {
		div = 1;
		t16 = (uint16_t)t1;
		p16 = (uint16_t)p1;
                p17 = (uint16_t)p2;
	} else if(t1 >= 0x10000l && t1 < 0x80000l) {
		div = 2;
		t16 = (uint16_t)(t1 >> 3);
		p16 = (uint16_t)(p1 >> 3);
                p17 = (uint16_t)(p2 >> 3);
	} else if(t1 >= 0x80000l && t1 < 0x400000l) {
		div = 3;
		t16 = (uint16_t)(t1 >> 6);
		p16 = (uint16_t)(p1 >> 6);
		p17 = (uint16_t)(p2 >> 6);
	}

	TCCR1A = 0xf2; //a +0x10
	TCCR1B = 0x18+div;
	ICR1 = t16 ? (t16-1) : t16;
	OCR1A = p16 ? p16 : 1;
	OCR1B = p17 ? p17 : 1;
}

//=========================================================================

void save_params()
{
    eeprom_write_dword((uint32_t*)0, period);
    eeprom_write_dword((uint32_t*)4, pulse) ;
    eeprom_write_dword((uint32_t*)8, pulse2);
}

void load_params()
{
    period = eeprom_read_dword((uint32_t*)0);
    pulse = eeprom_read_dword((uint32_t*)4);
    pulse2 = eeprom_read_dword((uint32_t*)8);

    if(period >= MAX_PERIOD) period = DEF_PERIOD;
    if(period < MIN_PERIOD) period = DEF_PERIOD; 

    if(pulse >= MAX_PULSE) pulse = DEF_PULSE;
    if(pulse < MIN_PULSE) pulse = DEF_PULSE; 

    if(pulse2 >= MAX_PULSE) pulse2 = DEF_PULSE2;
    if(pulse2 < MIN_PULSE) pulse2 = DEF_PULSE2; 
}

//=========================================================================

void puts1(const prog_char *s)
{
char c;
    for( ; ; s++) {
	c = pgm_read_byte(s);
	if(c == '\0') break;
	rgb_lcd_write(c);
    }
}

void putd(uint32_t n, uint8_t digs, uint8_t f)
{
uint8_t i,p;
uint32_t m;

    switch(digs) {
        case 1: m = 1;      break;
	case 2: m = 10;     break;
	case 3: m = 100;    break;
	case 4: m = 1000;   break;
	case 5: m = 10000;  break;
	case 6: m = 100000; break;
	case 7: 
	default: m = 1000000; 
    }

    p = f;
    for(i=0; i < digs; i++) {
	if(m == 1) p = 1;
	if(n / m != 0 || p) {
	    rgb_lcd_write(n / m + '0');
	    p = 1;
	} else {
   	    rgb_lcd_write(' ');
	}
	n %= m;
	m /= 10;
    }
}

void view()
{
uint32_t p = pulse/100;
uint32_t p2 = pulse2/100;
uint32_t t = period/100;

    rgb_lcd_clear();
    putd(p/10, 3, 0);      rgb_lcd_write('.');   putd(p%10, 1, 1);
    rgb_lcd_write('u');
    rgb_lcd_write('s');

    rgb_lcd_setCursor(8,0);
    putd(t/10, 3, 0);    rgb_lcd_write('.');    putd(t%10, 1, 1);
    rgb_lcd_write('u');
    rgb_lcd_write('s');

    rgb_lcd_setCursor(0,1);
    putd(p2/10, 3, 0);    rgb_lcd_write('.');    putd(p2%10, 1, 1);
    rgb_lcd_write('u');
    rgb_lcd_write('s');
}

//=========================================================================

int main(void)
{
uint16_t cnt1 = 0;
uint8_t cnt2 = 0;
uint8_t ok = 0;
uint8_t sw = 0;
uint16_t f = 0;
	DDRC = 0;	// portc in
	PORTC = 0;

	DDRB = 0x0f;	// LED, OC1A, OC1B, OC2A
	DDRD = 0xe8;

	DIDR0 = 0x0f;
	ADCSRA = 0x87;	// ADC enable, /128

	TCCR2A = 0xf1;
	TCCR2B = 1;

	period = DEF_PERIOD;
	pulse = DEF_PULSE;
	pulse2 = DEF_PULSE2;
        load_params();
	setup_timer1();

	USART_Init(BR_9600);
        _SEI();

        rgb_lcd_begin(16, 2, LCD_5x8DOTS);
//        puts1(PSTR("Hello"));

        view();

	for( ; ; ) {
                uint8_t k = get_key(); // 5ms
                if(k != ok) {
                    switch(k) {
                        case KEY_1: // pulse down
                            if(sw) {
                                if(pulse2 > MIN_PULSE+STEP) {
                                    pulse2 -= STEP;
                                    setup_timer1();
                                    f = 1000;
                                }
                            } else {
                                if(pulse > MIN_PULSE+STEP) {
                                    pulse -= STEP;
                                    setup_timer1();
                                    f = 1000;
                                }
                            }
                            break;

                        case KEY_2: // pulse up
                            if(sw) {
                                if(pulse2 < MAX_PULSE-STEP && pulse2 < period-STEP) {
                                   pulse2 += STEP;
                                   setup_timer1();
                                   f = 1000;
                                }
                            } else {
                                if(pulse < MAX_PULSE-STEP && pulse < period-STEP) {
                                    pulse += STEP;
                                    setup_timer1();
                                    f = 1000;
                                }
                            }
                            break;

                        case KEY_3: // period down
                            if(period > MIN_PERIOD+STEP) {
                                period -= STEP;
                                if(pulse >= period-MIN_PULSE) pulse = period - MIN_PULSE;
                                if(pulse2 >= period-MIN_PULSE) pulse2 = period - MIN_PULSE;
                                setup_timer1();
                                f = 1000;
                            }
                            break;

                        case KEY_4: // period up
                            if(period < MAX_PERIOD-STEP) {
                                period += STEP;
                                setup_timer1();
                                f = 1000;
                            }
                            break;
                        default:;
                    }
                }
                if(f == 1000) view();
		if(++cnt1 > (sw ? 300 : 50)) {
		    cnt1 = 0;
                    cnt2++;
                    if(k != 0 && cnt2 > (sw ? 3 : 6)) { sw ^= 1; cnt2 = 0; }
		    PORTB = PINB ^ 1;
		}
                ok = k;
                if(k == 0) cnt2 = 0;
                if(f) {
                    f--;
                    if(f == 0) save_params();
                }
	}

	return 0;
}

/*=========================================================================*/


