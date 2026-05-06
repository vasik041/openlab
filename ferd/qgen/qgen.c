/*=========================================================================*/
/*
 * sin/cos + mod gen
 * LZs, 2015
 */
/*=========================================================================*/

#include "qgen.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>
#include <math.h>

#define	FREQ_MAX  20000 // hz
#define	MOD_MAX	  500   // hz

volatile uint16_t phase0;
volatile uint16_t phase1;
volatile uint16_t step;

volatile uint16_t phasem;
volatile uint16_t stepm;


// 256
const prog_char sine[] = {
	  0,  0,  1,  2,  3,  3,  4,  5,    6,  6,  7,  8,  8,  9, 10, 11,  
	 11, 12, 13, 13, 14, 15, 15, 16,   17, 17, 18, 19, 19, 20, 20, 21,  
	 21, 22, 22, 23, 23, 24, 24, 25,   25, 26, 26, 26, 27, 27, 28, 28,  
	 28, 28, 29, 29, 29, 29, 30, 30,   30, 30, 30, 30, 30, 30, 30, 30,  
	
	 31, 30, 30, 30, 30, 30, 30, 30,   30, 30, 30, 29, 29, 29, 29, 28,  
	 28, 28, 28, 27, 27, 26, 26, 26,   25, 25, 24, 24, 23, 23, 22, 22,  
	 21, 21, 20, 20, 19, 19, 18, 17,   17, 16, 15, 15, 14, 13, 13, 12,  
	 11, 11, 10,  9,  8,  8,  7,  6,    6,  5,  4,  3,  3,  2,  1,  0,  
	
	  0,  0,255,254,253,253,252,251,  250,250,249,248,248,247,246,245,  
	245,244,243,243,242,241,241,240,  239,239,238,237,237,236,236,235,  
	235,234,234,233,233,232,232,231,  231,230,230,230,229,229,228,228,  
	228,228,227,227,227,227,226,226,  226,226,226,226,226,226,226,226,  
	
	225,226,226,226,226,226,226,226,  226,226,226,227,227,227,227,228,  
	228,228,228,229,229,230,230,230,  231,231,232,232,233,233,234,234,  
	235,235,236,236,237,237,238,239,  239,240,241,241,242,243,243,244,  
	245,245,246,247,248,248,249,250,  250,251,252,253,253,254,255,  0
};

/*=========================================================================*/

int16_t m;
int16_t a;
int16_t b;

// 10us
ISR (TIMER0_OVF_vect)
{
    m = (int8_t)pgm_read_byte(sine + (phasem >> 8));
    a = (int8_t)pgm_read_byte(sine + (phase0 >> 8));
    b = (int8_t)pgm_read_byte(sine + (phase1 >> 8));
    DAC0 = 32 + ((m * a) >> 5);
    DAC1 = 32 + ((m * b) >> 5);
    phase0 += step;
    phase1 += step;
    phasem += stepm;
}

/*=========================================================================*/

void USART_Init(uint16_t baud)
{
    UBRR0H = (uint8_t)((baud >> 8) & 0xff);	// Set baud rate
    UBRR0L = (uint8_t)(baud & 0xff);
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);		 // Enable receiver and transmitter 
    UCSR0C = (1<<USBS0) | (1<<UCSZ00) | (1<<UCSZ01); // Set frame format: 8data, 2stop bit 
}


void USART_Transmit( uint8_t data )
{
    while ( !( UCSR0A & (1<<UDRE0)) );	// Wait for empty transmit buffer
    UDR0 = data;			// Put data into buffer, sends the data
}


uint8_t USART_Ready()
{
    return ((UCSR0A & (1<<RXC0)) == 0) ? 0 : 1;
}


uint8_t USART_Receive(void)
{
    while ( !USART_Ready() );	// Wait for data to be received
    return UDR0;		// Get and return received data from buffer
}

/*=========================================================================*/

void printd(int16_t n)
{
uint8_t i;
int16_t m = 10000;
uint8_t f = 0;
    for(i=0; i < 5; i++) {
        if(n/m || f) {
            f = 1;
            USART_Transmit(n/m + '0');
        } else {
            USART_Transmit(' ');
        }
	n %= m;
	m /= 10;
    }
}

void print_pf()
{
    USART_Transmit('f');
    USART_Transmit(' ');
    printd(step);
    USART_Transmit(' ');
    USART_Transmit('h');
    USART_Transmit('z');

    if(stepm) {
	USART_Transmit(' ');
        USART_Transmit('m');
        USART_Transmit(' ');
	printd(stepm);
        USART_Transmit(' ');
	USART_Transmit('h');
	USART_Transmit('z');
    }

    USART_Transmit('\r');
    USART_Transmit('\n');
}


/*=========================================================================*/

int main(void)
{
uint8_t c;
uint32_t cnt=0;

    DDRC = 0x3f;	// portc out, DAC0
    PORTC = 0;
    DDRB = 0x3f;	// portb out, DAC1
    PORTC = 0;
    DDRD = 0x82;	// LED, TXD

    USART_Init(BR_9600);

    TCCR0B = 1;
    TIMSK0 = 1;

    phase0 = 0;
    phase1 = 64 << 8;
    phasem = 64 << 8;
    step = 500;
    stepm = 0;
    sei();

    for( ; ; ) {
	if(USART_Ready()) {
	    c = USART_Receive();
	    switch(c) {
		case '=': if(step < FREQ_MAX-1) step++; break;
		case '-': if(step > 1) step--; break;
		case '2': if(step < FREQ_MAX-10) step+=10; break;
		case '1': if(step > 10) step-=10; break;

		case '4': if(stepm < MOD_MAX-1) stepm++; break;
		case '3': if(stepm > 0) stepm--; break;
		case '6': if(stepm < MOD_MAX-10) stepm+=10; break;
		case '5': if(stepm > 10) stepm-=10; break;
	    }
	    if(stepm == 0) { cli(); phasem = 64<<8; sei(); }
	    print_pf();
	}

	if(++cnt > 100000) {
	    PORTD = PIND ^ 0x80; // blink
	    cnt = 0;
	}
    }
    return 0;
}

/*=========================================================================*/


