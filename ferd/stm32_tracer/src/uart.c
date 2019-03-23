/*
 * This is free and unencumbered software released into the public domain.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

#include "mini-printf.h"

#include "main.h"
#include "uart.h"


void uart_init()
{
    // serial comms
    // use PA9 (TX) and PA10 (RX)
    // USART1

    // enable clock for USART1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    // setup Tx/RX pins:
    // init TX
    GPIO_InitTypeDef gpioInitStruct;
    GPIO_StructInit(&gpioInitStruct);
    gpioInitStruct.GPIO_Pin = GPIO_Pin_9;
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStruct);
    // init RX
    gpioInitStruct.GPIO_Pin = GPIO_Pin_10;
    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpioInitStruct);

    // setup USART1:
    // initialize struct
    USART_InitTypeDef usartInitStruct;
    USART_StructInit(&usartInitStruct);
    // set parameters
    usartInitStruct.USART_BaudRate = 115200;
    usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &usartInitStruct);
    USART_Cmd(USART1, ENABLE);
}

int uart_putc(USART_TypeDef* USARTx, char c)
{
  assert_param(IS_USART_123_PERIPH(USARTx));
  while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
  USARTx->DR =  (c & 0xff);
  return 0;
}

int uart_rcvd(USART_TypeDef* USARTx)
{
  assert_param(IS_USART_123_PERIPH(USARTx));
  return USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) != RESET;
}

int uart_getc(USART_TypeDef* USARTx)
{
  assert_param(IS_USART_123_PERIPH(USARTx));
  while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
  return  USARTx->DR & 0xff;
}



void uart_puts(const char *p)
{
    while(*p) {
	uart_putc(USART1, *p++);
    }
}

static char buf[80];

void uart_printf(const char* format, ...)
{
int ret;
va_list ap;
    va_start(ap, format);
    ret = vsnprintf(buf, sizeof(buf), format, ap);
    if(ret > 0) {
	int i;
	for(i=0; i < ret && i < sizeof(buf); i++) {
	    uart_putc(USART1, buf[i]);
	}
    }
    va_end (ap);
}

