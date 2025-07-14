/*=========================================================================*/
/*
 * Reading ds1820
 * This is free and unencumbered software released into the public domain.
 * LZs,2009
 */
/*=========================================================================*/

#define	DQ	1

#define	DQ0	cbi(PORTA,DQ)
#define	DQ1	sbi(PORTA,DQ)
#define	DQX	(PINA & (1 << DQ))

/*=========================================================================*/

uint8_t	ow_reset(void);
uint8_t	read_byte(void);
uint8_t	read_bit(void);
void write_byte(uint8_t b);
void write_bit(uint8_t b);
void readTemp(void);
void readTemp2(void);

void Do_CRC(uint8_t b);

/*=========================================================================*/
