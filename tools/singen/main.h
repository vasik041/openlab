/****************************************************************************/
/*
 * SinGen
 * This is free and unencumbered software released into the public domain.
 * LZs,2009
 */
/****************************************************************************/

/* CPU frequency */
#define F_CPU 16000000UL

#define	VOL9	0	//PORTD
#define	VOL8	1       //PORTD

#define	AOUT8	7	//PORTA

#define	KEY	0x10
#define	ENC0	0x20
#define	ENC1	0x40

/*=========================================================================*/

#define	P_FREQ		0
#define	P_FREQ_STEP	1
#define	P_VOL		2
#define	P_VOL_STEP	3
#define	P_VOL_ATT	4

#define FREQ_MAX	49999000	//50Khz
#define FREQ_MIN        1000		//1Hz
#define VOL_MAX		999
#define VOL_MIN		1

/*=========================================================================*/

#define	NELEM(X)	(sizeof(X)/sizeof(X[0]))

