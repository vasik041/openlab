
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include "commport.h"
#include "lcmlite_recv.h"


WSADATA wd;

DWORD WINAPI lcm_thread(LPVOID p)
//void *lcm_thread(void *p)
{
    WSAStartup(MAKEWORD(2, 0), &wd);
    lcmlite_main();
//    pthread_exit(NULL);
    return 0;
}


//==============================================================================

void print_buf(char *msg,uint8_t *buf,int len)
{
int i;
    printf("%s %d:",msg,len);
//    for(i=0; i < len; i++) {
//	printf("%02x ",buf[i] & 0xff);
//    }
//    printf(" ");
    for(i=0; i < len && i < 32; i++) {
	printf("%c",(buf[i] & 0xff) <= ' ' ? '.' : (buf[i] & 0xff));
    }
    printf("\n");
}


void lcm_recvd(uint8_t *buf, int32_t len)
{
int i;
    print_buf("dev<-",buf,len);
    for(i=0; i < len; i++) {
	port_send(buf[i]);
    }
    port_send('\n');
}



int main(int argc, char *argv[])
{
//pthread_t t;
//int rc;
HANDLE h;
DWORD id;
uint8_t buf[1024*8]={0};
int pos;

    if(port_open(4,38400)) {
          printf("Error, can\'t open com port\n");
          exit(-1);
    }

    h = CreateThread(NULL,0,lcm_thread,NULL,0,&id);
    if(h == INVALID_HANDLE_VALUE) {
        printf("Can\'t create thread %d\n",GetLastError());
        exit(-1);
    }

//    rc = pthread_create(&t, NULL, lcm_thread, NULL);
//    if(rc) {
//          printf("Error, return code from pthread_create() is %d\n", rc);
//          exit(-1);
//    }

    for(pos=0; ; ) {
	int c = port_recv_wait(1);
	if(c >= 0x100) continue;

	if(pos < sizeof(buf) && c != '\n') {	
	    buf[pos++] = c;
	} else {
	    if(buf[0] == 'x') {
		buf[pos] = '\0';
		printf("%s\n",buf);
	    } else {
		print_buf("dev->",buf,pos);
		lcm_send(buf, pos);
	    }
	    pos = 0;
	}
    }

//    pthread_join(t,NULL);
    port_close();
    return 0;
}

//==============================================================================


