/*=========================================================================*/
/*
 * 4 x 7seg LED display driver
 * This is free and unencumbered software released into the public domain.
 * LZs,2005
 */
/*=========================================================================*/

#define	segA	1	//porta
#define	segB	4
#define	segC	0x20
#define	segD	0x80
#define	segE	8
#define	segF	2
#define	segG	0x10
#define	segDP	0x40

#define	cat0	0	//portb
#define	cat1	1
#define	cat2	2
#define	cat3	3

/*=========================================================================*/

void px(UCHR b);
void putc(UCHR b);
void putx(UCHR b);
void putd(UCHR n);
void putd2(UCHR n);
void home(void);
void cls(void);
void Setup_Display(void);

/*=========================================================================*/


