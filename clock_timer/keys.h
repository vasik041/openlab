//=========================================================================
//
// Keys with ADC
// LZ,2015
// This is free and unencumbered software released into the public domain.
//
//=========================================================================

#define	KEY_1_AVAL	475
#define	KEY_2_AVAL	296
#define	KEY_3_AVAL	136
#define	KEY_4_AVAL	0

#define	ABS(X)		(((X) >= 0) ? (X) : (-(X)))
#define	NEAROF(X,Y)	(ABS(X-Y) < 20)

#define	KEY_1	1
#define	KEY_2	2
#define	KEY_3	3
#define	KEY_4	4


uint8_t get_key();


