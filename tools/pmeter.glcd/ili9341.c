
#include "main.h"
#include "ili9341.h"
#include "glcdfont.h"

volatile int16_t _width;
volatile int16_t _height;



void spiWrite(uint8_t c)
{
uint8_t msk;
    for(msk=0x80; msk; msk >>= 1) {
	if(c & msk) {
      		SET_BIT(mosiport, mosipinmask);
	} else {
      		CLEAR_BIT(mosiport, mosipinmask);
	}
      	SET_BIT(clkport, clkpinmask);
      	CLEAR_BIT(clkport, clkpinmask);
    }
}


void writeCommand(uint8_t c)
{
	CLEAR_BIT(dcport, dcpinmask);
	CLEAR_BIT(csport, cspinmask);
	spiWrite(c);
	SET_BIT(csport, cspinmask);
}


void writeData(uint8_t c)
{
	SET_BIT(dcport,  dcpinmask);
	CLEAR_BIT(csport, cspinmask);
	spiWrite(c);
	SET_BIT(csport, cspinmask);
} 



void initLcd(void)
{
	DDRA |= rstpinmask; // OUTPUT
	CLEAR_BIT(rstport,rstpinmask);

	DDRA |= dcpinmask;
	DDRA |= cspinmask;
	DDRB |= clkpinmask;
	DDRA |= mosipinmask;

// 	DDRA &= ~misopinmask; // INPUT
// 	PORTA |= misopinmask;

	CLEAR_BIT(clkport, clkpinmask);
	CLEAR_BIT(mosiport, mosipinmask);

	// toggle RST low to reset
	SET_BIT(rstport,rstpinmask);
	_delay_ms(5);
	CLEAR_BIT(rstport,rstpinmask);
	_delay_ms(20);
	SET_BIT(rstport,rstpinmask);
	_delay_ms(150);

	_width  = ILI9341_TFTWIDTH;
	_height = ILI9341_TFTHEIGHT;

	writeCommand(0xEF);
	writeData(0x03);
	writeData(0x80);
	writeData(0x02);

	writeCommand(0xCF);  
	writeData(0x00); 
	writeData(0XC1); 
	writeData(0X30); 

	writeCommand(0xED);  
	writeData(0x64); 
	writeData(0x03); 
	writeData(0X12); 
	writeData(0X81); 

	writeCommand(0xE8);  
	writeData(0x85); 
	writeData(0x00); 
	writeData(0x78); 

	writeCommand(0xCB);  
	writeData(0x39); 
	writeData(0x2C); 
	writeData(0x00); 
	writeData(0x34); 
	writeData(0x02); 

	writeCommand(0xF7);  
	writeData(0x20); 

	writeCommand(0xEA);  
	writeData(0x00); 
	writeData(0x00); 
 
	writeCommand(ILI9341_PWCTR1);    //Power control 
	writeData(0x23);   //VRH[5:0] 
 
	writeCommand(ILI9341_PWCTR2);    //Power control 
	writeData(0x10);   //SAP[2:0];BT[3:0] 
 
	writeCommand(ILI9341_VMCTR1);    //VCM control 
	writeData(0x3e);
	writeData(0x28); 
  
	writeCommand(ILI9341_VMCTR2);    //VCM control2 
	writeData(0x86);  //--
 
	writeCommand(ILI9341_MADCTL);    // Memory Access Control 
	writeData(ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR);

	writeCommand(ILI9341_PIXFMT);    
	writeData(0x55); 
  
	writeCommand(ILI9341_FRMCTR1);    
	writeData(0x00);  
	writeData(0x18); 
 
	writeCommand(ILI9341_DFUNCTR);    // Display Function Control 
	writeData(0x08); 
	writeData(0x82);
	writeData(0x27);  
 
	writeCommand(0xF2);    // 3Gamma Function Disable 
	writeData(0x00); 
 
	writeCommand(ILI9341_GAMMASET);    //Gamma curve selected 
	writeData(0x01); 
 
	writeCommand(ILI9341_GMCTRP1);    //Set Gamma 
	writeData(0x0F); 
	writeData(0x31); 
	writeData(0x2B); 
	writeData(0x0C); 
	writeData(0x0E); 
	writeData(0x08); 
	writeData(0x4E); 
	writeData(0xF1); 
	writeData(0x37); 
	writeData(0x07); 
	writeData(0x10); 
	writeData(0x03); 
	writeData(0x0E); 
	writeData(0x09); 
	writeData(0x00); 
  
	writeCommand(ILI9341_GMCTRN1);    //Set Gamma 
	writeData(0x00); 
	writeData(0x0E); 
	writeData(0x14); 
	writeData(0x03); 
	writeData(0x11); 
	writeData(0x07); 
	writeData(0x31); 
	writeData(0xC1); 
	writeData(0x48); 
	writeData(0x08); 
	writeData(0x0F); 
	writeData(0x0C); 
	writeData(0x31); 
	writeData(0x36); 
	writeData(0x0F); 

	writeCommand(ILI9341_SLPOUT);    //Exit Sleep 
	_delay_ms(120); 		
	writeCommand(ILI9341_DISPON);    //Display on 
}



void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	writeCommand(ILI9341_CASET); // Column addr set
	writeData(x0 >> 8);
	writeData(x0 & 0xFF);     // XSTART 
	writeData(x1 >> 8);
	writeData(x1 & 0xFF);     // XEND

	writeCommand(ILI9341_PASET); // Row addr set
	writeData(y0>>8);
	writeData(y0);     // YSTART
	writeData(y1>>8);
	writeData(y1);     // YEND

	writeCommand(ILI9341_RAMWR); // write to RAM
}



void drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if(x < 0 || x >= _width || y < 0 || y >= _height) return;

	setAddrWindow(x,y,x+1,y+1);
	SET_BIT(dcport, dcpinmask);
	CLEAR_BIT(csport, cspinmask);
	spiWrite(color >> 8);
	spiWrite(color);
	SET_BIT(csport, cspinmask);
}



void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
uint8_t hi = color >> 8;
uint8_t lo = color;
	// Rudimentary clipping
	if((x >= _width) || (y >= _height)) return;
	if((y+h-1) >= _height) h = _height-y;
	setAddrWindow(x, y, x, y+h-1);

	SET_BIT(dcport, dcpinmask);
	CLEAR_BIT(csport, cspinmask);

	while (h--) {
		spiWrite(hi);
		spiWrite(lo);
	}
	SET_BIT(csport, cspinmask);
}


void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
uint8_t hi = color >> 8;
uint8_t lo = color;
	// Rudimentary clipping
	if((x >= _width) || (y >= _height)) return;
	if((x+w-1) >= _width)  w = _width-x;
	setAddrWindow(x, y, x+w-1, y);

	SET_BIT(dcport, dcpinmask);
	CLEAR_BIT(csport, cspinmask);
	while (w--) {
		spiWrite(hi);
		spiWrite(lo);
	}
	SET_BIT(csport, cspinmask);
}



void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
uint8_t hi = color >> 8;
uint8_t lo = color;

	// rudimentary clipping (drawChar w/big text requires this)
	if((x >= _width) || (y >= _height)) return;
	if((x + w - 1) >= _width)  w = _width  - x;
	if((y + h - 1) >= _height) h = _height - y;

	setAddrWindow(x, y, x+w-1, y+h-1);

	SET_BIT(dcport, dcpinmask);
	CLEAR_BIT(csport, cspinmask);

	for(y=h; y > 0; y--) {
	    for(x=w; x > 0; x--) {
		spiWrite(hi);
	      	spiWrite(lo);
	    }
	}
	SET_BIT(csport, cspinmask);
}


void fillScreen(uint16_t color)
{
	fillRect(0, 0,  _width, _height, color);
}


void setRotation(uint8_t m)
{
	writeCommand(ILI9341_MADCTL);
	switch (m & 3) {
		case 0:
			writeData(ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR);
			_width  = ILI9341_TFTWIDTH;
			_height = ILI9341_TFTHEIGHT;
			break;
		case 1:
			writeData(ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
			_width  = ILI9341_TFTHEIGHT;
			_height = ILI9341_TFTWIDTH;
			break;             
		case 2:
			writeData(ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
			_width  = ILI9341_TFTWIDTH;
			_height = ILI9341_TFTHEIGHT;
			break;
		case 3:
			writeData(ILI9341_MADCTL_MV | ILI9341_MADCTL_MY | ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR);
			_width  = ILI9341_TFTHEIGHT;
			_height = ILI9341_TFTWIDTH;
			break;
	}
}

void drawChar(int16_t x, int16_t y, unsigned char c,
			    uint16_t color, uint16_t bg, uint8_t size)
{
int8_t i,j;

  if((x >= _width)            || // Clip right
     (y >= _height)           || // Clip bottom
     ((x + 6 * size - 1) < 0) || // Clip left
     ((y + 8 * size - 1) < 0))   // Clip top
    return;

//  if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

  for (i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5) 
      line = 0x0;
    else 
      line = pgm_read_byte(font+(c*5)+i);
    for (j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, color);
        else {  // big size
          fillRect(x+(i*size), y+(j*size), size, size, color);
        } 
      } else if (bg != color) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, bg);
        else {  // big size
          fillRect(x+i*size, y+j*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}

#define swap(a, b) { int16_t t = a; a = b; b = t; }
#define abs(a)  ((a) > 0 ? (a) : -(a))


// Bresenham's algorithm - thx wikpedia
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  int16_t dx, dy;
  int16_t err;
  int16_t ystep;
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  dx = x1 - x0;
  dy = abs(y1 - y0);

  err = dx / 2;


  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

//=============================================================================

volatile int16_t x;
volatile int16_t y;
volatile uint16_t color;
volatile uint16_t bgcolor;
volatile uint8_t invert;
volatile uint8_t size;


void locXY(int16_t x1, int16_t y1)
{
	x = x1;
	y = y1;
}


void setColor(uint16_t c,uint16_t bg,uint8_t sz)
{
	color = c;
	bgcolor = bg;
	size = sz;
	invert = 0;
}

void inv(uint8_t f)
{
	invert = f;
}

void putch(uint8_t c)
{
	drawChar(x,y,c,
			invert ? bgcolor : color,
			invert ? color : bgcolor,
			size);
	x += size*6;
}


void puts1(const prog_char *s)
{
char c;
	for( ; ; s++) {
		c = pgm_read_byte(s);
		if(c == '\0') break;
		putch(c);
	}
}


void putd(int16_t n,uint8_t digs,uint8_t f)
{
uint8_t i,p;
uint16_t m;

	if(n < 0) {
		putch('-');
		n = -n;
	} else {
		putch(' ');
	}
	switch(digs) {
		case 2: m = 10;     break;
		case 3: m = 100;    break;
		case 4: m = 1000;   break;
		case 5:
		default: m = 10000;  break; 
	}

	p = f;
	for(i=0; i < digs; i++) {
		if(m == 1) p = 1;
		if(n / m != 0 || p) {
			putch(n / m + '0');
			p = 1;
		} else {
			putch(' ');
		}
		n %= m;
		m /= 10;
	}
}
