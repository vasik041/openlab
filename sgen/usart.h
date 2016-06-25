/****************************************************************************/
/*
 * Serial comunication API
 * This is free and unencumbered software released into the public domain.
 * LZ,2014
 */
/****************************************************************************/

#define STD_TIMEOUT	100

void USART_Init(uint16_t baud);
void USART_Transmit(uint8_t data);
uint8_t USART_Ready(void);
uint8_t USART_Receive(uint16_t t);
uint8_t USART_Timeout(void);

uint8_t h2d(uint8_t h);
uint8_t rhex(void);
uint8_t d2h(uint8_t b);
void thex(uint8_t b);

