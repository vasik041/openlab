/*
 * PWM Sin gen 40..400hz + sync out
 * LZs, 2020
 */
#include "include/due_sam3x_init.h"
#include "uart.h"
#include "display.h"
#include "adc.h"
#include "keys.h"
#include "pwm.h"

#define FREQ_MIN 2560      //40hz
#define FREQ_MAX 25600     //400hz

uint8_t mode;
uint32_t freq;
uint16_t stp;
uint16_t s0;
uint16_t t0;


void ledx()
{
    if(PIOB->PIO_ODSR & PIO_PB27) {
        PIOB->PIO_CODR = PIO_PB27; /* Set clear register */
    } else {
        PIOB->PIO_SODR = PIO_PB27; /* Set set register */
    }
}
 
void show()
{
    clrscr(0);
    switch(mode) {
     case 0:
            locxy(0,0);       puts1("Freq: ");    putd(10*freq/640, 3, 0);
            locxy(0,1);       puts1("Step: ");    putd(stp/64, 2, 0);
            break;
     case 1:
            locxy(0,0);       puts1("start0: ");  putd(s0, 2, 0);
            locxy(0,1);       puts1("t0:     ");  putd(t0, 2, 0);
            break;
     default:;
    }
}


uint8_t handle_freq(uint8_t k)
{
uint8_t f = 0;
    switch(k) {
        case KEY_RIGHT:   if(freq < FREQ_MAX-stp) { freq += stp; f=1; } break;
        case KEY_LEFT:    if(freq >= FREQ_MIN+stp) { freq -= stp; f=1; } break;
        case KEY_UP:      
        case KEY_DOWN:    if(stp == 64) stp = 640; else stp = 64; break;
        default:;
    }
    return f;
}

uint8_t handle_pulse0(uint8_t k)
{
uint8_t f = 0;
    switch(k) {
        case KEY_RIGHT:   if(s0 < 99) { s0++; f=1; } break;
        case KEY_LEFT:    if(s0 > 1)  { s0--; f=1; } break;
        case KEY_UP:      if(t0 < 99) { t0++; f=1; } break;
        case KEY_DOWN:    if(t0 > 1) { t0--; f=1; } break;
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

    uart_init(115200);
    uart_puts("Hello world!\n");

    display_init();
//    puts1("Hello world!");

    adc_init1();
    adc_enable_channel(ADC,7);

    pwm_init();
//    pwm_set(20000, 100);

    freq = FREQ_MIN; stp = 64;  set_freq(freq/10);
    s0 = 25; t0 = 5;           set_pulse0(s0, t0);
    mode = 0;
    show();

    for(;;) {
	if(uart_available()) {
	    uint8_t c = uart_read();
	    ledx();
	    uart_write(c);
	}

        uint8_t k = get_key();
        if(k != k0) {
            if(k == KEY_SELECT) {
                if(++mode > 1) mode = 0;
            } else {
                switch(mode) {
                    case 0: f = handle_freq(k); break;
                    case 1: f = handle_pulse0(k); break;
                    default:;
                }
            }
            if(f) {
                set_freq(freq/10);
                set_pulse0(s0,t0);
            }
            show();
            k0 = k;
	    ledx();
        }
	Sleep(10);
    }

    return 0;
}

