/****************************************************************************/
/*
 * Sync for FeR
 * LZs,2018
 */
/****************************************************************************/

/* CPU frequency */
#define F_CPU 8000000UL

#define	NELEM(X)	(sizeof(X)/sizeof(X[0]))

#define	LED	5		//PB5
#define SYNC	6		//PB6

#define	KEY1	(PINA & 4) 	//PA2
#define	KEY4	(PINA & 8) 	//PA3
#define	KEY3	(PINA & 0x10) 	//PA4
#define	KEY2	(PINA & 0x20) 	//PA5

#define CH1	sbi(PORTA,6)
#define CH0	cbi(PORTA,6)

#define CH21	sbi(PORTA,7)
#define CH20	cbi(PORTA,7)

/*=========================================================================*/
