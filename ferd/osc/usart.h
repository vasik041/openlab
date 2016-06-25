/*=========================================================================*/
/*
 * Serial communication API
 * LZ,2009-2015
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

#define	STD_TIMEOUT	100

void USART_Init(uint16_t baud);
void USART_Transmit(uint8_t d);
uint8_t USART_Ready(void);
uint8_t USART_Receive(uint16_t t);
void USART_Flush(void);
uint8_t USART_Timeout(void);

uint8_t h2d(uint8_t h);
uint8_t rhex(void);
uint8_t d2h(uint8_t b);
void thex(uint8_t b);
void thexw(uint16_t b);

