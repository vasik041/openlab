//===================================================================================
//
// Com port terminal
// LZ,2007-2018
//
//===================================================================================

#include "terminal.h"
#include "port.h"

Port com;

char line[80];
int pos;
FILE *f;
char fnm[256] = {0};

//===================================================================================

void sendCmd(char *s)
{
	while(*s) {
		char c = *s++;
		com.Send(c);
		printf("%c",c);
		Sleep(1);
	}
}

void sendCmd1()
{
	int x=0;
	com.Send('s');
	Sleep(1);
	for(int i=0; i < 256; i++) {
		char str[10] = {0};
		sprintf(str,"%02x",i);
		com.Send(str[0]);
		Sleep(1);
		com.Send(str[1]);
		Sleep(1);
		x ^= i;
	}
	com.Send('\n');
	printf("x = %02x\n",x & 0xff);
}

//===================================================================================


void open(void)
{
time_t rawtime;
struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf(fnm,"%02d-%02d-%02d--%02d-%02d-%04d.txt",
	    timeinfo->tm_hour,
	    timeinfo->tm_min,
	    timeinfo->tm_sec,
	        timeinfo->tm_mday,
	    timeinfo->tm_mon + 1,
	    timeinfo->tm_year + 1900);
    if(f) {
	fclose(f);
	f = NULL;
    }
    if(!f) {
	f = fopen(fnm,"w");    
    }
}


void dump(char c)
{
    if(c == '@') {
	open();
	return;
    }
    if(c == '\r') return;
    if(c != '\n') {
        if(pos < NELEM(line)-1) {
    	    line[pos] = c;
    	    line[pos+1] = '\0';
    	    pos++;
	}
    } else {
	if(line[0] == '-') {
    	    if(f) fclose(f);
    	    f = NULL;
    	    //exec();
        } else {
            if(f) fprintf(f,"%s\n",line);
	}
	pos = 0;
    }
}

//===================================================================================

int main(int argc, char* argv[])
{
	if(!com.PortOpen(7,9600)) {
		printf("error %s",com.ErrMsg());
		exit(1);
	}
	com.SetDTR();
	for( ; ; ) {
		if(_kbhit()) {
			int c = _getch();
			if(c == ESC) break;
/*
			switch(c) {
				case '1':	sendCmd("f0010\n"); break;
				case '2':	sendCmd("f0011\n"); break;
				case '3':	sendCmd1(); break;
			}
*/
			com.Send(c);
		}

		long c = com.Rcv();
		if(c < 0x100) {
			dump(c);
			_putch(c);
		}

	}
	return 0;
}


//===================================================================================
