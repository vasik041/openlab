;***************************************************************************
;* A P P L I C A T I O N   N O T E   F O R   T H E   A V R   F A M I L Y
;* 
;* Number		:AVR000
;* File Name		:"tn22def.inc"
;* Title		:Register/Bit Definitions for the ATtiny22
;* Date	 		:99.01.28
;* Version		:1.30
;* Support telephone	:+47 72 88 43 88 (ATMEL Norway)
;* Support fax		:+47 72 88 43 99 (ATMEL Norway)
;* Support E-Mail	:avr@atmel.com
;* Target MCU		:ATtiny22
;*
;* DESCRIPTION
;* When including this file in the assembly program file, all I/O register
;* names and I/O register bit names appearing in the data book can be used.
;* 
;* The Register names are represented by their hexadecimal addresses.
;* 
;* The Register Bit names are represented by their bit number (0-7).
;* 
;* Please observe the difference in using the bit names with instructions
;* such as "sbr"/"cbr" (set/clear bit in register) and "sbrs"/"sbrc" 
;* (skip if bit in register set/cleared). The following example illustrates
;* this:
;* 
;* in	r16,PORTB		;read PORTB latch
;* sbr	r16,(1<<PB6)+(1<<PB5)	;set PB6 and PB5 (use masks, not bit#)
;* out  PORTB,r16		;output to PORTB
;*
;* in	r16,TIFR		;read the Timer Interrupt Flag Register
;* sbrc	r16,TOV0		;test the overflow flag (use bit#)
;* rjmp	TOV0_is_set		;jump if set
;* ...				;otherwise do something else
;***************************************************************************

;***** Specify Device
.device ATtiny22

;***** I/O Register Definitions
.equ	SREG	=$3f
.equ	SPL	=$3d
.equ	SPH	=$3e
.equ	GIMSK	=$3b
.equ	GIFR	=$3a
.equ	TIMSK	=$39
.equ	TIFR	=$38
.equ	MCUCR	=$35
.equ	MCUSR	=$34
.equ	TCCR0	=$33
.equ	TCNT0	=$32
.equ	WDTCR	=$21

.equ	EEAR	=$1e
.equ	EEARL	=$1e
.equ	EEARH	=$1f
.equ	EEDR	=$1d
.equ	EECR	=$1c

.equ	PORTB	=$18
.equ	DDRB	=$17
.equ	PINB	=$16

.equ	PORTA	=$1B
.equ	DDRA	=$1A
.equ	PINA	=$19

.equ	OSCCAL	=$31
.equ    ACSR   	=$08	; Analog Comparator Control and Status Register

.equ	PCMSK0	=$23
.equ	PCMSK1	=$22

;***** Bit Definitions

;MCUSR
.equ	WDRF	=3
.equ	BORF	=2
.equ	EXTRF	=1
.equ	PORF	=0

.equ	INT0	=6
.equ	INTF0	=6

.equ	TOIE0	=1
.equ	TOV0	=1

.equ	SE	=5
.equ	SM	=4
.equ	ISC01	=1
.equ	ISC00	=0

.equ	CS02	=2
.equ	CS01	=1
.equ	CS00	=0

;WDTCR
.equ	WDIF	=7
.equ	WDIE	=6
.equ	WDP3	=5
.equ	WDCE	=4
.equ	WDE	=3
.equ	WDP2	=2
.equ	WDP1	=1
.equ	WDP0	=0

;EECR
.equ	EEPM1	=5
.equ	EEPM0	=4
.equ	EERIE	=3
.equ	EEMPE	=2
.equ	EEPE	=1
.equ	EERE	=0

.equ	PB4	=4
.equ	PB3	=3
.equ	PB2	=2
.equ	PB1	=1
.equ	PB0	=0

.equ	DDB4	=4
.equ	DDB3	=3
.equ	DDB2	=2
.equ	DDB1	=1
.equ	DDB0	=0

.equ	PINB4	=4
.equ	PINB3	=3
.equ	PINB2	=2
.equ	PINB1	=1
.equ	PINB0	=0

.equ	PA7	=7
.equ	PA6	=6
.equ	PA5	=5
.equ	PA4	=4
.equ	PA3	=3
.equ	PA2	=2
.equ	PA1	=1
.equ	PA0	=0

.def	XL	=r26
.def	XH	=r27
.def	YL	=r28
.def	YH	=r29
.def	ZL	=r30
.def	ZH	=r31

.equ    RAMEND  =$DF    ;Last On-Chip SRAM Location
.equ	XRAMEND =$DF
.equ	E2END	=$7F
.equ	FLASHEND=$3FF


.equ	INT0addr=$001	;External Interrupt0 Vector Address
.equ	OVF0addr=$002	;Overflow0 Interrupt Vector Address

