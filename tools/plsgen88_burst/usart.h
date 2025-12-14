/****************************************************************************/
/*
 * Serial comunication API
 * LZ,2014
 * This is free and unencumbered software released into the public domain.
 */
/****************************************************************************/

//#define BR_9600	103		// 16Mhz  (F_CPU/16/BR)-1 see page 179
#define	BR_9600		129		// 20Mhz  (F_CPU/16/BR)-1 see page 179

#define STD_TIMEOUT	100

void USART_Init(uint16_t baud);
void USART_Transmit(uint8_t data);
uint8_t USART_Ready(void);
uint8_t USART_Receive(uint16_t t);
uint8_t USART_Timeout(void);


