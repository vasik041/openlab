/*=========================================================================*/
/**
 * 1 x 16 LCD driver
 * LZs,2000
 */
/*=========================================================================*/

#define RS	5	// PC.5
#define E	4       // PC.4

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
void locx(uint8_t a);
void putd(uint32_t n,uint8_t digs,uint8_t f);
void putx(uint8_t n);

/*=========================================================================*/
