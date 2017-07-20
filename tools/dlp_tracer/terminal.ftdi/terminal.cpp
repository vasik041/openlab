
#include "terminal.h"
#include "portx.h"

Portx p;

//===================================================================================

char line[80];
int pos;
FILE *f;
char fnm[256] = {0};


void exec()
{
char cmd[256] = {0};

    sprintf(cmd,"graph.exe %s",fnm);
    STARTUPINFO info = { sizeof(info) };
    PROCESS_INFORMATION processInfo;
    if (CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo)) {
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
    }
}


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
	    exec();
	} else {
            if(f) fprintf(f,"%s\n",line);
	}
	pos = 0;
    }
}


int main(int argc, char* argv[])
{
uint8 x = 0;

    printf("Hello World!\n");
    for( ; ; ) {
	if(_kbhit()) {
		uint8 c = _getch();
		if(c == ESC) break;
		p.write(&c,1);
	}
	if(p.read(&x,1)) {
		_putch(x);
		dump(x);
	}
    }
    return 0;
}


//===================================================================================
