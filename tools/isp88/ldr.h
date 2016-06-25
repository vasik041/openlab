//==========================================================================
//
// ISP ATmega88
// This is free and unencumbered software released into the public domain.
// LZ,2012
//
//==========================================================================

#define	elif	else if

//==========================================================================

#define	FLSIZE		(8*1024)
#define	FLPG_SIZE	32		//words

#define	EESIZE		512
#define	EEPG_SIZE	4		//bytes

#define	ESC	27

//==========================================================================

int	tx(int b);
int	penable(void);
void	cerase(void);
int	pread(int adr,int h);
void	pwrite(int adr,int h,int b);
int	eread(int adr);
void	ewrite(int adr,int b);
int	sread(int b);
void	wrpg(int adr);

void	readblk(char *fnm,long sz);
void	writeblk(char *fnm,long sz);
void	Usage(void);

void	readfls(void);
void	writefls(void);
void	verifyfls(void);
void	pgm(int adr,int h,int d);

void	wree(void);
void	rdee(void);
void	plocks(int l);
void	readsign(void);
char	*ps(int s1,int s2);

void	kill(void);

//==========================================================================
