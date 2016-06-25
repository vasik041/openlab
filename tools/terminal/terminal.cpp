//===================================================================================
//
// Com port terminal
// LZ,2007
//
//===================================================================================

#include "terminal.h"
#include "port.h"

Port com;

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
			_putch(c);
		}

	}
	return 0;
}


//===================================================================================
