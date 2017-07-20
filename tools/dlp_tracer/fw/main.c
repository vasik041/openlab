
#define NO_BIT_DEFINES
#include <pic14regs.h>
#include <stdint.h> 


#define LED_PORT PORTEbits.RE0
#define LED_TRIS TRISEbits.TRISE0

#define	RD	PORTBbits.RB1
#define	WR	PORTBbits.RB2
#define	RXF	PORTBbits.RB4

#define	TXE	PORTEbits.RE2
#define	SIWUB	PORTEbits.RE1


void wt10us(void)
{
uint16_t i;
	for(i=0; i < 4; i++) {
		__asm clrwdt __endasm;
		__asm nop __endasm;
		__asm nop __endasm;
		__asm nop __endasm;
		__asm nop __endasm;
	}
}


void wt1ms(void)
{
uint16_t i;
	for(i=0; i < 200; i++) {
		__asm clrwdt __endasm;
		__asm nop __endasm;
		__asm nop __endasm;
		__asm nop __endasm;
		__asm nop __endasm;
	}
}

void delay(uint16_t n)
{
uint16_t i;
    for(i=0; i < n; i++)
	wt1ms();
}


//==============================================================================

void init_pwm(void)
{
uint16_t duty = 512;
    //Set the PWM period by writing to the PR2 register.
    PR2 = 0xff;

    //Set the PWM duty cycle by writing to the CCPR1L register and CCP1CON<5:4> bits.
    CCPR1L = duty >> 2;
    CCP1CON = ((duty & 3) << 4) | 0x0c;

    //Make the CCP1 pin an output by clearing the TRISC<2> bit.
    TRISCbits.TRISC2 = 0;

    //Set the TMR2 prescale value and enable Timer2 by writing to T2CON.
    T2CON = 4;

    //Configure the CCP1 module for PWM operation.
    //CCP1CON = 0x0c;
}


void set_pwm(uint16_t duty)
{
    //Set the PWM duty cycle by writing to the CCPR1L register and CCP1CON<5:4> bits.
    CCPR1L = duty >> 2;
    CCP1CON = ((duty & 3) << 4) | 0x0c;
}

//==============================================================================

void putchr(uint8_t b)
{
    while(TXE);
    TRISD = 0;
    PORTD = b;
    WR = 1;
    wt10us();
    WR = 0;	
    TRISD = 0xff;
}


uint8_t kbhit()
{
    if(!RXF) return 1;
    return 0;
}


uint8_t getchr()
{
uint8_t d;
    while(RXF);

    RD = 0;
    wt10us();
    d = PORTD;
    RD = 1;	
    return d;
}


uint8_t rcvw(void)
{
uint16_t w;
    for(w=0; w < 5000; w++) {
	if(kbhit()) return getchr();
	wt1ms();
    }
    return 0;
}

uint8_t a2x(uint8_t c)
{
    if(c >= '0' && c <= '9') return c-'0';
    if(c >= 'a' && c <= 'f') return c-'a'+10;
    if(c >= 'A' && c <= 'F') return c-'A'+10;
    return 0;
}


uint8_t rcvwx(void)
{
uint8_t c,c2;
    c = rcvw();
    c2 = rcvw();
    return (a2x(c) << 4) + a2x(c2);
}


void init_rxtx(void)
{
    TRISE = 4;	// e0,1 out
    TRISD = 0xff;
    TRISB = 0xf8;
    SIWUB = 1;
    RD = 1;
}

//==============================================================================

void px(uint8_t n)
{
    n &= 0x0f;
    if(n < 10) putchr(n+'0'); else putchr(n-10+'a');
}

void putx(uint8_t x)
{
    px(x >> 4);
    px(x);
}

void putxw(uint16_t x)
{
    px(x >> 12);
    px(x >> 8);
    px(x >> 4);
    px(x);
}


void nl(void)
{
    putchr('\r');
    putchr('\n');
}


uint8_t p;

void putchr0p(uint8_t c)
{
    if(c == '0' && p == 0) {
	putchr(' ');
    } else {
	putchr(c);
	p = 1;
    }
}


void putd(uint16_t n)
{
    p = 0;
    putchr0p('0'+n/1000); 	n %= 1000;
    putchr0p('0'+n/100);	n %= 100;
    putchr0p('0'+n/10);		n %= 10;
    putchr('0'+n);
}

//==============================================================================

uint16_t adc_read(uint8_t ch)
{
uint8_t b;
    ADCON0 = (ch << 3) + 0x81;		// enable ADC, fosc/32
    wt10us();
    wt10us();
    ADCON0bits.GO = 1;
    while(ADCON0bits.NOT_DONE);
    b = ADRESL;
    return (ADRESH << 8) + b;
}

void init_adc(void)
{
    TRISA = 3;
    ADCON1 = 0x80;	// normal format of ADRESH
}

//==============================================================================

void measure(uint8_t step)
{
uint16_t j;
    putchr('@');
    for(j=12; j < 1024; j+=step) {
	set_pwm(j);
	delay(100);

	putd(adc_read(0));
	putchr(' ');
	putd(adc_read(1));
	nl();
    }
    putchr('-');
    putchr('-');
    putchr('-');
    nl();
    set_pwm(512);
}


void set_dac(void)
{
uint8_t h = rcvwx();
uint8_t l = rcvwx();
uint16_t v = (h << 8)+l;
    putchr('s');
    putxw(v);
    nl();
    set_pwm(v);
}


void trea(void)
{
uint16_t i;
    for(;;) {
	for(i=0; i < 1024; i++) {
	    set_pwm(i);
	    wt10us();
	    wt10us();
	    wt10us();
	}
	for(i=0; i < 1024; i++) {
	    set_pwm(1023-i);
	    wt10us();
	    wt10us();
	    wt10us();
	}
	if(kbhit()) break;
    }
}


void measure1(void)
{
uint16_t j;

    putchr('@');
    for(j=512; j >= 512-50; j--) {
	set_pwm(j);
	delay(100);

	putd(adc_read(0));
	putchr(' ');
	putd(adc_read(1));
	nl();
    }
    putchr('-');
    putchr('-');
    putchr('-');
    nl();

    putchr('@');
    for(j=512-50; j < 512; j++) {
	set_pwm(j);
	delay(100);

	putd(adc_read(0));
	putchr(' ');
	putd(adc_read(1));
	nl();
    }
    putchr('-');
    putchr('-');
    putchr('-');
    nl();

    set_pwm(512);
}

//==============================================================================

void main(void)
{
uint16_t cnt = 0;
uint8_t sw = 0;
//	LED_TRIS = 0; // Pin as output
	LED_PORT = 0; // LED off

	TRISC = 0;

	init_pwm();
	init_rxtx();
	init_adc();
	for(;;) {
	    wt1ms();
	    if(kbhit()) {
		uint8_t c = getchr();
		switch(c) {
		    case 'm':
			measure(50);
			break;

		    case 'n':
			measure(5);
			break;

		    case 'b':
			measure1();
			break;


		    case 'r':
			putd(adc_read(0));
			putchr(' ');
			putd(adc_read(1));
			nl();
			break;

		    case 's':
			set_dac();
			break;

		    case 't':
			trea();
			break;

		    default:;
		}
	    }
	    if(cnt++ < 1000) continue;
	    cnt = 0;
	    if(sw) { sw=0; LED_PORT=0; } else { sw=1; LED_PORT=1; }
	}
}
