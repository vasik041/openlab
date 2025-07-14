/*=========================================================================*/
/*
 * Reading ds1820
 * This is free and unencumbered software released into the public domain.
 * LZs,2009,2015
 */
/*=========================================================================*/

#define	DQ	0

#define	DQ0	cbi(PORTA,DQ)
#define	DQ1	sbi(PORTA,DQ)

#define	DQX	(PINA & (1 << DQ))

/*=========================================================================*/

uint8_t	ow_reset2(void);
uint8_t	read_byte2(void);
uint8_t	read_bit2(void);
void write_byte2(uint8_t b);
void write_bit2(uint8_t b);
void readTemp2(void);
void Do_CRC(uint8_t b);

/*=========================================================================*/
