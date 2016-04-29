/*=========================================================================*/
/*
 * I2C operations
 * This is free and unencumbered software released into the public domain.
 * LZs,2005
 */
/*=========================================================================*/

#include "iotn26.h"
#include <ina90.h>
#include "types.h"
#include "i2c.h"
#include "main.h"

/*=========================================================================*/

#define SDABIT	4
#define SCLBIT	5

#define	SCL0	clrbit(PORTB,SCLBIT)
#define	SCL1    setbit(PORTB,SCLBIT)

#define	SDA0    clrbit(PORTB,SDABIT)
#define	SDA1    setbit(PORTB,SDABIT)

#define	SCL	(PINB & (1 << SCLBIT))
#define	SDA	(PINB & (1 << SDABIT))

/*=========================================================================*/

UCHR	e2err;

/*=========================================================================*/

void e2_err(UCHR err_code)
{
	if(e2err == 0) e2err = err_code;
}

/*=========================================================================*/

void i2c_init(void)
{
	setbit(DDRB,SDABIT);	//sda out
	setbit(DDRB,SCLBIT);	//scl out
	SCL1;
	SDA1;
}

/*=========================================================================*/

void wx(void)
{
UCHR w;
	for(w=10; w; w--);
}

/*=========================================================================*/

void bstart(void)
{
	SCL1; wx();
	SDA1; wx();
	SDA0; wx();		/* SDA goes low during SCL high */
	SCL0; wx();		/* Start clock train */
}

/*=========================================================================*/

void bstop(void)
{
	SDA0; wx();		/* Return SDA to low */
	SCL1; wx();		/* Set SCL high      */
	if(SCL == 0) e2_err(1); /* No, SCL locked low by device */

	SDA1; wx();         	/* SDA goes from low to high during SCL high */
	if(SDA == 0) e2_err(4); /* High? */
//
//	SCL0; wx();		/* No, SDA bus not release for STOP */
}

/*=========================================================================*/

UCHR bitin(void)
{
UCHR d;
	clrbit(DDRB,SDABIT);	// sda in
	SCL1; wx();		/* Clock high */
	if(SCL == 0) e2_err(1);	/* Skip if SCL is high */
	d = 0;
	if(SDA != 0) d = 1;	/* Read SDA pin */
	SCL0; wx();		/* Return SCL to low */
	return d;
}

/*=========================================================================*/

void bitout(UCHR b)
{
	setbit(DDRB,SDABIT);		// sda out
	if(b) {
		SDA1; wx();		/* Output bit 0 */
		if(SDA == 0) e2_err(2);	/* Check for error code 2 */
	} else {
		SDA0; wx();		/* Output bit 0 */
	}
	SCL1; wx();
	if(SCL == 0) e2_err(1);		/* SCL locked low */
	SCL0; wx();			/* Return SCL to low */

}

/*=========================================================================*/

UCHR rx(UCHR ack)
{
int i;
UCHR d,b;
	for(i=7,d=0; i >= 0; i--) {
		b = bitin();
		d +=  (b << i);
	}
	bitout(ack);			// Set acknowledge bit
	return d;
}

/*=========================================================================*/

void tx(UCHR d)
{
int i;
	for(i=7; i >= 0; i--)
		bitout( (d >> i) & 1 );

	if(bitin() != 0) e2_err(3);	/* Read acknowledge bit */
	setbit(DDRB,SDABIT);		// sda out
}

/*=========================================================================*/

#define	rxACK		0
#define	rxNO_ACK	1

/*=========================================================================*/

UCHR rd_clk(UCHR adr)
{
UCHR d;
	e2err = 0;
	bstart();		/* Generate START bit */
	tx(0xa0);
	tx(adr);		/* dummy write */
	bstart();
	tx(0xa1);
	d = rx(rxNO_ACK);	/* READ in data and acknowledge */
	bstop();		/* Generate STOP bit */
	return d;
}

/*=========================================================================*/

void wr_clk(UCHR adr,UCHR d)
{
	e2err = 0;
	bstart();		/* Generate START bit */
	tx(0xa0);		/* Output SLAVE address */
	tx(adr);		/* Output WORD address */
	tx(d);			/* Output DATA and detect acknowledgement */
	bstop();		/* Generate STOP bit */
}

/*=========================================================================*/
