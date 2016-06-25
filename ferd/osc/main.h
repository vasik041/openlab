/****************************************************************************/
/*
 * Osc
 * LZs,2009,2015
 * This is free and unencumbered software released into the public domain.
 */
/****************************************************************************/

/* CPU frequency */
#define F_CPU 16000000UL

/*=========================================================================*/

#define	NELEM(X)	(sizeof(X)/sizeof(X[0]))

#define LED0		sbi(PORTB,4)
#define LED1		cbi(PORTB,4)
