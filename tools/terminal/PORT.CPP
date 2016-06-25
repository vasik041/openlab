//====================================================================================
//
// COM port operations
// LZ, 2000-2016
//
//====================================================================================

#include "Port.h"

//====================================================================================
//
// Construction/Destruction
//

Port::Port()
{
	fOpen = FALSE;
}

Port::~Port()
{
	if(fOpen) PortClose();
}

//====================================================================================
//====================================================================================
//
// Open port
//

long Port::PortOpen(long port,long baud)
{
DCB dcb;
char strComPort[10];

	sprintf(strComPort,"COM%d",port);
	m_hCommPort = CreateFile(strComPort, GENERIC_WRITE | GENERIC_READ,0, NULL,
		OPEN_EXISTING, 0, 0);	//FILE_FLAG_OVERLAPPED

	if (m_hCommPort == INVALID_HANDLE_VALUE) {
		Error = P_OPEN;
		ErrNum = GetLastError();
		return 0;
	}

	if(!GetCommState(m_hCommPort, &dcb)) {
		Error = P_OPEN2;
		ErrNum = GetLastError();
		return 0;
	}
	dcb.BaudRate = baud;	//CBR_4800;
	dcb.fBinary = true;
	dcb.fParity = false;
	dcb.fOutxCtsFlow = false;
	dcb.fOutxDsrFlow = false;
//	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fDsrSensitivity = false;
	dcb.fTXContinueOnXoff = true;
	dcb.fOutX = false;
	dcb.fInX = false;
	dcb.fNull = false;
//	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	if(!SetCommState (m_hCommPort, &dcb)) {
		Error = P_OPEN3;
		ErrNum = GetLastError();
		return 0;
	}

	COMMTIMEOUTS to; 
	if(!GetCommTimeouts(m_hCommPort,&to)) {
		Error = P_OPEN4;
		ErrNum = GetLastError();
		return 0;
	}
	to.ReadIntervalTimeout = MAXDWORD; 
    to.ReadTotalTimeoutMultiplier = MAXDWORD; 
    to.ReadTotalTimeoutConstant = 10;			//00; 
	if(!SetCommTimeouts(m_hCommPort,&to)) {
		Error = P_OPEN5;
		ErrNum = GetLastError();
		return 0;
	}
	
	Error = P_OK;
	fOpen = TRUE;
	return 1;
}
 
//====================================================================================
//====================================================================================
//
// send char
//

long Port::Send(long b)
{
DWORD TxChar = b;
DWORD nBytes = 1;
	if(!WriteFile(m_hCommPort, &TxChar,1,&nBytes, NULL)) {
		ErrNum = GetLastError();
		Error = P_SEND;
		return 0;			
	}
	Error = P_OK;
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

long Port::Rcv()
{
DWORD RxChar = 0;
DWORD nBytes = 0;
	if(!ReadFile(m_hCommPort, &RxChar,1,&nBytes, NULL)) {
		ErrNum = GetLastError();
		Error = P_RCV;
		return 0x101;			
	}
	if(nBytes == 1) {
		Error = P_OK;
		return RxChar & 0xff;
	}
	Error = P_TO;
	return 0x100;
}


//===================================================================================
//
// clear input buffer
//

long Port::Clear(long n)
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
// returns 0õ100 when timeout, 0x101 if error occured, or 
// received character
//

long Port::RcvW(long to)
{
long i,c;

	for(i=0; i < to; i++) {
		c = Rcv();
		switch(Error) {
			case P_OK:	return c;
			case P_TO:	continue;
			default:	return 0x101;
		}
	}
	return 0x100;
}

//====================================================================================
//====================================================================================
//
// close port
//

void Port::PortClose()
{
	if(!fOpen) return;

	fOpen = FALSE;
	if(!CloseHandle(m_hCommPort)) {
		ErrNum = GetLastError();
		Error = P_CLOSE;
		return;
	}
	Error = P_OK;
	m_hCommPort = INVALID_HANDLE_VALUE;
}

//====================================================================================
//
// error messages
//

LPSTR Port::ErrMsg()
{
	switch(Error) {
		case P_OPEN:
			sprintf(s,"Error opening COM - %d", ErrNum);
			break;

		case P_OPEN2:
			sprintf(s,"Error getting DCB - %d", ErrNum);
			break;

		case P_OPEN3:
			sprintf(s,"Error setting DCB - %d", ErrNum); 
			break;
	
		case P_OPEN4:
			sprintf(s,"Error getting TO - %d", ErrNum); 
			break;
	
		case P_OPEN5:
			sprintf(s,"Error setting TO - %d", ErrNum); 
			break;

		case P_SEND:
			sprintf(s,"Error writing COM - %d", ErrNum); 
			break;

		case P_RCV:
			sprintf(s,"Error reading COM - %d", ErrNum); 
			break;

		case P_TO:
			sprintf(s,"Rx Timeout - %d", ErrNum); 
			break;

		case P_CLOSE:
			sprintf(s,"COM Close error - %d", ErrNum); 
			break;

		case P_OK:
			sprintf(s,"Ok !");
			break;

		case P_SETDTR:
			sprintf(s,"Set DTR error - %d", ErrNum); 
			break;

		case P_CLRDTR:
			sprintf(s,"Clr DTR error - %d", ErrNum); 
			break;

		default:;
			sprintf(s,"Unknown error code !");
		}
	return s;
}

//====================================================================================

void Port::SetDTR(void)
{
	if(EscapeCommFunction(m_hCommPort,SETDTR)) {
		Error = P_OK;
		ErrNum = 0;
		return;
	}
	if(EscapeCommFunction(m_hCommPort,SETRTS)) {
		Error = P_OK;
		ErrNum = 0;
		return;
	}
	Error = P_SETDTR;
	ErrNum = GetLastError();
}

//====================================================================================

void Port::ClrDTR(void)
{
	if(EscapeCommFunction(m_hCommPort,CLRDTR)) {
		Error = P_OK;
		ErrNum = 0;
		return;
	}
	if(EscapeCommFunction(m_hCommPort,CLRRTS)) {
		Error = P_OK;
		ErrNum = 0;
		return;
	}
	Error = P_CLRDTR;
	ErrNum = GetLastError();
}

//====================================================================================
