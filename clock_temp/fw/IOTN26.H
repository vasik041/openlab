/*			- iotn26.h -

   This file #defines the internal register addresses for ATtiny26.
   Used with ICCA90 and AA90.

   Version: 1.04 [IANP]

*/

#pragma language=extended

#if (((__TID__ >> 8) & 0x7F) != 90)
#error This file should only be compiled by icca90/aa90
#endif
#if (((__TID__ >> 4) & 0x0F) != 0)
#error This file should only be included if the processor option is -v0
#endif

/* Include the SFR part if this file has not been included before,
 * OR this file is included by the assembler (SFRs must be defined in
 * each assembler module). AA90 has __TID__ bit 15 = 0. */
#if !defined(__IO2313_INCLUDED) || ((__TID__ >> 15) == 0)

/*==========================*/
/* Predefined SFR Addresses */
/*==========================*/


sfrb ACSR     	= 0x08;		/* Analog Comparator Control and Status Register */
//sfrb UBRR	= 0x09;		/* UART Baud Rate Register */
//sfrb UCR	= 0x0A;    	/* UART Control Register */
//sfrb USR	= 0x0B;		/* UART Status Register */
//sfrb UDR	= 0x0C;		/* UART I/O Data Register */


sfrb PIND	= 0x10; 	/* Input Pins, Port D */
sfrb DDRD	= 0x11;		/* Data Direction Register, Port D */
sfrb PORTD	= 0x12;    	/* Data Register, Port D */
sfrb PINB	= 0x16;		/* Input Pins, Port B */
sfrb DDRB	= 0x17;		/* Data Direction Register, Port B */
sfrb PORTB	= 0x18;		/* Data Register, Port B */

sfrb EECR	= 0x1C;		/* EEPROM Control Register */
sfrb EEDR	= 0x1D;   	/* EEPROM Data Register */
sfrb EEAR	= 0x1E;      	/* EEPROM Address Register */

sfrb WDTCR	= 0x21;		/* Watchdog Timer Control Register */

sfrw ICR1	= 0x24;         /* T/C 1 Input Capture Register */
sfrb ICR1L	= 0x24;
sfrb ICR1H	= 0x25;


//sfrw OCR1	= 0x2A;		/* Output Compare Register 1 */
sfrb OCR1A	= 0x2D;
sfrb OCR1B	= 0x2C;
sfrb OCR1C	= 0x2B;

sfrb TCNT1	= 0x2e;		/* Timer/Counter 1 */

sfrb TCCR1B	= 0x2f;		/* Timer/Counter 1 Control and Status Register */
sfrb TCCR1A	= 0x30;		/* Timer/Counter 1 Control Register */


sfrb TCNT0	= 0x32;		/* Timer/Counter 0 */
sfrb TCCR0	= 0x33;		/* Timer/Counter 0 Control Register */

sfrb MCUCR	= 0x35;		/* MCU general Control Register */


sfrb TIFR	= 0x38;		/* Timer/Counter Interrupt Flag register */
sfrb TIMSK	= 0x39;		/* Timer/Counter Interrupt MaSK register */
sfrb GIFR 	= 0x3A;		/* General Interrupt Flag Register */
sfrb GIMSK	= 0x3B;		/* General Interrupt MaSK register */

sfrb SP		= 0x3D;		/* Stack Pointer */
sfrb SREG	= 0x3F;		/* Status REGister */

sfrw ADC     	= 0x04;	/* ADC Data Register */
sfrb ADCL     	= 0x04;
sfrb ADCH     	= 0x05;
sfrb ADCSR     	= 0x06;	/* ADC Control and Status Register */
sfrb ADMUX     	= 0x07;	/* ADC Multiplexer Select Register */

sfrb OSCCAL	= 0x31;
sfrb MCUSR	= 0x34;

sfrb PINA	= 0x19; /* Input Pins, Port A */
sfrb DDRA	= 0x1A; /* Data Direction Register, Port A */
sfrb PORTA	= 0x1B; /* Data Register, Port A */

sfrb USIDR	= 0x0f; /* USI data register */
sfrb USISR	= 0x0e; /* USI status register */
sfrb USICR	= 0x0d;	/* USI control register */

/*=========================================================================*/
//
//***** EECR ******
//

#define	EERIE	3
#define	EEMWE	2
#define	EEWE	1
#define	EERE	0

/*=========================================================================*/
/*
 * ADCSR bits
 */

#define	ADEN	7
#define	ADSC	6
#define	ADFR	5
#define	ADIF	4
#define	ADIE	3
#define	ADPS2	2
#define	ADPS1	1
#define	ADPS0	0

/*=========================================================================*/
/*
 * USISR
 */

#define	USIOIF	6

/*
.equ	USISIF	=7
.equ	USIOIF	=6
.equ	USIPF	=5
.equ	USIDC	=4
.equ	USICNT3	=3
.equ    USICNT2 =2
.equ    USICNT1 =1
.equ    USICNT0 =0
 */

/*=========================================================================*/
/*
 * USICR
 */

#define	USITC	0
#define	USICLK	1
#define	USICS0	2
#define	USICS1	3
#define	USIWM0	4
#define	USIWM1	5
#define USIOIE  6
#define USISIE  7

/****************************************************************************/


#ifndef __IO2313_INCLUDED
#define __IO2313_INCLUDED

/* SFRs are local in assembler modules (so this file may need to be */
/* included in more than one module in the same source file), */
/* but #defines must only be made once per source file. */

/*==============================*/
/* Interrupt Vector Definitions */
/*==============================*/

/* NB! vectors are specified as byte addresses */

#define	RESET_vect		(0x00)
#define	INT0_vect		(0x02)
#define	PC03_vect		(0x04)
#define	TIMER1_CAPA_vect	(0x06)
#define	TIMER1_CAPB_vect	(0x08)
#define	TIMER1_OVF_vect		(0x0A)
#define	TIMER0_OVF_vect		(0x0C)
#define	USI_STRT_vect		(0x0E)
#define	USI_OVF_vect		(0x10)
#define	EE_RDY_vect		(0x12)
#define	ANA_COMP_vect		(0x14)
#define	ADC_vect		(0x16)


#endif /* __IO2313_INCLUDED (define part) */
#endif /* __IO2313_INCLUDED (SFR part) */
