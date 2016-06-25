/***************************************************************************/
/*
 * ds1307
 * LZ,2010
 * This is free and unencumbered software released into the public domain.
 */
/***************************************************************************/

//
// ds1307 registers
//
#define	SECONDS	0
#define	MINUTES	1
#define	HOURS	2
#define	DAY	3
#define	DATE	4
#define	MONTH	5
#define	YEAR	6
#define	CONTROL	7
#define	RTCRAM	8


int8_t clock_init(void);
void set_time(void);
void get_time(void);

uint8_t b2bcd(uint8_t x);
uint8_t bcd2b(uint8_t x);

/***************************************************************************/
