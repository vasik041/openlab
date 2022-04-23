//=========================================================================
//
// Keys with ADC
// This is free and unencumbered software released into the public domain.
// LZ,2015-2020
//
//=========================================================================

#define	KEY_UP_AVAL	523
#define	KEY_DOWN_AVAL	1182
#define	KEY_LEFT_AVAL	1684
#define	KEY_RIGHT_AVAL	0
#define	KEY_SELECT_AVAL	2285

#define	NEAROF(X,Y)	(X > Y-200 && X < Y+200)

#define	KEY_UP		1
#define	KEY_DOWN 	2
#define	KEY_LEFT 	3
#define	KEY_RIGHT	4
#define	KEY_SELECT	5

uint8_t get_key(void);


