//====================================================================================
//
// Port.h: interface for the Port class.
//
//====================================================================================

#if !defined(AFX_PORT_H__DA3F6661_EB77_11D3_A4CA_008048B07AD0__INCLUDED_)
#define AFX_PORT_H__DA3F6661_EB77_11D3_A4CA_008048B07AD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <atlbase.h>

//====================================================================================

enum PortErr {
	P_OK = 0,
	P_OPEN,P_OPEN2,P_OPEN3,P_OPEN4,P_OPEN5,
	P_SEND,
	P_RCV,P_TO,
	P_CLOSE,
	P_SETDTR,P_CLRDTR
};

//====================================================================================

class Port  
{
	BOOL	fOpen;
	HANDLE	m_hCommPort;
	char	s[256];
	
public:
	Port();
	virtual ~Port();

	long	PortOpen(long port,long baud);
	void	PortClose(void);
	long	Send(long b);
	long	Rcv(void);
	long	RcvW(long to);
	long	Clear(long n);

	LPSTR	ErrMsg(void);
	PortErr	Error;
	long	ErrNum;

	void	SetDTR(void);
	void	ClrDTR(void);

	BOOL	IsOpen(void)	{ return fOpen; };
};

//====================================================================================

#endif // !defined(AFX_PORT_H__DA3F6661_EB77_11D3_A4CA_008048B07AD0__INCLUDED_)
