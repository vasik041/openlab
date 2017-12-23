
#include "include/due_sam3x.h"
#include <stdint.h>
#include "uart.h"


Uart *_pUart = UART;
static uint8_t timeout;


void uart_init(uint32_t dwBaudRate)
{
uint32_t modeReg = (Mode_8N1 & 0x00000E00) | UART_MR_CHMODE_NORMAL;

    PIO_Configure(PIOA, PIO_PERIPH_A, PIO_PA8A_URXD|PIO_PA9A_UTXD, PIO_DEFAULT);
    PIO_PullUp(PIOA,PIO_PA8A_URXD,1);

    // Configure PMC
    pmc_enable_periph_clk(ID_UART);

    // Disable PDC channel
    _pUart->UART_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS;

    // Reset and disable receiver and transmitter
    _pUart->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RXDIS | UART_CR_TXDIS;

    // Configure mode
    _pUart->UART_MR = modeReg;

    // Configure baudrate (asynchronous, no oversampling)
    _pUart->UART_BRGR = (SystemCoreClock / dwBaudRate) >> 4;

    // Configure interrupts
    _pUart->UART_IDR = 0xFFFFFFFF;
    //_pUart->UART_IER = UART_IER_RXRDY | UART_IER_OVRE | UART_IER_FRAME;

    // Enable UART interrupt in NVIC
    //NVIC_EnableIRQ(_dwIrq);

    // Enable receiver and transmitter
    _pUart->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
}



uint8_t uart_available(void)
{
    if((_pUart->UART_SR & UART_SR_RXRDY) == UART_SR_RXRDY)
	return 1;
    return 0;
}


void uart_write(uint8_t uc_data)
{
    while ((_pUart->UART_SR & UART_SR_TXEMPTY) != UART_SR_TXEMPTY);
    _pUart->UART_THR = uc_data;
    Sleep(1);
}


uint8_t uart_read(void)
{
//    while((_pUart->UART_SR & UART_SR_RXRDY) != UART_SR_RXRDY);
    return _pUart->UART_RHR;
}


void uart_puts(const char *s)
{
    while(*s)
	uart_write(*s++);
}



uint8_t uart_recv(uint32_t t)
{
uint32_t w;
    for(w=0; w < t && !uart_available(); w++) {
//	Sleep(1);
    }
    if(!uart_available()) {
	timeout = 1;
	return 0;
    }
    timeout = 0;
    return uart_read();
}


uint8_t uart_timeout(void)
{
    return timeout;
}

//==============================================================================

void nl(void)
{
    uart_write('\r');
    uart_write('\n');
}


void uart_putd(int d)
{
int msk = 10000;
int i;
    for(i=0; i < 5; i++) {
	uart_write('0'+d/msk);
	d %= msk;
	msk /= 10;
    }
}


