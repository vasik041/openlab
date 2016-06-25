/*=========================================================================*/
/**
 * Pulse generator using atm328p/Timer1
 *
 * FE R&D group,2014
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

#define K1M	625UL	// 62.5ns = 1/16MHz
#define K1D	10UL

#define	SCALE(X)	(K1M*X/K1D)
#define	SCALE_1(X)	(K1D*X/K1M)

void setup_timer1();


