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
#include "i2c.h"

#define FREQ_MIN 10
#define FREQ_MAX 1000

volatile uint32_t pulse0;
volatile uint32_t pulse1;
volatile uint32_t period;
volatile uint32_t phase;
volatile uint32_t step;
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

//==============================================================================

uint8_t handle_period(uint8_t k)
{
uint8_t f = 0;
    switch(k) {
        case KEY_UP:   if(period < FREQ_MAX-step) { period += step; f=1; } break;
        case KEY_DOWN: if(period >= FREQ_MIN+step) { period -= step; f=1; } break;
        default:;
    }
    return f;
}

uint8_t handle_pulse0(uint8_t k)
{
uint8_t f = 0;
    switch(k) {
        case KEY_UP:   if(pulse0 < FREQ_MAX-step) { pulse0 += step; f=1; } break;
        case KEY_DOWN: if(pulse0 >= FREQ_MIN+step)  { pulse0 -= step; f=1; } break;
        default:;
    }
    return f;
}

uint8_t handle_pulse1(uint8_t k)
{
uint8_t f = 0;
    switch(k) {
        case KEY_UP:   if(pulse0 < FREQ_MAX-step) { pulse1 += step; f=1; } break;
        case KEY_DOWN: if(pulse0 >= FREQ_MIN+step)  { pulse1 -= step; f=1; } break;
        default:;
    }
    return f;
}

uint8_t handle_phase(uint8_t k)
{
uint8_t f = 0;
    switch(k) {
        case KEY_UP:   if(phase < FREQ_MAX-step) { phase += step; f=1; } break;
        case KEY_DOWN: if(phase >= FREQ_MIN+step)  { phase -= step; f=1; } break;
        default:;
    }
    return f;
}

//==============================================================================

uint32_t read32(uint16_t addr)
{
    uint32_t a = rd_eeprom(addr);
    uint32_t b = rd_eeprom(addr+1);
    uint32_t c = rd_eeprom(addr+2);
    uint32_t d = rd_eeprom(addr+3);
    return a + (b << 8) + (c << 16) + (d << 24);
}


void read_eeprom()
{
    pulse0 = read32(0);
    if(pulse0 < FREQ_MIN || pulse0 > FREQ_MAX) pulse0 = 10;

    pulse1 = read32(4);
    if(pulse1 < FREQ_MIN || pulse1 > FREQ_MAX) pulse1 = 10;
    
    period = read32(8);
    if(period < FREQ_MIN || period > FREQ_MAX) period = 500;

    phase = read32(12);
    if(phase < FREQ_MIN || phase > FREQ_MAX) phase = 10;

    step = read32(16);
    if(step != 1 && step != 2 && step != 5 && step != 10) step = 10;
}

void write32(uint16_t addr, uint32_t val)
{
    wr_eeprom(addr,   val);
    wr_eeprom(addr+1, val>>8);
    wr_eeprom(addr+2, val>>16);
    wr_eeprom(addr+3, val>>24);
}

void write_eeprom()
{
    write32(0, pulse0);
    write32(4, pulse1);
    write32(8, period);
    write32(12,phase);
    write32(16,step);
}

/*

extern volatile uint8_t e2err;

void i2c_pins_test()
{
    for(;;) {
        uint8_t k = get_key();
        switch (k) {
            case KEY_UP: SDA1(); break;
            case KEY_DOWN: SDA0(); break;
            case KEY_LEFT: SCL1(); break;
            case KEY_RIGHT: SCL0(); break;
        }
        Sleep(1);
        locxy(0,0);
        putd(SDA(), 3, 0);
        putd(SCL(), 3, 0);
    }
}

void rw_test()
{
    for(;;) {
        locxy(0,0);
        wr_eeprom(0, 0x55);
        wr_eeprom(1, 0xaa);
        putd(rd_eeprom(0), 3, 0);
        putd(rd_eeprom(1), 3, 0);
        putd(rd_eeprom(2), 3, 0);
        putd(e2err, 3, 0);
        Sleep(1000);
    }
}

void keys_test()
{
    for(;;) {
        locxy(0,0);
        putd(adc_get1(7), 5, 0);
        Sleep(1000);
    }
}

*/

//==============================================================================


int main(void)
{
uint8_t k0 = 0;
uint8_t f = 0;
uint16_t save = 0;
    init_controller(); 						/* The general init (clock, libc, watchdog ...) */
    PIO_Configure(PIOB, PIO_OUTPUT_1, PIO_PB27, PIO_DEFAULT);	/* Board pin 13 == PB27 LED */

//    uart_init(115200);
//    uart_puts("Hello world!\n");

    display_init();
    puts1("Hello world!");
    adc_init1();
    adc_enable_channel(ADC,7);

    pwm_init();
    i2c_init();
    pulse0 = pulse1 = 10;
    period = 500;
    phase = 10;
    step = 10;
    mode = 0;
    read_eeprom();
    pwm_set(0, period, pulse0);
    pwm_set(1, period, phase);
    pwm_set(2, period, phase+pulse1);

//    i2c_pins_test();

    show();
    get_key();
    for(;;) {
        uint8_t k = get_key();
        if(k != k0) {
            if(k == KEY_SELECT) {
                if(step == 1) step = 2;
                else if(step == 2) step = 5;
                else if(step == 5) step = 10;
                else if(step == 10) step = 1;
                save = 1000;
                clrscr(0); locxy(0,0); puts1("step "); putd(step, 2, 0); Sleep(1000);
            } else if(k == KEY_LEFT || k == KEY_RIGHT) {
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
                pwm_set(1, period, phase);
                pwm_set(2, period, phase+pulse1);
                save = 1000;
            }
            show();
            k0 = k;
	    ledx();
        }
        if(save) {
            save--;
            if(!save) {
                write_eeprom();
                clrscr(0);
                locxy(0,0);
                puts1("saved");
                Sleep(1000);
                show();
            }
        }
	Sleep(10);
    }

    return 0;
}

