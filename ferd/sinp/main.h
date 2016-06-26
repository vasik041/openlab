/****************************************************************************/
/*
 * sin/pls gen
 * LZs,2011
 * This is free and unencumbered software released into the public domain.
 */
/****************************************************************************/

#ifndef MAIN_H
#define MAIN_H

#define F_CPU 	16000000UL // CPU frequency
#define	NELEM(X) (sizeof(X)/sizeof(X[0]))

#define LED1	sbi(PORTB,4)
#define LED0	cbi(PORTB,4)

#define OUT1	sbi(PORTB,1)
#define OUT0	cbi(PORTB,1)


#endif //MAIN_H
