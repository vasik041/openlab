/*=========================================================================*/
/*
 * Reading ds1820
 * LZs,2009
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

#define	DQ	6

#define	DQ0	cbi(PORTB,DQ)
#define	DQ1	sbi(PORTB,DQ)

#define	DQX	(PINB & (1 << DQ))

/*=========================================================================*/

uint8_t	ow_reset(void);
uint8_t	read_byte(void);
uint8_t	read_bit(void);
void	write_byte(uint8_t b);
void	write_bit(uint8_t b);
void	readTemp(void);

void	readROMCode(void);
uint8_t	sendMatchROM(void);
void	readScratchPad(void);

void	Do_CRC(uint8_t b);

/*=========================================================================*/
