
#include "include/due_sam3x_init.h"
#include "uart.h"
#include "pwm.h"
#include "adc.h"

#define	WAVE_LENGTH	1024

uint16_t wave[WAVE_LENGTH];
uint32_t freq;
uint32_t pulse;

uint8_t buf[32];
uint8_t pos;
uint8_t ready;

//==============================================================================

extern Uart *_pUart;


void poll()
{
uint8_t c;
    if(ready) return;
    if(!uart_available()) return;
    c = uart_read();
    if(c == '\n') {
        ready = 1;
        return;
    }
    if(pos < sizeof(buf)-1) {
	buf[pos++] = c;
    }
}


void tx(uint8_t d)
{
    while ((_pUart->UART_SR & UART_SR_TXEMPTY) != UART_SR_TXEMPTY)
	poll();
    _pUart->UART_THR = d;
}

//==============================================================================
//==============================================================================

uint8_t h2d(uint8_t h)
{
    if(h >= '0' && h <= '9') return h-'0';
    if(h >= 'A' && h <= 'F') return h-'A'+10;
    if(h >= 'a' && h <= 'f') return h-'a'+10;
    return 0;
}


uint8_t rhex(void)
{
uint8_t h,l;
    h = buf[pos++];
    l = buf[pos++];
    return (h2d(h) << 4) + h2d(l);
}

//==============================================================================

void set_freq(void)
{
uint8_t h,l,u,v,cs;
    l = rhex();
    h = rhex();
    u = rhex();
    v = rhex();
    cs = rhex();
    if((l ^ h ^ u ^ v) != cs) return;
    freq = (uint32_t)l + ((uint32_t)h << 8) + ((uint32_t)u << 16) + ((uint32_t)v << 24);
    pwm_set(freq,pulse);
}


void set_pulse_length(void)
{
uint8_t h,l,u,v,cs;
    l = rhex();
    h = rhex();
    u = rhex();
    v = rhex();
    cs = rhex();
    if((l ^ h ^ u ^ v) != cs) return;
    pulse = (uint32_t)l + ((uint32_t)h << 8) + ((uint32_t)u << 16) + ((uint32_t)v << 24);
    pwm_set(freq,pulse);
}


//==============================================================================
//==============================================================================

uint8_t d2h(uint8_t b)
{
    b &= 0x0f;
    if(b < 10) return '0'+b;
    return 'a'+b-10;
}

void thex(uint8_t b)
{
    tx(d2h(b >> 4));
    tx(d2h(b));
}

void thexw(uint16_t b)
{
    tx(d2h(b >> 12));
    tx(d2h(b >> 8));
    tx(d2h(b >> 4));
    tx(d2h(b));
}

//==============================================================================

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

//==============================================================================

void send_wave1(uint16_t off,uint16_t len)
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
    tx('\n');
}


void send_wave(void)
{
    get_sync_wave(7,1024);
    tx('i');
    send_wave1(0,1024);

    get_sync_wave(6,1024);
    tx('u');
    send_wave1(0,1024);
}

//==============================================================================

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


void send_settings(void)
{
    tx('z');
    thex32(freq);
    thex32(pulse);
    thex(xor32(freq) ^ xor32(pulse));
    tx('\n');
}

//==============================================================================
//==============================================================================

void process(void)
{
    if(!ready) return;

//    tx('x');    
//    for(i=0; i < pos; i++) {
//	tx(buf[i]);    
//    }
//    tx('\n');

    pos = 0;
    switch(buf[pos++]) {
	case 'f': set_freq();		break;
        case 't': set_pulse_length();	break;
	default:;
    }
    pos = 0;
    ready = 0;
}


void led_x(void)
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
uint16_t cnt1,cnt10;

    init_controller(); 						// The general init (clock, libc, watchdog ...)
    PIO_Configure(PIOB, PIO_OUTPUT_1, PIO_PB27, PIO_DEFAULT);	// Board pin 13 == PB27 LED
    PIO_Configure(PIOC, PIO_OUTPUT_1, PIO_PC28, PIO_DEFAULT);	// pin 3

    pmc_enable_periph_clk(ID_PIOC);
    PIO_Configure(PIOC, PIO_INPUT, PIO_PC26, PIO_DEFAULT);	// pin 4

    uart_init(38400);
    uart_puts("xHello world\r\n");

    pwm_init();
    pulse = 10;
    freq = 4*50000;
    pwm_set(freq,pulse);
    adc_init1();

    ready = 0;
    pos = 0;
    for(cnt1=cnt10=0; ; ) {
	poll();
	process();
	if(++cnt1 > 1000) {
	    send_wave();
	    cnt1 = 0;
	    led_x();
	}
	if(++cnt10 > 10000) {
	    send_settings();
	    cnt10 = 0;
	}
	Sleep(1);
    }
    return 0;
}

