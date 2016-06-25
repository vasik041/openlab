/****************************************************************************/
/*
 * 128x64 LCD driver
 * LZs,2000
 * This is free and unencumbered software released into the public domain.
 */
/****************************************************************************/

#define	LCD_RS	0
#define	LCD_RW	1
#define	LCD_E	2
#define	LCD_CS1	3
#define	LCD_CS2	4
#define	LCD_RST	7

#define	LCD_CTRL_PORT 	PORTB
#define	LCD_CTRL_DDR	DDRB
#define	LCD_CTRL_PIN	PINB

#define	LCD_CTRL_PORT2 	PORTD
#define	LCD_CTRL_DDR2	DDRD
#define	LCD_CTRL_PIN2	PIND

#define	LCD_DATA_PORT	PORTC
#define	LCD_DATA_DDR	DDRC
#define	LCD_DATA_PINS	PINC

#define	DEG	0xb0

void lcd_init (void);
void eStrobe (void);
void enable_cs1 (void);
void enable_cs2 (void);
void changeLine (unsigned char d);

void cls(void);
void putc_xy(int x,int y,int c);
void putc1(int c);
void locxy(int x,int y);
void putd3(int n,int f);
void putd2(int n);
void puts1(const prog_char *s);
void putx(int n);

void putc_xy2(int x,int y,int c);
void set_font(int f);
void plot(int x,int y,int c);
void clear(void);
void show(void);
void drawline(int x0,int y0,int x1,int y1,int c);
