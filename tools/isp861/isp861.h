//==========================================================================
//
// ISP for ATtiny861
// This is free and unencumbered software released into the public domain.
// LZs,2003-2017
//
//==========================================================================

#define	FLSIZE	(8*1024)
#define	EESIZE	512

//==========================================================================

int 	init(void);
void 	release(void);
int	tx(int b);

int	penable(void);
void	cerase(void);
int	flread(int adr,int h);
void	flwrite(int adr,int h,int b);
int	eeread(int adr);
void	eewrite(int adr,int b);
int	sread(int b);
void	wrpg(int adr);

int	readblk(char *fnm,long sz);
void	writeblk(char *fnm,long sz);
void	usage(void);

void	readfls(void);
void	writefls(void);
void	verifyfls(void);
void	pgm(int adr,int h,int d);

void	wree(void);
void	rdee(void);
void	plocks(int l);
void	readsign(void);
char	*ps(int s1,int s2);

//==========================================================================
