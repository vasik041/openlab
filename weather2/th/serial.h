/*=========================================================================*/
/*
 * Software serial port emulation
 * This is free and unencumbered software released into the public domain.
 * LZ,2002
 */
/*=========================================================================*/

#define TXPORT	PORTB
#define	TXD 	5

#define	RXD	7	//pa7

void sftbit(uint8_t c);
void putch_s(uint8_t c);
void puts_s(const prog_char *s);
void nl(void);
void putd_s(uint8_t n);
void enable_rcvr(void);
uint8_t rcvd(void);
