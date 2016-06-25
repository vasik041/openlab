/*=========================================================================*/
/*
 * Serial communication API
 * LZ,2009-2015
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

#include "main.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>

#include "usart.h"

volatile uint8_t timeout;


/*=========================================================================*/

void USART_Init(uint16_t baud)
{
	UBRRH = (uint8_t)((baud >> 8) & 0xff);	// Set baud rate
	UBRRL = (uint8_t)(baud & 0xff);
	UCSRB = (1<<RXEN)|(1<<TXEN);		// Enable receiver and transmitter 
	UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);// Set frame format: 8data, 2stop bit 
	timeout = 0;
}

/*=========================================================================*/

void USART_Transmit(uint8_t d)
{
	while ( !( UCSRA & (1<<UDRE)) );
	UDR = d;
}

/*=========================================================================*/

uint8_t USART_Ready(void)
{
	return ((UCSRA & (1<<RXC)) == 0) ? 0 : 1;
}

/*=========================================================================*/

uint8_t USART_Receive(uint16_t t)
{
uint16_t w;

	for(w=0; w < t && !USART_Ready(); w++) {
		_delay_ms(1);
	}
	if(!USART_Ready()) {
		timeout = 1;
		return 0;
	}
	timeout = 0;
	return UDR;
}

uint8_t USART_Timeout(void)
{
	return timeout;
}


void USART_Flush( void )
{
uint8_t dummy;
	while ( UCSRA & (1<<RXC) )
		dummy = UDR;
}

/*=========================================================================*/

uint8_t h2d(uint8_t h)
{
	if(h >= '0' && h <= '9') return h-'0';
	if(h >= 'A' && h <= 'F') return h-'A'+10;
	if(h >= 'a' && h <= 'f') return h-'a'+10;
	return 0;
}

uint8_t rhex(void)
{
uint8_t h;
uint8_t l;
	h = USART_Receive(STD_TIMEOUT);
	if(USART_Timeout()) return 0;
	l = USART_Receive(STD_TIMEOUT);
	if(USART_Timeout()) return 0;
	return (h2d(h) << 4) + h2d(l);
}


uint8_t d2h(uint8_t b)
{
	b &= 0x0f;
	if(b < 10) return '0'+b;
	return 'a'+b-10;
}

void thex(uint8_t b)
{
	USART_Transmit(d2h(b >> 4));
	USART_Transmit(d2h(b));
}

void thexw(uint16_t b)
{
	USART_Transmit(d2h(b >> 12));
	USART_Transmit(d2h(b >> 8));
	USART_Transmit(d2h(b >> 4));
	USART_Transmit(d2h(b));
}
