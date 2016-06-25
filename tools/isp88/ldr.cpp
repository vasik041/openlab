//==========================================================================
//
// ISP ATmega88
// This is free and unencumbered software released into the public domain.
// LZ,2012
//
//==========================================================================

#include <windows.h>
#include <stdio.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <time.h>

#include "ldr.h"
#include "portx.h"

#define	delay(X) Sleep(X)

//==========================================================================

#define	RST		0x40
#define	OE		0x20

#define	RST0	p.setLoBits(0x00, 0x63)
#define	RST1	p.setLoBits(0x40, 0x63)


//==========================================================================

Portx p(DEVICE_USB2232);

unsigned char buf[FLSIZE];
int	sz;

int	err = 0;

//==========================================================================

int tx(int d)
{
	return p.sendByte(d);
}

//==========================================================================
//
// Enable programming
//

int penable()
{
int i,r3;
int r1,r2,r4;

	for(i=0; i < 2000; i++) {
		RST0;	delay(10);
		RST1;	delay(10);
		RST0;	delay(20);

		r1 = tx(0xac);
		r2 = tx(0x53);
		r3 = tx(0);
		r4 = tx(0);
		if(r3 == 0x53) return 1;

		RST1;
		printf("\nr1=%x r2=%x r3=%x r4=%x",r1,r2,r3,r4);
		delay(100);
	}
	return 0;
}

//==========================================================================
//
// Chip erase
//

void cerase()
{
	tx(0xac);
	tx(0x80);
	tx(0x00);
	tx(0x00);
}

//==========================================================================
//
// read flash
//

int pread(int adr,int h)
{
	h &= 1;
	tx(0x20 + h * 8);
	tx(adr >> 8);
	tx(adr);
	return tx(0xff);
}

//==========================================================================
//
// load flash page
//

void pwrite(int adr,int h,int d)
{
	tx(h ? 0x48 : 0x40);
	tx((adr >> 8) & 0xff);
	tx(adr & 0xff);
	tx(d);
}

//==========================================================================
//
// write flash page
//

void wrpg(int adr)
{
	tx(0x4c);
	tx((adr >> 8) & 0xff);
	tx(adr & 0xff);
	tx(0);
}

//==========================================================================
//==========================================================================
//
// read eeprom
//

int eread(int adr)
{
	tx(0xa0);
	tx(adr >> 8);
	tx(adr);
	return tx(0);
}

int erdpg(int pg)
{
	tx(0xc1);
	tx(0);
	tx(pg);
	return tx(0);
}

//==========================================================================
//
// write eeprom
//

void ewrite(int adr,int d)
{
	tx(0xc0);
	tx(adr >> 8);
	tx(adr);
	tx(d);
}

//==========================================================================
//
// write eeprom page
//

void ewrpg(int pg)
{

	tx(0xc2);
	tx((pg >> 6) & 3);
	tx((pg << 6) & 0xfc);
	tx(0);
}

//==========================================================================
//==========================================================================
//
// programm lock bits
//

void plocks(int l)
{
	l &= 3;
	l |= 0xfc;

	tx(0xac);
	tx(0xe0);
	tx(0);
	tx(l);
}

int rlocks()
{
	tx(0x58);
	tx(0);
	tx(0);
	return tx(0);
}

//==========================================================================
//
// Read signature byte
//

int sread(int b)
{
	tx(0x30);
	tx(0);
	tx(b & 3);
	return tx(0);
}

//==========================================================================
//
// Read calibration byte
//

int cbread()
{
	tx(0x38);
	tx(0);
	tx(0);
	return tx(0);
}

//==========================================================================
//
// read fuses low/high
//

int rfuses(int h)
{
	h &= 1;
	tx(0x50+h*8);
	tx(h*8);
	tx(0);
	return tx(0);
}

//==========================================================================
//
// read ext fuses
//

int rextfuses()
{
	tx(0x50);
	tx(8);
	tx(0);
	return tx(0);
}

//==========================================================================
//
// write fuses low/high
//

void wfuses(int fs,int h)
{
	h &= 1;
	tx(0xac);
	tx(0xa0+h*8);
	tx(0);
	tx(fs);
}

//==========================================================================
//
// write ext fuses
//

void wextfuses(int fs)
{
	tx(0xac);
	tx(0xa4);
	tx(0);
	tx(fs);
}

//==========================================================================
//==========================================================================
//
// Write locks
//

void wlocks(char *s)
{
int i;
int lck = 0;
	if(strcmp(s,"l1") == 0) lck = 1;
	elif(strcmp(s,"l2") == 0) lck = 2;
	elif(strcmp(s,"l12") == 0) lck = 3;
	else {
err:
		printf("\nStrange locks %s",s);
		exit(1);
	}
	if(lck == 0) goto err;

	printf("\nconnecting...%s", ((i = penable()) == 1)? "Ok" : "Error");
	if(!i) {
		err = 1;
		return;
	}
	plocks(lck);
}

//==========================================================================
//
// read block from file
//

void readblk(char *fnm,long lclsz)
{
FILE *f;
int i;
	for(i=0; i < lclsz; i++)
		buf[i] = 0xff;

	if((f = fopen(fnm,"rb+")) == NULL) {
		printf("\nError reading %s",fnm);
		exit(1);
	}
	sz = fread(buf,1,lclsz,f);
	fclose(f);
}

//==========================================================================
//
// save buffer
//

void writeblk(char *fnm,long sz)
{
FILE *f;
	if((f = fopen(fnm,"wb+")) == NULL) {
		printf("\nError writing %s",fnm);
		exit(1);
	}
	fwrite(buf,1,sz,f);
	fclose(f);
}

//==========================================================================
//==========================================================================
//
// read flash
//

void readfls()
{
int i;
	printf("\nconnecting...%s", ((i = penable()) == 1)? "Ok" : "Error");
	if(!i) {
		err = 1;
		return;
	}

	printf("\n");
	for(i=0; i < FLSIZE; i += 2) {
		if(_kbhit()) exit(1);
		printf("\rreading... %4x",i/2);

		buf[i]   = pread(i/2,0);
		buf[i+1] = pread(i/2,1);
	}
	printf("\rreading Ok     \n");
	err = 0;
}

//==========================================================================
//
// write flash
//

void writefls()
{
int i,j,b1;
	printf("\nconnecting...%s", ((i = penable()) == 1) ? "Ok" : "Error");
	if(!i) {
		err = 1;
		return;
	}

	printf("\nerasing...");
	cerase();
	delay(5000);

	err = 0;
	printf("\n");

	int npg = sz/(FLPG_SIZE*2) +1;
	if(npg > 128) npg = 128;
	printf("number of pages: %d\n",npg);

	for(i=0; i < npg; i++) {
		if(_kbhit()) break;
		printf("\rwriting... %4x",i);

		for(j=0; j < FLPG_SIZE; j++) {
			b1 = buf[FLPG_SIZE*i*2 + 2*j];
			pwrite(j,0,b1);
			b1 = buf[FLPG_SIZE*i*2 + 2*j + 1];
			pwrite(j,1,b1);
		}
		wrpg(32 * i);
		delay(10);
	}
	if(err) printf("Error at %x:%x",i,j);
	printf("\rwriting Ok     \n");
}

//==========================================================================
//
// Read from EEPROM
//

void rdee()
{
int i;
	printf("\nconnecting...%s", ((i = penable()) == 1)? "Ok" : "Error");
	if(!i) {
		err = 1;
		return;
	}

	printf("\n");
	for(i=0; i < EESIZE; i++) {
		if(_kbhit()) exit(1);
		printf("\ree reading... %4x",i);
		buf[i] = eread(i);
	}
	printf("\ree reading Ok     \n");
	err = 0;
}

//==========================================================================
//
// Write to EEPROM
//

void wree()
{
int i,e;
	printf("\nconnecting...%s", ((i = penable()) == 1) ? "Ok" : "Error");
	if(!i) {
		err = 1;
		return;
	}
	err = 0;
	printf("\n");
	for(i=0; i < sz; i++) {
		if(_kbhit()) break;
		printf("\ree writing ... %4x",i);
		ewrite(i,buf[i]);
		delay(50);
		if((e = eread(i)) != buf[i]) {
			printf("\nerror at %x ee %x buf %x\n",i,e & 0xff,buf[i] & 0xff);
			err = 1;
		}
		if(err) break;
	}
	if(!err) printf("\ree writing Ok      \n");
}

//==========================================================================
//
// Read signature bytes
//

void readsign()
{
int i;
int s0,s1,s2;

	printf("\nconnecting...%s", ((i = penable()) == 1) ? "Ok" : "Error");
	if(!i) {
		err = 1;
		return;
	}
	err = 0;
	s0 = sread(0);
	s1 = sread(1);
	s2 = sread(2);

	if(s0 == 0 && s1 == 1 && s2 == 2) {
		printf("\nDevice locked...");
		return;
	}
	printf("\ns0 = %x %s",s0, (s0 == 0x1e) ? "Atmel" : "");
	printf("\ns1 = %x",s1);
	printf("\ns2 = %x %s",s2, (s1 == 0x93 && s2 == 0x0a) ? "ATmega88" : "");
}

//==========================================================================

void rcalib()
{
FILE *fc;
int cb;
	printf("\nCb = %x",cb = cbread());
	printf("\nFsl = %x",rfuses(0));
	printf("\nFsh = %x",rfuses(1));
	printf("\nFse = %x",rextfuses());
	printf("\nLck = %x",rlocks());

	if((fc=fopen("osc.inc","w")) == NULL) {
		printf("can\'t create osc.inc");
		return;
	}
	fprintf(fc,"\n\t.dw\t0x00%02x\n\n",cb & 0xff);
	fclose(fc);
}

//==========================================================================

void writefuses(char *fuses,char ftp)
{
int i=0;
int fx=0;
	printf("\nconnecting...%s", ((i = penable()) == 1) ? "Ok" : "Error");
	if(!i) {
		err = 1;
		return;
	}
	err = 0;
	sscanf(fuses,"%x",&fx);

	switch(ftp) {
		case 'l':
			printf("\nFuse low = %02x",fx);
			wfuses(fx,0);
			break;

		case 'h':
			printf("\nFuse high = %02x",fx);
			wfuses(fx,1);
			break;

		case 'e':
			printf("\nFuse ext = %02x",fx);
			wextfuses(fx);
			break;

		default:
			printf("unknown fuse type: %c\n",ftp);
	}
}

//==========================================================================
//
// print usage message
//

void Usage()
{
	printf("\nUsage:");
	printf("\nldr88 -r file -- read pmem to file");
	printf("\nldr88 -w file -- write file to pmem ");
	printf("\nldr88 -d file -- read ee to file");
	printf("\nldr88 -u file -- write file to ee");
	printf("\nldr88 -l 1|12 -- write lock bits");
	printf("\nldr88 -fl n   -- write fuse low byte");
	printf("\nldr88 -fh n   -- write fuse high byte");
	printf("\nldr88 -fe n   -- write fuse ext byte");
	printf("\nldr88 -s      -- read signature/fuse/lock bytes");
	exit(1);
}

//==========================================================================

time_t	start_time = 0;
time_t	stop_time  = 0;
time_t	elapsed_time;



void main(int argc,char *argv[])
{
/*
	for( ; !_kbhit(); ) {
		Sleep(100);
		p.setHiBits(0,1);
		uint8 a = p.sendByte(0x55);

		Sleep(100);
		p.setHiBits(1,1);
		uint8 b = p.sendByte(0xaa);

		printf("a = %02x b = %02x\n",a & 0xff,b & 0xff);
	}
	p.setHiBits(0,0);
	exit(0);
*/
	printf("\n* ATmega88 loader vers. 1.0 *");
	printf("\nLZs,2002-2012");

//	for( ; ; ) {
//		if(kbhit()) exit(1);
//        	printf("\n%d",DIN);
//	}
//	clrscr();

	if(*argv[1] != '-' || argc < 2) Usage();
	switch(argv[1][1]) {
		case 'w':
		       readblk(argv[2],FLSIZE);
		       time( &start_time );
		       writefls();
		       time( &stop_time );
		       break;

		case 'r':
			readfls();
			if(!err) writeblk(argv[2],FLSIZE);
			break;

		case 'd':
			rdee();
			if(!err) writeblk(argv[2],EESIZE);
			break;

		case 'u':
			readblk(argv[2],EESIZE);
			wree();
			break;

		case 'l':
			wlocks(&argv[1][1]);
			break;

		case 'f':
			writefuses(argv[2],argv[1][2]);
			break;

		case 's':
			readsign();
			rcalib();
			break;
	}
	RST1;
	p.setLoBits(0x60, 0x63);
//	p.setLoBits(0,0);
//	CLK1;
	elapsed_time = stop_time - start_time;
	if(elapsed_time) printf("\nElapsed time %ld sec.",elapsed_time);
}

//==========================================================================
