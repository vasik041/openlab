/*=========================================================================*/
/*
 * ds1820 operations
 * LZ, 2001-2016
 */
/*=========================================================================*/


#define clrbit(X,Y)     X &= ~(1 << Y)
#define setbit(X,Y)     X |=  (1 << Y)

/*=========================================================================*/

#define	DQ	0

#define	DQ0	clrbit(PORTC,DQ)
#define	DQ1	setbit(PORTC,DQ)

#define	DQX	(PINC & (1 << DQ))


#define	OW_NO_DEVICE	(16*99)
#define	OW_NO_DEVICE2	(16*98)
#define	OW_BAD_CRC	(16*97)

/*=========================================================================*/

uint8_t	ow_reset(void);
uint8_t	read_byte(void);
uint8_t	read_bit(void);
void	write_byte(uint8_t b);
void	write_bit(uint8_t b);
void	readTemp(void);
void	Do_CRC(uint8_t b);
void	u_delay(uint8_t n);

void	readTemp1(void);
void	readROM(void);
void	readTempBus(void);

/*=========================================================================*/
