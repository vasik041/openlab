
#include "include/due_sam3x_init.h"
#include "uart.h"
#include "pwm.h"
#include "adc.h"

#define	WAVE_LENGTH	1024

uint16_t wave[WAVE_LENGTH];
uint32_t freq;
uint32_t pulse;

//==============================================================================

uint8_t d2h(uint8_t b)
{
    b &= 0x0f;
    if(b < 10) return '0'+b;
    return 'a'+b-10;
}

void thex(uint8_t b)
{
    uart_write(d2h(b >> 4));
    uart_write(d2h(b));
}

void thexw(uint16_t b)
{
    uart_write(d2h(b >> 12));
    uart_write(d2h(b >> 8));
    uart_write(d2h(b >> 4));
    uart_write(d2h(b));
}


uint8_t h2d(uint8_t h)
{
    if(h >= '0' && h <= '9') return h-'0';
    if(h >= 'A' && h <= 'F') return h-'A'+10;
    if(h >= 'a' && h <= 'f') return h-'a'+10;
    return 0;
}

uint8_t rhex(void)
{
uint8_t h;
uint8_t l;
    h = uart_recv(STD_TIMEOUT);    if(uart_timeout()) return 0;
    l = uart_recv(STD_TIMEOUT);    if(uart_timeout()) return 0;
    return (h2d(h) << 4) + h2d(l);
}


void send_ok(uint8_t b)
{
    uart_write('o');
    uart_write('k');
    thex(b);
    uart_write('\n');
}


void send_err(uint8_t b)
{
    uart_write('e');
    uart_write('r');
    thex(b);
    uart_write('\n');
}

//==============================================================================

uint8_t set_freq(void)
{
uint8_t h,l,u,v;
    l = rhex();	if(uart_timeout()) return 1;
    h = rhex();	if(uart_timeout()) return 2;
    u = rhex();	if(uart_timeout()) return 3;
    v = rhex();	if(uart_timeout()) return 4;

    freq = (uint32_t)l + ((uint32_t)h << 8) +
	 ((uint32_t)u << 16) + ((uint32_t)v << 24);

    pwm_set(freq,pulse);
    send_ok(l^h^u^v);
    return 0;
}


uint8_t set_pulse_length(void)
{
uint8_t h,l,u,v;
    l = rhex(); if(uart_timeout()) return 1;
    h = rhex();	if(uart_timeout()) return 2;
    u = rhex();	if(uart_timeout()) return 3;
    v = rhex();	if(uart_timeout()) return 4;

    pulse = (uint32_t)l + ((uint32_t)h << 8) +
	 ((uint32_t)u << 16) + ((uint32_t)v << 24);

    pwm_set(freq,pulse);
    send_ok(l^h^u^v);
    return 0;
}


uint8_t xor32(uint32_t x)
{
uint8_t a = x & 0xff;
uint8_t b = (x >> 8) & 0xff;
uint8_t c = (x >> 16) & 0xff;
uint8_t d = (x >> 24) & 0xff;
    return a ^ b ^ c ^ d;
}


void thex32(uint32_t x)
{
    thex(x >> 24);
    thex(x >> 16);
    thex(x >> 8);
    thex(x);
}


void get_settings(void)
{
    uart_write('z');
    thex32(freq);
    thex32(pulse);
    thex(xor32(freq) ^ xor32(pulse));
    uart_write('\n');
}


void get_sync_wave(uint8_t ch,uint16_t len)
{
uint16_t i;

    if(ch == 7) {
	adc_enable_anch(ADC);
	adc_set_channel_input_gain(ADC,ch,3);
    }
    __disable_irq();
    while((PIOC->PIO_PDSR & PIO_PC26) == 0);	//sync to driver pulse

    adc_enable_channel(ADC,ch);
    ADC->ADC_MR |= 0x80;
    ADC->ADC_CR = ADC_CR_START;
    
    PIOC->PIO_SODR = PIO_PC28;
    for(i=0; i < len; i++) {
        while((ADC->ADC_ISR & (1 << ch))==0); // wait for conversion
	wave[i] = ADC->ADC_CDR[ch];       // read data
    }
    PIOC->PIO_CODR = PIO_PC28;
    __enable_irq();

    ADC->ADC_MR &= ~0x80;
    adc_disable_channel(ADC,ch);
}


void send_wave(uint16_t off,uint16_t len)
{
uint16_t i;
uint16_t cs;
    cs = 0;
    for(i=off; i < off+len; i += 2) {
	thexw(wave[i]);
	thexw(wave[i+1]);
	cs ^= wave[i];
	cs ^= wave[i+1];
    }
    thexw(cs ^ 0x1234);
    uart_write('\n');
}


void get_trace()
{
uint8_t ch,l,h;
uint16_t len;
    ch = rhex(); if(uart_timeout()) return;
    l = rhex();	if(uart_timeout()) return;
    h = rhex();	if(uart_timeout()) return;
    len = ((uint16_t)h << 8) + (uint16_t)l;
    get_sync_wave(ch,len);
    send_wave(0,(len < 500) ? len : 500);
}


void get_wave()
{
uint8_t l,h;
uint16_t len,off;

    l = rhex();	if(uart_timeout()) return;
    h = rhex();	if(uart_timeout()) return;
    off = ((uint16_t)h << 8) + (uint16_t)l;
    l = rhex();	if(uart_timeout()) return;
    h = rhex();	if(uart_timeout()) return;
    len = ((uint16_t)h << 8) + (uint16_t)l;
    send_wave(off,len);
}


void led_x()
{
    if(PIOB->PIO_ODSR & PIO_PB27) {
        PIOB->PIO_CODR = PIO_PB27; /* Set clear register */
    } else {
        PIOB->PIO_SODR = PIO_PB27; /* Set set register */
    }
}
 
//==============================================================================

int main(void)
{
uint8_t c,r;
uint16_t cnt;
    init_controller(); 						// The general init (clock, libc, watchdog ...)
    PIO_Configure(PIOB, PIO_OUTPUT_1, PIO_PB27, PIO_DEFAULT);	// Board pin 13 == PB27 LED
    PIO_Configure(PIOC, PIO_OUTPUT_1, PIO_PC28, PIO_DEFAULT);	// pin 3

    pmc_enable_periph_clk(ID_PIOC);
    PIO_Configure(PIOC, PIO_INPUT, PIO_PC26, PIO_DEFAULT);	// pin 4

    uart_init(9600);
//    uart_puts("Hello world\r\n");

    pwm_init();
    pulse = 10;
    freq = 4*50000;
    pwm_set(freq,pulse);
    adc_init1();

    for(cnt=0; ; ) {
	c = uart_recv(STD_TIMEOUT);
	switch(c) {
	    case 'f':
		r = set_freq(); if(r) send_err(r);
		break;
	    case 't':
		r = set_pulse_length(); if(r) send_err(r);
		break;
	    case 's': get_settings();		break;
	    case 'q': get_trace();		break;
	    case 'w': get_wave();		break;
	    default:;
	}
	Sleep(1);
	if(++cnt > 10) {
	    led_x();
	    cnt = 0;
	}
    }

    return 0;
}

