/*=========================================================================*/
/**
 * Keyboard for ARD216 LCD shield
 *
 * FE R&D group,2014
 * This is free and unencumbered software released into the public domain.
 */
/*=========================================================================*/

#define	KEY_UP_AVAL	499
#define	KEY_DOWN_AVAL	677
#define	KEY_LEFT_AVAL	860
#define	KEY_RIGHT_AVAL	253
#define	KEY_OK_AVAL	0

#define	ABS(X)		(((X) >= 0) ? (X) : (-(X)))
#define	NEAROF(X,Y)	(ABS(X-Y) < 10)

#define	KEY_UP		1
#define	KEY_DOWN	2
#define	KEY_LEFT	3
#define	KEY_RIGHT	4
#define	KEY_OK		5


uint8_t get_key();


