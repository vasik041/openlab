/*=========================================================================*/
/*
 * Software serial port emulation
 * This is free and unencumbered software released into the public domain.
 * LZ,2002
 */
/*=========================================================================*/

#define	TXD 	3

void sftbit(uint8_t c);
void putch_s(uint8_t c);
void nl(void);
void px(uint8_t n);
void putx(uint8_t n);
void putxw(uint16_t n);
void putxq(uint32_t n);
