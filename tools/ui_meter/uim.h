/****************************************************************************/
/*
 * Uimeter on uiptfg board
 * LZs,2008,2015
 * This is free and unencumbered software released into the public domain.
 */
/****************************************************************************/

//#define UI12	1	// 12v pwr suply

#ifdef UI12
	#define	USCALE	20
#else
	#define	USCALE	40
#endif


// CPU frequency
#define F_CPU 1000000UL

/*=========================================================================*/
