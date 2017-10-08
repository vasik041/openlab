//====================================================================================
//
// COM port operations for Win32
// LZs, 2000-2017
//
//====================================================================================

#include <windows.h>
#include <stdio.h>
#include "commport.h"

HANDLE m_hCommPort;

//====================================================================================
//
// Open com port
//

int port_open(int port,int baud)
{
DCB dcb;
char strComPort[10];

	sprintf(strComPort,"COM%d",port);
	m_hCommPort = CreateFile(strComPort, GENERIC_WRITE | GENERIC_READ,0, NULL,
		OPEN_EXISTING, 0, 0);	//FILE_FLAG_OVERLAPPED

	if (m_hCommPort == INVALID_HANDLE_VALUE) {
		printf("Error %d\n",GetLastError());
		return -1;
	}

	if(!GetCommState(m_hCommPort, &dcb)) {
		printf("Error %d\n",GetLastError());
		return -1;
	}
	dcb.BaudRate = baud;	//CBR_4800;
	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
//	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fTXContinueOnXoff = TRUE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fNull = FALSE;
//	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	if(!SetCommState (m_hCommPort, &dcb)) {
		printf("Error %d\n",GetLastError());
		return -1;
	}

	COMMTIMEOUTS to; 
	if(!GetCommTimeouts(m_hCommPort,&to)) {
		printf("Error %d\n",GetLastError());
		return -1;
	}
	to.ReadIntervalTimeout = MAXDWORD; 
	to.ReadTotalTimeoutMultiplier = MAXDWORD; 
	to.ReadTotalTimeoutConstant = 10;			//00; 
	if(!SetCommTimeouts(m_hCommPort,&to)) {
		printf("Error %d\n",GetLastError());
		return -1;
	}
	
	return 0;
}
 
//====================================================================================
//====================================================================================
//
// send char
//

int port_send(int b)
{
DWORD TxChar = b;
DWORD nBytes = 1;
	if(!WriteFile(m_hCommPort, &TxChar,1,&nBytes, NULL)) {
		printf("Error %d\n",GetLastError());
		return 0;			
	}
	return nBytes;
}

//====================================================================================
//
// Receive char
//
// char or
// 0x100 - timeout 10ms
// 0x101 - error
//

int port_recv(void)
{
DWORD RxChar = 0;
DWORD nBytes = 0;
	if(!ReadFile(m_hCommPort, &RxChar,1,&nBytes, NULL)) {
	    printf("Error %d\n",GetLastError());
	    return 0x101;			
	}
	if(nBytes == 1) {
	    return RxChar & 0xff;
	}
	return 0x100;
}


//===================================================================================
//
// clear input buffer
//

int port_clear(int n)
{
//int i;
//	for(i=0; i < n; i++)
//		Rcv();

	BOOL r = PurgeComm(m_hCommPort,PURGE_TXCLEAR | PURGE_RXCLEAR);
	return r;
}

//===================================================================================
//
// wait for a char to*10 ms
// returns 0x100 when timeout, 0x101 if error occured, or 
// received character
//

int port_recv_wait(int to)
{
int i,c;
	for(i=0; i < to; i++) {
		c = port_recv();
		if(c != 0x100)	return c;
	}
	return 0x100;
}

//====================================================================================
//====================================================================================
//
// close port
//

void port_close(void)
{
	if(!CloseHandle(m_hCommPort)) {
	    printf("Error %d\n",GetLastError());
	}
	m_hCommPort = INVALID_HANDLE_VALUE;
}

//====================================================================================

void SetDTR(void)
{
	if(EscapeCommFunction(m_hCommPort,SETDTR)) {
		return;
	}
//	if(EscapeCommFunction(m_hCommPort,SETRTS)) {
//		return;
//	}
	printf("Error %d\n",GetLastError());
}

//====================================================================================

void ClrDTR(void)
{
	if(EscapeCommFunction(m_hCommPort,CLRDTR)) {
		return;
	}
//	if(EscapeCommFunction(m_hCommPort,CLRRTS)) {
//		return;
//	}
	printf("Error %d\n",GetLastError());
}

//====================================================================================
