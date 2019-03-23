/*
 * This is free and unencumbered software released into the public domain.
 */

void uart_init();
int uart_putc(USART_TypeDef* USARTx, char c);
int uart_rcvd(USART_TypeDef* USARTx);
int uart_getc(USART_TypeDef* USARTx);
void uart_puts(const char *p);
void uart_printf(const char* format, ...);
