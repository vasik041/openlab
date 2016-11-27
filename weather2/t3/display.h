/*=========================================================================*/
/**
 * 2 x 16 LCD driver
 * FE R&D group,2000-2014
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

#define RS	2	// PD.2
#define E	3       // PD.3

#define DSP_C	0
#define DSP_D	1

#define SCR_W	16

/*=========================================================================*/

void wt1ms(void);
void delay(uint16_t n);
void wrs(uint8_t adr,uint8_t d);
void display_init(void);
void wdispl(uint8_t b);
void putch(uint8_t c);
void puts1(const prog_char *s);
void clrscr(uint8_t m);
void locxy(uint8_t x,uint8_t y);
void putd3(uint16_t n,uint8_t f);
void putx(uint8_t n);

/*=========================================================================*/
