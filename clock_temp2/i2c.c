/*=========================================================================*/
/*
 * I2C operations
 * LZs,2010
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

#include "i2c.h"

/*=========================================================================*/

#define SDABIT	6
#define SCLBIT	7

#define	SCL0	clrbit(PORTA,SCLBIT)
#define	SCL1    setbit(PORTA,SCLBIT)

#define	SDA0    clrbit(PORTA,SDABIT)
#define	SDA1    setbit(PORTA,SDABIT)

#define	SCL	(PINA & (1 << SCLBIT))
#define	SDA	(PINA & (1 << SDABIT))

/*=========================================================================*/

volatile uint8_t e2err;
volatile uint8_t e2plc;

/*=========================================================================*/
/*
 * Two wire/I2C - CPU communication error status table
 * and subroutine
 *
 * code error    status mode
 * --------      -----------------------------------
 * 1 : SCL locked low by device (bus is still busy)
 * 2 : SDA locked low by device (bus is still busy)
 * 3 : No acknowledge from device (no handshake)
 * 4 : SDA bus not released for master to generate STOP bit
 * ------------------------------------------------------
 *
 * Subroutine to identify the status of the serial clock
 * (SCL) and serial data
 * (SDA) condition according to the error status table.
 * Codes generated are useful for bus/device diagnosis.
 */

void e2_err(uint8_t err_code,uint8_t err_place)
{
	if(e2err == 0) {
		e2err = err_code;
		e2plc = err_place;
	}
}

/*=========================================================================*/

void i2c_init(void)
{
	setbit(DDRA,SDABIT);	//sda out
	setbit(DDRA,SCLBIT);	//scl out
	SCL1;
	SDA1;
	e2err = 0;
	e2plc = 0;
}

/*=========================================================================*/

void wx(void)
{
	_delay_us(10);
}

/*=========================================================================*/
/*
 * START bus communication routine
 */

void bstart(void)
{
	SCL1; wx();
	SDA1; wx();
	SDA0; wx();		/* SDA goes low during SCL high */
	SCL0; wx();		/* Start clock train */
}

/*=========================================================================*/
/*
 * STOP bus communication routine
 */

void bstop(void)
{
	SDA0;			// Return SDA to low
	wx();

	SCL1;			// Set SCL high
	wx();
	if(SCL == 0) e2_err(1,1); // No, SCL locked low by device

	SDA1;
	wx();	         	// SDA goes from low to high during SCL high
	if(SDA == 0) e2_err(4,2); // High?

	SCL0;
	wx();			// No, SDA bus not release for STOP
}

/*=========================================================================*/
/*
 * Serial data send from PIC16CXX to serial EEPROM,
 *  bit-by-bit subroutine
 */

uint8_t bitin(void)
{
uint8_t d;
	clrbit(DDRA,SDABIT);	// sda in

	SCL1;
	wx();			// Clock high
	if(SCL == 0) e2_err(1,3); // Skip if SCL is high

	d = 0;
	if(SDA != 0) d = 1;	// Read SDA pin
	SCL0;			// Return SCL to low
	wx();
	return d;
}

/*=========================================================================*/
/*
 * Serial data receive from serial EEPROM to PIC16CXX,
 * bit-by-bit subroutine
 */

void bitout(uint8_t b)
{
	setbit(DDRA,SDABIT);		// sda out
	if(b) {
		SDA1;			// Output bit 0
		wx();
		if(SDA == 0) e2_err(2,4);// Check for error code 2
	} else {
		SDA0;			// Output bit 0
		wx();
	}

	SCL1;
	wx();
	if(SCL == 0) e2_err(1,5);	// SCL locked low

	SCL0;
	wx();				// Return SCL to low
}

/*=========================================================================*/
/*
 * RECEIVE DATA subroutine
 */

uint8_t rx(uint8_t ack)
{
int8_t i;
uint8_t d,b;
	for(i=7,d=0; i >= 0; i--) {
		b = bitin();
		d +=  (b << i);
	}
	bitout(ack);			// Set acknowledge bit
	return d;
}

/*=========================================================================*/
/*
 * TRANSMIT DATA subroutine
 */

void tx(uint8_t d)
{
int i;
	for(i=7; i >= 0; i--) {
		bitout( (d >> i) & 1 );
	}
	if(bitin() != 0) {
		e2_err(3,6);	// Read acknowledge bit
	}
	setbit(DDRA,SDABIT);	// sda out
}

/*=========================================================================*/

#define	rxACK		0
#define	rxNO_ACK	1

/*=========================================================================*/

uint8_t rd_clk(uint8_t adr)
{
uint8_t d;
	e2err = 0;
	e2plc = 0;

	bstart();		// Generate START bit
	tx(0xd0);		// Output SLAVE address and r/w=0
	tx(adr);		// Output address
	bstop();		// Generate STOP bit

	bstart();		// Generate START bit
	tx(0xd1);               // Output SLAVE address and r/w=1
	d = rx(rxNO_ACK);	// READ in data and acknowledge
	bstop();		// Generate STOP bit
	return d;
}

/*=========================================================================*/

void wr_clk(uint8_t adr,uint8_t d)
{
	e2err = 0;
	e2plc = 0;
	bstart();		// Generate START bit
	tx(0xd0);		// Output SLAVE address and r/w=0
	tx(adr);		// Output address
	tx(d);			// Output DATA and detect acknowledgement
	bstop();		// Generate STOP bit
}

/*=========================================================================*/
