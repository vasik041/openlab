//=========================================================================
//
// Keys with ADC
// LZ,2015
// This is free and unencumbered software released into the public domain.
//
//=========================================================================

#define	KEY_LEFT_AVAL	727
#define	KEY_DOWN_AVAL	491
#define	KEY_RIGHT_AVAL	323
#define	KEY_ENTER_AVAL	130
#define	KEY_UP_AVAL	0

#define	ABS(X)		(((X) >= 0) ? (X) : (-(X)))
#define	NEAROF(X,Y)	(ABS(X-Y) < 20)

#define	KEY_UP		1
#define	KEY_DOWN	2
#define	KEY_LEFT	3
#define	KEY_RIGHT	4
#define	KEY_ENTER	5

uint16_t get_aval(uint8_t ch);
uint8_t get_key();


