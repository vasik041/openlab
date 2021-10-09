//=========================================================================
//
// Keys with ADC
//
// This is free and unencumbered software released into the public domain.
// LZ,2015
//
//=========================================================================

#define	KEY_4_AVAL	488
#define	KEY_3_AVAL	322
#define	KEY_2_AVAL	134
#define	KEY_1_AVAL	0

#define	ABS(X)		(((X) >= 0) ? (X) : (-(X)))
#define	NEAROF(X,Y)	(ABS(X-Y) < 20)

#define	KEY_1	1
#define	KEY_2	2
#define	KEY_3	3
#define	KEY_4	4


uint16_t get_aval(uint8_t ch);
uint8_t get_key();


