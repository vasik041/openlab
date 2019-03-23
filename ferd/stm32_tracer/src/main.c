
#include <stdio.h>
#include <stdlib.h>

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_spi.h"

#include "main.h"
#include "ili9341.h"
#include "timer.h"
#include "uart.h"
#include "adc.h"
#include "keys.h"
#include "i2c.h"

static volatile uint32_t sysTickCount = 0;
static volatile uint32_t sysTickCount2 = 0;

void delay_ms(uint32_t nTime)
{
	sysTickCount = nTime+1;
	while(sysTickCount != 0);
}

void SysTick_Handler()
{
	if (sysTickCount != 0) {
		sysTickCount--;
	}
	sysTickCount2++;
}


static const uint16_t colors[] = {
    ILI9341_BLACK, ILI9341_NAVY, ILI9341_DARKGREEN,ILI9341_DARKCYAN,
    ILI9341_MAROON,	ILI9341_PURPLE,	ILI9341_OLIVE, ILI9341_LIGHTGREY,
    ILI9341_DARKGREY, ILI9341_BLUE,	ILI9341_GREEN, ILI9341_CYAN,
    ILI9341_RED, ILI9341_MAGENTA, ILI9341_YELLOW, ILI9341_WHITE,
    ILI9341_ORANGE, ILI9341_GREENYELLOW, ILI9341_PINK
};


static volatile uint16_t tau1 = 10;
static volatile uint16_t tau2 = 10;
static volatile uint16_t tau3 = 10;
static volatile uint16_t tau4 = 10;
static volatile uint16_t period = 1000;
static volatile uint16_t step = 1;
static volatile uint16_t timebase = 0;

static volatile uint16_t aval[1000];

static const char *tbs[] = {
    "125ns",
    "250ns",
    "500ns",
    " 1us "
};

static const uint16_t tbv[] = {
    8,
    16,
    32,
    64
};


void params_load()
{
    tau1 = i2c_read16(0);
    tau2 = i2c_read16(2);
    tau3 = i2c_read16(4);
    tau4 = i2c_read16(6);

    period = i2c_read16(8);
    if(period < 100 || period > 50000) period = 1000;
    step = i2c_read16(10);
    if(step != 1 && step != 10 && step != 100 && step != 1000) step = 1;
    timebase = i2c_read16(12);
    if(timebase > 3) timebase = 0;

    if(tau1 < 1) tau1 = 1;
    if(tau1 >= period) tau1 = 1;
    
    if(tau2 < 1) tau2 = 1;
    if(tau2 >= period) tau2 = 1;

    if(tau3 < 1) tau3 = 1;
    if(tau3 >= period) tau3 = 1;

    if(tau4 < 1) tau4 = 1;
    if(tau4 >= period) tau4 = 1;
}


void params_save()
{
    i2c_write16(0, tau1);
    i2c_write16(2, tau2);
    i2c_write16(4, tau3);
    i2c_write16(6, tau4);

    i2c_write16(8, period);
    i2c_write16(10, step);
    i2c_write16(12, timebase);
}


int dec(int pos)
{
    switch(pos) {
	case 0:	if(tau1 > step) { tau1 -= step; set_ch1(tau1); return 1; } break;
	case 1: if(tau2 > step) { tau2 -= step; set_ch2(tau2); return 1; } break;
	case 2: if(tau3 > step) { tau3 -= step; set_ch3(tau3); return 1; } break;
	case 3: if(tau4 > step) { tau4 -= step; set_ch4(tau4); return 1; } break;

	case 4: if(period > step) {
		    period -= step;
		    set_period(tbv[timebase],period);
		    return 1;
		}
		break;

	case 5: if(step > 1) {
		    step /= 10;
		    return 1;
		}
		break;

	case 6: if(timebase) {
		    timebase--;
		    set_period(tbv[timebase], period);
		    return 1;
		}
		break;

	default:;
    }
    return 0;
}

int inc(int pos)
{
    switch(pos) {
	case 0:	if(tau1 < period-step*2) { tau1 += step; set_ch1(tau1); return 1; } break;
	case 1: if(tau2 < period-step*2) { tau2 += step; set_ch2(tau2); return 1; } break;
	case 2: if(tau3 < period-step*2) { tau3 += step; set_ch3(tau3); return 1; } break;
	case 3: if(tau4 < period-step*2) { tau4 += step; set_ch4(tau4); return 1; } break;

	case 4: if(period < 50000-step) {
		    period += step;
		    set_period(tbv[timebase], period);
		    return 1;
		}
		break;

	case 5:	if(step < 1000) {
		    step *= 10;
		    return 1;
		}
		break;

	case 6: if(timebase < NELEM(tbv)-1) {
		    timebase++;
		    set_period(tbv[timebase], period);
		    return 1;
		}
		break;

	default:;
    }
    return 0;
}

void invif(int pos, int row, uint8_t sw)
{
    if(pos == row) inv(sw);
}


void draw(int pos)
{
    locXY(8,8);    puts1("ch1       "); invif(pos,0,1); putd(tau1,4,0); invif(pos,0,0);
    locXY(8,8+16); puts1("ch2       "); invif(pos,1,1); putd(tau2,4,0); invif(pos,1,0);
    locXY(8,8+32); puts1("ch3       "); invif(pos,2,1); putd(tau3,4,0); invif(pos,2,0);
    locXY(8,8+48); puts1("ch4       "); invif(pos,3,1); putd(tau4,4,0); invif(pos,3,0);

    locXY(8,8+64); puts1("period   "); invif(pos,4,1); putd(period,5,0); invif(pos,4,0);
    locXY(8,8+80); puts1("step      "); invif(pos,5,1); putd(step,4,0); invif(pos,5,0);
    locXY(8,8+96); puts1("time base "); invif(pos,6,1); puts1(tbs[timebase]); invif(pos,6,0);
}

/*
TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
NVIC_EnableIRQ(TIM3_IRQn);

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)  {
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
    } 
}
*/

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // LED is on PC13
    // enable clock for peripheral GPIOA, GPIOB and GPIOC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    // initialize GPIO structure
    GPIO_InitTypeDef gpioInitStruct;
    GPIO_StructInit(&gpioInitStruct);
    gpioInitStruct.GPIO_Pin = GPIO_Pin_13;
    gpioInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &gpioInitStruct);

    // set up a timer
    SysTick_Config(SystemCoreClock/1000);

    uart_init();
    uart_puts("Hello ARM world\r\n");

    timer3_init();
    TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);

    adc_init();
    i2c_init();

    spi_init();
    initLcd();
    fillScreen(ILI9341_BLACK);
    setRotation(1);
    setColor(ILI9341_WHITE, ILI9341_BLACK, 2);

    int sw = 0, cnt = 0;
    int c = 0, oc = 0;

    params_load();
    set_period(tbv[timebase], period);
    set_ch1(tau1);
    set_ch2(tau2);
    set_ch3(tau3);
    set_ch4(tau4);

    int pos = 0;
    draw(pos);
    int timer10 = 0;    
    for( ; ; ) {
	    delay_ms(10);

	    if(cnt++ > 100) {
    		if(sw) {
		    GPIO_WriteBit(GPIOC, GPIO_Pin_13, 0);
		} else {
		    GPIO_WriteBit(GPIOC, GPIO_Pin_13, 1);
		}
		sw = (sw + 1) & 1;
		cnt = 0;
		if(timer10) {
		    if(!--timer10) params_save();
		}
	    }
    
	    c = keys_get();
	    if(c != oc) {
		int f = 0;
		switch(c) {
		    case KEY1:	f = dec(pos);	break;
		    case KEY2:	f = inc(pos);	break;
		    case KEY3:	if(pos) { pos--; f = 1; } break;
		    case KEY4:	if(pos < 7-1) { pos++; f = 1; } break;
		    default:;
		}
    		oc = c;
		if(f) {
		    draw(pos);
		    timer10 = 10;
		}
	    }

	    if(uart_rcvd(USART1)) {
		int i;
		int c = uart_getc(USART1);
		if(c == 'r') {
		    for(i=0; i < 16; i++) {
			uart_printf("%02x ",i2c_read(i) & 0xff);
		    }
		    uart_printf("\r\n");
		} else if(c == 'w') {
		    for(i=0; i < 16; i++) {
			i2c_write(i,i+3);
		    }
		    uart_printf("\r\n");
		} else if(c == 'a') {
    		    uint16_t a = adc_read(3);
		    uint16_t b = adc_read(4);
		    uart_printf("adc3 = %d adc4 =%d\r\n",a,b);
		} else if(c == 'g') {
		    uint16_t w=0,h=0;
		    getSize(&w,&h);
		    uint16_t i,j;
		    for(i=0, j=0; i < NELEM(colors); i++, j+=16) {
			fillRect(0,j,w,16,colors[i]);
		    }
		} else if(c == 'c') {
		    fillScreen(ILI9341_BLACK);
		    draw(pos);
		} else if(c == 'm') {
		    while(TIM_GetITStatus(TIM3, TIM_IT_CC2) == RESET);
		    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
		    while(TIM_GetITStatus(TIM3, TIM_IT_CC2) == RESET);
    		    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
//		    adc_read_n(3, aval, NELEM(aval));
		    adc_read_n_dma(3, aval, NELEM(aval));
		    uart_printf("@");
		    for(i=0; i < NELEM(aval); i++) {
			uart_printf("%d,%d\r\n",i,aval[i]);
		    }
		    uart_printf("---\r\n");
		}
	    }

    }
}


