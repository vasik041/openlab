/*=========================================================================*/
/*
 * Software I2C
 * LZ,2010-2022
 */
/*=========================================================================*/

#include "include/due_sam3x.h"
#include "i2c.h"

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

void SCL0()
{
    PIOD->PIO_CODR = PIO_PD2;
}

void SCL1()
{
    PIOD->PIO_SODR = PIO_PD2;
}

uint8_t SCL()
{
    return PIO_Get(PIOD, PIO_INPUT, PIO_PD2) != 0;
}


void SDA0()
{
    PIOD->PIO_CODR = PIO_PD0;
}

void SDA1()
{
    PIOD->PIO_SODR = PIO_PD0;
}

uint8_t	SDA()
{
    return PIO_Get(PIOD, PIO_INPUT, PIO_PD0) != 0;
}

void SDAOUT()
{
}

void SDAIN()
{
}

void i2c_init(void)
{
    pmc_enable_periph_clk(ID_PIOD);
    PIO_Configure(PIOD, PIO_OUTPUT_1, PIO_PD0, PIO_OPENDRAIN);	/* Board pin 25 == PD0 SDA */
    PIO_Configure(PIOD, PIO_OUTPUT_1, PIO_PD2, PIO_OPENDRAIN);	/* Board pin 27 == PD2 SCL */
    e2err = 0;
    e2plc = 0;
}


/*=========================================================================*/

static void wx(void)
{
volatile uint16_t w;
    for(w = 0; w < 1000; w++);
}

/*=========================================================================*/
/*
 * START bus communication routine
 */

void bstart(void)
{
	SCL1(); wx();
	SDA1(); wx();
	SDA0(); wx();		/* SDA goes low during SCL high */
	SCL0(); wx();		/* Start clock train */
}

/*=========================================================================*/
/*
 * STOP bus communication routine
 */

void bstop(void)
{
	SDA0();			// Return SDA to low
	wx();

	SCL1();			// Set SCL high
	wx();
	if(SCL() == 0) e2_err(1,1); // No, SCL locked low by device

	SDA1();
	wx();	         	// SDA goes from low to high during SCL high
	if(SDA() == 0) e2_err(4,2); // High?

	SCL0();
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
	SDAIN();

	SCL1();
	wx();			// Clock high
	if(SCL() == 0) e2_err(1,3); // Skip if SCL is high

	d = 0;
	if(SDA() != 0) d = 1;	// Read SDA pin
	SCL0();			// Return SCL to low
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
	SDAOUT();
	if(b) {
		SDA1();			// Output bit 0
		wx();
		if(SDA() == 0) e2_err(2,4);// Check for error code 2
	} else {
		SDA0();			// Output bit 0
		wx();
	}

	SCL1();
	wx();
	if(SCL() == 0) e2_err(1,5);	// SCL locked low

	SCL0();
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
		d += (b << i);
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
	SDAOUT();
}

/*=========================================================================*/

#define	EEPROM_ADDR	0

uint8_t rd_eeprom(uint16_t adr)
{
uint8_t d;
	e2err = 0;
	e2plc = 0;

	bstart();
	tx(0xa0+EEPROM_ADDR);
	tx(adr >> 8);
	tx(adr);
	bstop();

	bstart();
	tx(0xa1+EEPROM_ADDR);
	d = rx(rxNO_ACK);
	bstop();
	return d;
}


void wr_eeprom(uint16_t adr,uint8_t d)
{
	e2err = 0;
	e2plc = 0;
	bstart();
	tx(0xa0+EEPROM_ADDR);
	tx(adr >> 8);
	tx(adr);
	tx(d);
	bstop();
	Sleep(5);
}

/*=========================================================================*/
