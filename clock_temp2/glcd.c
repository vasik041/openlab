/****************************************************************************/
/*
 * 128x64 LCD driver
 * LZs,2000
 * This is free and unencumbered software released into the public domain.
 */
/****************************************************************************/

#include "main.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <compat/ina90.h>
#include <compat/deprecated.h>
#include <util/delay.h>

#include "glcd.h"
#include "font.h"
#include "font16x24.h"

int cx;
int cy;
int f;
unsigned char frame[128*64/8];

//=============================================================================

void lcd_init (void)
{		 
	LCD_DATA_PORT = 0;
	LCD_DATA_DDR = 0xff;

	LCD_CTRL_PORT = 0;
	LCD_CTRL_DDR = 0xff;

	LCD_CTRL_PORT2 = 0;
	LCD_CTRL_DDR2 = 0x80;

	LCD_CTRL_PORT2 |= (1 << LCD_RST);
	LCD_CTRL_PORT |= (1 << LCD_CS1);
        LCD_CTRL_PORT |= (1 << LCD_CS2);
        LCD_CTRL_PORT &= ~(1 << LCD_RS);
        LCD_CTRL_PORT &= ~(1 << LCD_RW);

	_delay_us(10000);
	LCD_DATA_PORT = 0b11000000;	 //Display start line = 0 (0-63)
        eStrobe();

	_delay_us(10000);
        LCD_DATA_PORT = 0b01000000;	 //Set address = 0 (0-63)
        eStrobe();

	_delay_us(10000);
        LCD_DATA_PORT = 0b10111000;	 //Set page = 0 (0-7)
        eStrobe();

	_delay_us(10000);
        LCD_DATA_PORT = 0b00111111;	 //Display ON 
        eStrobe();

        LCD_CTRL_PORT &= ~(1 << LCD_CS1); //Disable the CS1 of display 
        LCD_CTRL_PORT &= ~(1 << LCD_CS2); //Disable the CS2 of display

	cx = 0;
	cy = 0;
	f = 0;
}

//====================================================================

void changeLine (unsigned char y)
{
	LCD_CTRL_PORT |= (1 << LCD_CS1);
        LCD_CTRL_PORT |= (1 << LCD_CS2);
        LCD_CTRL_PORT &= ~(1 << LCD_RS);
	LCD_CTRL_PORT &= ~(1 << LCD_RW);

        LCD_DATA_PORT = 0b10111000 + y;	//Set page = 0 (0-7)
        eStrobe();

        LCD_DATA_PORT = 0b01000000;	//Set address = 0 (0-63)
        eStrobe();
	LCD_CTRL_PORT |= (1 << LCD_RS);	//Set RS. Data mode
}

//====================================================================

void show(void)
{
unsigned char column = 0;
unsigned char line = 0;
unsigned int  address = 0;

	changeLine(0);
	for(address = 0; address < 1024; address++) {
		if(column == 128) {
			column = 0;
			line++;
			line &= 7;
			changeLine(line);
		} 

		if(column < 64) enable_cs1(); else enable_cs2();

		_delay_us(10);
		LCD_CTRL_PORT |= (1 << LCD_RS);	// "DATA SEND" mode
		LCD_CTRL_PORT &= ~(1 << LCD_RW);	
		LCD_DATA_PORT = frame[address];
		eStrobe();
		column++;		// increase column (maximum 128)
	}
	cx = 0;
	cy = 0;
	f = 0;
}

//====================================================================

void clear()
{
int i;
	for(i=0; i < 128*64/8; i++)
		frame[i] = 0;
}

//====================================================================

void cls(void)
{
	clear();
	show();
}

//====================================================================

void enable_cs1 (void)
{
	LCD_CTRL_PORT |= (1 << LCD_CS1);
	LCD_CTRL_PORT &= ~(1 << LCD_CS2);
}

//====================================================================

void enable_cs2 (void)
{
	LCD_CTRL_PORT |= (1 << LCD_CS2);
	LCD_CTRL_PORT &= ~(1 << LCD_CS1);
}

//====================================================================

void eStrobe (void)
{
        LCD_CTRL_PORT |= (1 << LCD_E);
	_delay_us(10);
        LCD_CTRL_PORT &= ~(1 << LCD_E);
	_delay_us(10);
}

//====================================================================

void draw8(int x,int y,int b)
{
	LCD_CTRL_PORT |= (1 << LCD_CS1);
        LCD_CTRL_PORT |= (1 << LCD_CS2);
        LCD_CTRL_PORT &= ~(1 << LCD_RS);
	LCD_CTRL_PORT &= ~(1 << LCD_RW);

        LCD_DATA_PORT = 0b10111000+y;	//Set page = 0 (0-7)
        eStrobe();

        LCD_DATA_PORT = 0b01000000+(x & 0x3f); //Set address = 0 (0-63)
        eStrobe();

	if(x < 64) enable_cs1(); else enable_cs2();

	_delay_us(10);
	LCD_CTRL_PORT |= (1 << LCD_RS);	// "DATA SEND" mode
	LCD_CTRL_PORT &= ~(1 << LCD_RW);	
	LCD_DATA_PORT = b;
	eStrobe();
}

//=============================================================================

void putc_xy2(int x,int y,int c)
{
int i;
	if(c >= '0' && c <= '9') c -= '0';
	else if(c == ':') c = 10;
	else if(c == '.') c = 11;
	else if(c == '-') c = 12;
	else if(c == DEG) c = 13;
	else return;

	for(i=0; i < 16; i++) {
		draw8(x*8+i,  y,pgm_read_byte(&font2[c*48+i]));
		draw8(x*8+i,y+1,pgm_read_byte(&font2[c*48+i+16]));
		draw8(x*8+i,y+2,pgm_read_byte(&font2[c*48+i+32]));
	}
}

//=============================================================================

void putc_xy(int x,int y,int c)
{
int i;
	for(i=0; i < 8; i++) {
		draw8(x*8+i,y,pgm_read_byte(&font[c*8+i]));
	}
}

//=============================================================================

void set_font(int b)
{
	f = b;
}

//=============================================================================

void putc1(int c)
{
	if(f == 1) {
		putc_xy2(cx,cy,c);
		if(c >= '0' && c <= '9') cx++;
		cx++;
	} else {
		putc_xy(cx,cy,c);
		cx++;
	}
	if(cx >= 16) {
		cx = 0;
		cy++;
	}
}

//=============================================================================

void locxy(int x,int y)
{
	cx = x;
	cy = y;
}

//=============================================================================

void putd2(int n)
{
	if(n > 99) n = 99;
	putc1('0'+n/10);
	putc1('0'+n%10);
}

//=============================================================================

void putd3(int n,int f)
{
int m,i,p;

	if(n > 999) n = 999;
	m = 100;
	p = f;
	for(i=0; i < 3; i++) {
		if(m == 1) p = 1;
		if(n / m != 0 || p) {
			putc1(n / m + '0');
			p = 1;
		}
		n %= m;
		m /= 10;
	}
}

//=============================================================================

void puts1(const prog_char *s)
{
char c;
	for( ; ; s++) {
		c = pgm_read_byte(s);
		if(c == '\0') break;
		putc1(c);
	}
}

//=============================================================================

void px(int n)
{
	n &= 0x0f;
	if(n < 10) putc1('0'+n); else putc1('a'+n-10);
}

void putx(int n)
{
	px(n >> 4);
	px(n);
}

//=============================================================================
// x < 128, y < 64

void plot(int x,int y,int c)
{
int msk;
int off;
	x &= 0x7f;
	y &= 0x3f;
	msk = 1 << (y & 7);
	off = (y >> 3) & 7;
	if(c) {
		frame[x + 128*off] |= msk;
	} else {
		frame[x + 128*off] &= ~msk;
	}
}

//=============================================================================

#define swap(X,Y)	tmp = X; X = Y; Y = tmp
#define abs(X)		((X >= 0) ? X : (-X))

 
void drawline(int x0,int y0,int x1,int y1,int c)
{
int deltax;
int deltay;
int error;
int ystep;
int x,y;
int tmp;
int steep = (abs(y1 - y0) > abs(x1 - x0)) ? 1 : 0;

	if(steep) {
        	swap(x0, y0);
	        swap(x1, y1);
	}
	
	if(x0 > x1) {
        	swap(x0, x1);
		swap(y0, y1);
	}

	deltax = x1 - x0;
	deltay = abs(y1 - y0);
	error = deltax / 2;
	y = y0;
	if(y0 < y1) ystep = 1; else ystep = -1;
    
	for(x=x0; x <= x1; x++) {
		if(steep) plot(y, x, c); else plot(x, y, c);
		error -= deltay;
		if (error < 0) {
			y += ystep;
			error += deltax;
		}
	}
}
