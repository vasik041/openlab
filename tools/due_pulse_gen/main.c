//==============================================================================
/*
 * Pls gen 2ch
 * LZs, 2022
 */
//==============================================================================

#include "include/due_sam3x_init.h"
#include "display.h"
#include "adc.h"
#include "keys.h"
#include "pwm.h"

#define FREQ_MIN 10
#define FREQ_MAX 1000

volatile uint32_t pulse0;
volatile uint32_t pulse1;
volatile uint32_t period;
volatile uint32_t phase;
volatile uint8_t mode;


void ledx()
{
    if(PIOB->PIO_ODSR & PIO_PB27) {
        PIOB->PIO_CODR = PIO_PB27; /* Set clear register */
    } else {
        PIOB->PIO_SODR = PIO_PB27; /* Set set register */
    }
}

void put31(uint32_t d)
{
    putd(d/10, 3, 0);
    putch('.');
    putch('0'+d%10);
}

void show()
{
    clrscr(1);
    locxy(0,0); puts1("t1:"); put31(pulse0);
    locxy(8,0); puts1("fq:"); put31(period);
    locxy(0,1); puts1("t2:"); put31(pulse1);
    locxy(8,1); puts1("ph:"); put31(phase);

    switch(mode) {
     case 0: locxy(3,0); break;
     case 1: locxy(11,0); break;
     case 2: locxy(3,1); break;
     case 3: locxy(11,1); break;
     default:;
    }
}


uint8_t handle_period(uint8_t k)
{
uint8_t f = 0;
    switch(k) {
        case KEY_UP:   if(period < FREQ_MAX-10) { period += 10; f=1; } break;
        case KEY_DOWN: if(period >= FREQ_MIN+10) { period -= 10; f=1; } break;
        default:;
    }
    return f;
}

uint8_t handle_pulse0(uint8_t k)
{
uint8_t f = 0;
    switch(k) {
        case KEY_UP:   if(pulse0 < FREQ_MAX-10+10) { pulse0 += 10; f=1; } break;
        case KEY_DOWN: if(pulse0 >= FREQ_MIN+10)  { pulse0 -= 10; f=1; } break;
        default:;
    }
    return f;
}

uint8_t handle_pulse1(uint8_t k)
{
uint8_t f = 0;
    switch(k) {
        case KEY_UP:   if(pulse0 < FREQ_MAX-10+10) { pulse1 += 10; f=1; } break;
        case KEY_DOWN: if(pulse0 >= FREQ_MIN+10)  { pulse1 -= 10; f=1; } break;
        default:;
    }
    return f;
}

uint8_t handle_phase(uint8_t k)
{
uint8_t f = 0;
    switch(k) {
        case KEY_UP:   if(phase < FREQ_MAX-10+10) { phase += 10; f=1; } break;
        case KEY_DOWN: if(phase >= FREQ_MIN+10)  { phase -= 10; f=1; } break;
        default:;
    }
    return f;
}

int main(void)
{
uint8_t k0 = 0;
uint8_t f = 0;
    init_controller(); 						/* The general init (clock, libc, watchdog ...) */
    PIO_Configure(PIOB, PIO_OUTPUT_1, PIO_PB27, PIO_DEFAULT);	/* Board pin 13 == PB27 LED */

//    uart_init(115200);
//    uart_puts("Hello world!\n");

    display_init();
    puts1("Hello world!");
    adc_init1();
    adc_enable_channel(ADC,7);

    pwm_init();
    pulse0 = pulse1 = 10;
    period = 500;
    phase = 10;
    mode = 0;
    pwm_set(0, period, pulse0);
    pwm_set(1, period, pulse1);
    pwm_set(2, period, phase);

    show();
    for(;;) {
        uint8_t k = get_key();
        if(k != k0) {
            if(k == KEY_SELECT || k == KEY_LEFT || k == KEY_RIGHT) {
                if(k == KEY_LEFT) {
                    if(mode == 0) mode = 3; else mode--;
                } else {
                    if(mode == 3) mode = 0; else mode++;
                }
            } else {
                switch(mode) {
                    case 0: f = handle_pulse0(k); break;
                    case 1: f = handle_period(k); break;
                    case 2: f = handle_pulse1(k); break;
                    case 3: f = handle_phase(k); break;
                    default:;
                }
            }
            if(f) {
                pwm_set(0, period, pulse0);
                pwm_set(1, period, pulse1);
                pwm_set(2, period, phase);
            }
            show();
            k0 = k;
	    ledx();
        }
	Sleep(10);
    }

    return 0;
}

