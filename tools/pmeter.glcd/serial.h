/*=========================================================================*/
/*
 * Software serial port emulation
 * LZ,2002
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

#define	TXD 	3

void sftbit(uint8_t c);
void putch_s(uint8_t c);
void puts_s(const prog_char *s);

void nl(void);
void px(uint8_t n);
void putx(uint8_t n);
void putxw(uint16_t n);
void putxq(uint32_t n);

void putd2_s(uint8_t n);
void putd4_s(uint16_t n);
