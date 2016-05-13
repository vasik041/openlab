//=============================================================================
//
// Flower Clock vers. 2.0
// This is free and unencumbered software released into the public domain.
// LZs, 2015
//
//=============================================================================

#define interrupt(x) void __attribute__((interrupt (x)))

#define	xA	2
#define	xB	0x10
#define	xC	0x20
#define	xD	8

#define	LD	0x40
#define	LC	1

#define PH	4
#define KEY	(P1IN & 0x80)	//P1.7

#define BLANK	0x0f

#define	MAX_KEY_DLY	1000
