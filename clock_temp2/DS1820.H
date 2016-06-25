/****************************************************************************/
/*
 * ds1820 operations
 * LZs,2010
 * This is free and unencumbered software released into the public domain.
 */
/****************************************************************************/

typedef unsigned char UCHR;
typedef unsigned int  UINT;

#define clrbit(X,Y)     X &= ~(1 << Y)
#define setbit(X,Y)     X |=  (1 << Y)

/*=========================================================================*/

#define	DQ	6

#define	DQ0	clrbit(PORTD,DQ)
#define	DQ1	setbit(PORTD,DQ)

#define	DQX	(PIND & (1 << DQ))


#define	OW_NO_DEVICE	(16*99)
#define	OW_NO_DEVICE2	(16*98)
#define	OW_BAD_CRC	(16*97)

/*=========================================================================*/

UCHR	ow_reset(void);
UCHR	read_byte(void);
UCHR	read_bit(void);
void	write_byte(UCHR b);
void	write_bit(UCHR b);
void	readTemp(void);
void	Do_CRC(UCHR b);
void	u_delay(UCHR n);

void	readTemp1(void);
void	readROM(void);
void	readTempBus(void);

/*=========================================================================*/
