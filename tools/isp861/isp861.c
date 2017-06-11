//==========================================================================
//
// ISP for ATtiny861
// This is free and unencumbered software released into the public domain.
// LZs,2003-2017
//
//==========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <ftdi.h>
#include "isp861.h"

//==========================================================================

#define	delay(X) usleep((X)*1000)

#define	RST	0x40
#define	OE	0x20

#define	RST0	setLoBits(0x00, 0x63)
#define	RST1	setLoBits(RST,  0x63)

//==========================================================================

uint8_t buf[FLSIZE];
int sz;
int err;

struct ftdi_context *ftdi = NULL;

//==========================================================================

int init()
{
int f;
    if ((ftdi = ftdi_new()) == 0) {
        fprintf(stderr, "ftdi_new failed\n");
        return -1;
    }

    f = ftdi_usb_open(ftdi, 0x0403, 0x6010);
    if (f < 0 && f != -5) {
        fprintf(stderr, "unable to open ftdi device: %d (%s)\n", f, ftdi_get_error_string(ftdi));
	ftdi_free(ftdi);
	ftdi = NULL;
        return -1;
    }

    f = ftdi_set_bitmode(ftdi, 0xff, BITMODE_MPSSE);
    if(f != 0) {
	fprintf(stderr, "unable to set bit mode: %d (%s)\n", f, ftdi_get_error_string(ftdi));
	release();
	return -1;
    }

    uint8_t p[] = { 0x80, 0, 3 };
    f = ftdi_write_data(ftdi, p, 3);
    if(f != 3) {
	fprintf(stderr, "unable to write command to ftdi device: %d (%s)\n", f, ftdi_get_error_string(ftdi));
	release();
	return -1;
    }

    //Set TCK/SK Divisor 0x86, 0xValueL, 0xValueH
    uint8_t p1[] = { 0x86, 0, 1 };	//250KHz
//  uint8_t p1[] = { 0x86, 0, 10 };	//25KHz
    
    f = ftdi_write_data(ftdi, p1, 3);
    if(f != 3) {
	fprintf(stderr, "unable to write command2 to ftdi device: %d (%s)\n", f, ftdi_get_error_string(ftdi));
	release();
	return -1;
    }

    return 0;
}



int tx(int d)
{
uint8_t cmd[] = { 0x37, 7, d };
uint8_t ans = 0;
int i,f;
    f = ftdi_write_data(ftdi, cmd, 3);
    if(f != 3) {
	fprintf(stderr, "unable to write command3 to ftdi device: %d (%s)\n", f, ftdi_get_error_string(ftdi));
	err = 1;
	return 0;
    }
    
    for(i=0; i < 10; i++) {    
	f = ftdi_read_data(ftdi,&ans,1);
	if(f == 0) {
	    usleep(1);
	    continue;
	}
        if(f == 1) break;
	else {
    	    fprintf(stderr, "unable to read data from ftdi device: %d (%s)\n", f, ftdi_get_error_string(ftdi));
	    err = 1;
	    return 0;
	}
    }

    return ans & 0xff;
}



void setLoBits(uint8_t val,uint8_t drc)
{
uint8_t cmd[] = { 0x80, val, drc };
int f;
    f = ftdi_write_data(ftdi, cmd, 3);
    if(f != 3) {
	fprintf(stderr, "unable to write command4 to ftdi device: %d (%s)\n", f, ftdi_get_error_string(ftdi));
	err = 1;
	return;
    }
}



void release(void)
{
    if(ftdi) {
	ftdi_disable_bitbang(ftdi);
        ftdi_usb_close(ftdi);
	ftdi_free(ftdi);
	ftdi = NULL;
    }
}

//==========================================================================
//
// Enable programming
//

int penable(void)
{
int i;
int r1,r2,r3,r4;
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
		printf("\nr1=%02x r2=%02x r3=%02x r4=%02x",r1,r2,r3,r4);
		delay(100);
	}
	if(err) return 1;
	return 0;
}

//==========================================================================
//
// Chip erase
//

void cerase(void)
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

int flread(int adr,int h)
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

void flwrite(int adr,int h,int d)
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
//
// read eeprom
//

int eeread(int adr)
{
	tx(0xa0);
	tx(adr >> 8);
	tx(adr);
	return tx(0);
}

//==========================================================================
//
// write eeprom
//

void eewrite(int adr,int d)
{
	tx(0xc0);
	tx(adr >> 8);
	tx(adr);
	tx(d);
}

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
	else if(strcmp(s,"l2") == 0) lck = 2;
	else if(strcmp(s,"l12") == 0) lck = 3;
	else {
	    printf("\nStrange locks %s",s);
	}
	if(lck == 0) {
	    err = 1;
	    return;
	}

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

int readblk(char *fnm,long lclsz)
{
FILE *f;
int i;
    for(i=0; i < lclsz; i++)
	buf[i] = 0xff;

    if((f = fopen(fnm,"rb+")) == NULL) {
	printf("\nError reading %s",fnm);
	return -1;
    }
    sz = fread(buf,1,lclsz,f);
    if(sz <= 0) {
	printf("\nError reading %s rc=%d",fnm,sz);
    }
    fseek(f,0,SEEK_END);
    if(ftell(f) > lclsz) {
	printf("\nwarning: %s size %ld > %ld\n",fnm,ftell(f),lclsz);
    }
    fclose(f);
    return 0;
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
	    err=1;
	    return;
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
		printf("\rreading... %4x",i/2);
		buf[i]   = flread(i/2,0);
		buf[i+1] = flread(i/2,1);
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

	int npg = sz/64 +1;
	if(npg > 128) npg = 128;
	printf("number of pages: %d\n",npg);

	for(i=0; i < npg; i++) {
		printf("\rwriting... %4x",i);

		for(j=0; j < 32; j++) {
			b1 = buf[64*i + 2*j];
			flwrite(j,0,b1);
			b1 = buf[64*i + 2*j + 1];
			flwrite(j,1,b1);
		}
		wrpg(32 * i);
		delay(10);
	}
	if(err) printf("Error at %x:%x",i,j);
	printf("\rwriting Ok     %d\n",i);
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
		printf("\ree reading... %4x",i);
		buf[i] = eeread(i);
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
		printf("\ree writing ... %4x",i);
		eewrite(i,buf[i]);
		delay(50);
		if((e = eeread(i)) != buf[i]) {
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
	printf("\ns2 = %x %s",s2, (s1 == 0x93 && s2 == 0x0d) ? "ATtiny861" : "");
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

//	if((fc=fopen("osc.inc","w")) == NULL) {
//		printf("can\'t create osc.inc");
//		return;
//	}
//	fprintf(fc,"\n\t.dw\t0x00%02x\n\n",cb & 0xff);
//	fclose(fc);
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

void usage(void)
{
	printf("\nUsage:");
	printf("\nisp861 -r file -- read pmem to file");
	printf("\nisp861 -w file -- write file to pmem ");
	printf("\nisp861 -d file -- read ee to file");
	printf("\nisp861 -u file -- write file to ee");
	printf("\nisp861 -l 1|12 -- write lock bits");
	printf("\nisp861 -fl n   -- write fuse low byte");
	printf("\nisp861 -fh n   -- write fuse high byte");
	printf("\nisp861 -fe n   -- write fuse ext byte");
	printf("\nisp861 -s      -- read signature bytes");
	printf("\n\n");
	exit(1);
}

//==========================================================================

time_t	start_time = 0;
time_t	stop_time  = 0;
time_t	elapsed_time;



int main(int argc,char *argv[])
{
    printf("\n* ATTiny861 loader vers. 1.0 *");
    printf("\nLZs,2002-2017");

    if(argc < 2 || *argv[1] != '-') usage();
    if(init() != 0) {
        exit(1);
    }
    switch(argv[1][1]) {
	case 'w':
		    if(readblk(argv[2],FLSIZE) == 0) {
			time( &start_time );
		        writefls();
		        time( &stop_time );
		    }
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
		    if(readblk(argv[2],EESIZE) == 0) {
			wree();
		    }
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

    	default:
		    printf("Unrecognised option %c\n",argv[1][1]);
    }
    RST1;
    setLoBits(RST | OE, 0x63);
    release();
    elapsed_time = stop_time - start_time;
    if(elapsed_time) printf("\nElapsed time %ld sec.",elapsed_time);
    printf("\n\n");
    return 0;
}

//==========================================================================
