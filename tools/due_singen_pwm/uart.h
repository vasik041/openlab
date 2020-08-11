
#ifndef _UART_H
#define _UART_H

#define Mode_8N1  (US_MR_CHRL_8_BIT | US_MR_NBSTOP_1_BIT | UART_MR_PAR_NO)

void	uart_init(uint32_t dwBaudRate);
uint8_t uart_available(void);
void	uart_write(uint8_t c);
uint8_t uart_read(void);
void	uart_puts(const char *s);

#endif // _UART_H
