/****************************************************************************/
/*
 * TPU driver
 * LZs,2014
 */
/****************************************************************************/

/* CPU frequency */
#define F_CPU	20000000UL

#define K1M	50UL	// 50ns = 1/20MHz
#define K1D	1UL

#define	SCALE(X)	(K1M*X/K1D)
#define	SCALE_1(X)	(K1D*X/K1M)

#define NELEM(X)	(sizeof(X)/sizeof(X[0]))


#define	LED0	sbi(PORTD,1)
#define	LED1	cbi(PORTD,1)

#define	K01	sbi(PORTD,5)
#define	K00	cbi(PORTD,5)

#define	K11	sbi(PORTD,6)
#define	K10	cbi(PORTD,6)

#define	K21	sbi(PORTD,7)
#define	K20	cbi(PORTD,7)

#define	T41	sbi(PORTB,0)
#define	T40	cbi(PORTB,0)


/*=========================================================================*/
