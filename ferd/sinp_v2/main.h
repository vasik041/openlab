/****************************************************************************/
/*
 * sin/pls gen
 * LZs,2011
 */
/****************************************************************************/

#ifndef MAIN_H
#define MAIN_H

#define F_CPU 	16000000UL // CPU frequency
#define	NELEM(X) (sizeof(X)/sizeof(X[0]))

#define LED1	sbi(PORTB,4)
#define LED0	cbi(PORTB,4)

#define OUTP_1	sbi(PORTB,1)
#define OUTP_0	cbi(PORTB,1)

#define OUT1_1	sbi(PORTD,4)
#define OUT1_0	cbi(PORTD,4)
#define OUT2_1	sbi(PORTD,5)
#define OUT2_0	cbi(PORTD,5)


#endif //MAIN_H
