/*=========================================================================*/
/*
 * Battery monitor
 * This is free and unencumbered software released into the public domain.
 * LZs,2020
 */
/*=========================================================================*/

#include "batmon.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>

/*=========================================================================*/
/*
 * delay for 1ms
 */

void wt1ms(void)
{
    _WDR();
    _delay_us(1000);
}

/*=========================================================================*/
/*
 * delay for n ms
 */

void delay(uint16_t n)
{
    while(n--)
	wt1ms();
}

/*=========================================================================*/

uint16_t get_aval(void)
{
    ADMUX = 3;
    wt1ms();
    ADCSR |= (1 << ADSC);
    while((ADCSR & (1 << ADSC)) != 0);
    return ADC;
}

uint16_t get_aval16(void)
{
uint8_t i;
uint16_t val = 0;
    for(i=0; i < 16; i++) {
        val += get_aval();
    }
    return (val >> 4);
}

/*=========================================================================*/

void show(uint16_t val)
{
uint8_t i;
    sbi(PORTA, LED1);
    sbi(PORTA, LED2);
    sbi(PORTA, LED3);
    sbi(PORTA, LED4);

    if(val >= LEVEL1 && val < LEVEL2) {
        for(i=0; i < 5; i++) {
            cbi(PORTA, LED1);
            delay(100);
            sbi(PORTA, LED1);
            delay(100);
        }
        return;
    }

    if(val >= LEVEL2 && val < LEVEL3) {
        cbi(PORTA, LED1);
        return;
    }

    if(val >= LEVEL3 && val < LEVEL4) {
        cbi(PORTA, LED1);
        cbi(PORTA, LED2);
        return;
    }

    if(val >= LEVEL4 && val < LEVEL5) {
        cbi(PORTA, LED1);
        cbi(PORTA, LED2);
        cbi(PORTA, LED3);
        return;
    }

    if(val >= LEVEL5) {
        cbi(PORTA, LED1);
        cbi(PORTA, LED2);
        cbi(PORTA, LED3);
        cbi(PORTA, LED4);
    }
}

/*=========================================================================*/

void test()
{
uint8_t i;
    for(i=0; i < 5; i++) {
        cbi(PORTA, LED1);
        cbi(PORTA, LED2);
        cbi(PORTA, LED3);
        cbi(PORTA, LED4);
        delay(100);
        sbi(PORTA, LED1);
        sbi(PORTA, LED2);
        sbi(PORTA, LED3);
        sbi(PORTA, LED4);
        delay(100);
    }
}

/*=========================================================================*/

int main(void)
{
uint16_t val;
    DDRA = 0xff-0x10;	// porta 4 in, 0-3,5-7 out
    PORTA = 0xff-0x10;
    DDRB = 0x38;
    PORTB = 0x38;

    _WDR();
    WDTCR = 0x0f;

    ADCSR = 0x80;	// ADC
    ADMUX = 3;          // PA4

    sbi(PORTA, LOCK);   // lock power
    delay(100);
    test();

    for(;;) {
	val = get_aval16();
	show(val);
	delay(1000);
        
        sbi(PORTA, LED1);
        sbi(PORTA, LED2);
        sbi(PORTA, LED3);
        sbi(PORTA, LED4);
        cbi(PORTA, LOCK); // unlock power
    }

    return 0;
}



