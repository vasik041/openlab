/****************************************************************************/
/*
 * Pulse gen using atm88/T1
 * (c) LZs,2012-2014
 */
/****************************************************************************/

/* CPU frequency */
#define F_CPU	20000000UL

#define K1M	50UL	// 50ns = 1/20MHz
#define K1D	1UL

#define	SCALE(X)	(K1M*X/K1D)
#define	SCALE_1(X)	(K1D*X/K1M)

#define NELEM(X)	(sizeof(X)/sizeof(X[0]))


#define	LED0	sbi(PORTB,0)
#define	LED1	cbi(PORTB,0)

#define	KF	((PIND & 0x20) == 0)
#define	KD1	((PIND & 8) == 0)
#define	KD2	((PIND & 0x10) == 0)

// rot INT0
#define	DRC	((PIND & 0x40) == 0)
#define	KEY	((PIND & 0x80) == 0)

/*=========================================================================*/
