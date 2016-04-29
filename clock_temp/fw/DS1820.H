/*=========================================================================*/
/*
 * Reading ds1820
 * This is free and unencumbered software released into the public domain.
 * LZs,2005
 */
/*=========================================================================*/

#define	DQ	6

#define	DQ0	clrbit(PORTB,DQ)
#define	DQ1	setbit(PORTB,DQ)

#define	DQX	(PINB & (1 << DQ))

/*=========================================================================*/

UCHR	ow_reset(void);
UCHR	read_byte(void);
UCHR	read_bit(void);
void	write_byte(UCHR b);
void	write_bit(UCHR b);
void	readTemp(void);

void	readROMCode(void);
UCHR	sendMatchROM(void);
void	readScratchPad(void);

void	Do_CRC(UCHR b);
void	u_delay(UCHR n);

/*=========================================================================*/
