//=========================================================================
//
// Keys with ADC
// This is free and unencumbered software released into the public domain.
// LZ,2015-2020
//
//=========================================================================

#define	KEY_UP_AVAL	513
#define	KEY_DOWN_AVAL	1165
#define	KEY_LEFT_AVAL	1664
#define	KEY_RIGHT_AVAL	0
#define	KEY_SELECT_AVAL	2251

#define	ABS(X)		(((X) >= 0) ? (X) : (-(X)))
#define	NEAROF(X,Y)	(ABS(X-Y) < 100)

#define	KEY_UP		1
#define	KEY_DOWN 	2
#define	KEY_LEFT 	3
#define	KEY_RIGHT	4
#define	KEY_SELECT	5

uint8_t get_key(void);


