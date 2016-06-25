/****************************************************************************/
/*
 * Sample Gen
 * LZs,2015
 * This is free and unencumbered software released into the public domain.
 */
/****************************************************************************/

/* CPU frequency */
#define F_CPU 20000000UL

#define	NELEM(X)	(sizeof(X)/sizeof(X[0]))

#define	DACH		PORTC	//hi6
#define	DACL		PORTB   //lo6

#define	BR_9600		129		// (F_CPU/16/BR)-1 see page 179

#define	LO6(X)		((X) & 0x3f)
#define	HI6(X)		(((X) >> 6) & 0x3f)

/*=========================================================================*/
