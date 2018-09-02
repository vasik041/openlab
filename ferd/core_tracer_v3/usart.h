/****************************************************************************/
/*
 * Serial comunication API
 * LZ,2014
 */
/****************************************************************************/

#define	BR_9600		103		// (F_CPU/16/BR)-1 see page 179

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

