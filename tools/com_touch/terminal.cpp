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
	if(argc < 2) {
		printf("Usage: com_touch portN\n");
		exit(1);
	}
	int n = atoi(argv[1]);
	printf("touching COM%d\n",n);
	if(!com.PortOpen(n,1200)) {
		printf("error %s",com.ErrMsg());
		exit(1);
	}
	Sleep(1000);
	com.Send(0);
	Sleep(1000);
	com.PortClose();
	Sleep(1000);
	printf("ok\n");
	return 0;
}


//===================================================================================
