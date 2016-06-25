
#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "portx.h"

//=============================================================================


Portx::Portx(char *desc) : ftHandle(NULL)
{
	if(!Open(desc)) {
		printf("can\'t open device\n");
	}
}

//=============================================================================

Portx::~Portx()
{
	Close();
}

//=============================================================================

int Portx::Open(char *desc)
{
char *BufPtrs[MAX_DEVS];
FT_STATUS ftStatus;
DWORD numDevs = 0;
int i;
	for(i=0; i < MAX_DEVS-1; i++) {
		BufPtrs[i] = (char *)malloc(64);
		assert(BufPtrs[i] != NULL);
	}
	BufPtrs[MAX_DEVS-1] = NULL;		// last entry should be NULL

	ftStatus = FT_ListDevices(BufPtrs,&numDevs,FT_LIST_ALL | FT_OPEN_BY_DESCRIPTION);
	if (ftStatus != FT_OK && numDevs == 0) {
		printf("FT_ListDevices failed\n");
		return 0;
	}

	// FT_ListDevices OK, product descriptions are in Buffer1 and Buffer2, and
	// numDevs contains the number of devices connected

	for(i=0; i < (int)numDevs; i++) {
		printf("%s\n",BufPtrs[i]);
		if(strcmp(desc,BufPtrs[i]) == 0) break;
	}

	if(i >= (int)numDevs) {
		printf("device not found\n");
		exit(1);
	}

	ftStatus = FT_OpenEx(BufPtrs[i],FT_OPEN_BY_DESCRIPTION,&ftHandle);
	if (ftStatus != FT_OK) {
		printf("FT_OpenEx failed\n");
		return 0;
	}

	ftStatus = FT_SetLatencyTimer(ftHandle, 2);
	if (ftStatus != FT_OK) {
		printf("FT_SetLatencyTimer failed\n");
		return 0;
	}

	setBitsMode();
	
	//Set Data Bits Low Byte  0x80, 0xValue, 0xDirection (1 = output)
//	uint8 p[] = { 0x80, 3, 3 };
	uint8 p[] = { 0x80, 0, 3 };
	write(p,3);

	//Set TCK/SK Divisor 0x86, 0xValueL, 0xValueH
	uint8 p1[] = { 0x86, 0, 1 };	//250KHz
//	uint8 p1[] = { 0x86, 0, 10 };	//25KHz
	write(p1,3);

	return 1;
}

//=============================================================================

void Portx::Close()
{
	if(ftHandle) FT_Close(ftHandle);
}

//=============================================================================
/*
uint8 Portx::getBits()
{
uint8 BitMode = 0;
FT_STATUS ftStatus;
	
	ftStatus = FT_GetBitMode(ftHandle, &BitMode);
	if (ftStatus != FT_OK) {
		printf("FT_GetBitMode failed\n");
	}
	return BitMode;
}
*/

//=============================================================================

void Portx::setBitsMode()
{
FT_STATUS ftStatus;
uint8 Mask = 0xff;
UCHAR Mode = 2;		// Set MPSSE mode

	ftStatus = FT_SetBitMode(ftHandle, Mask, Mode);
	if (ftStatus != FT_OK) {
		printf("FT_SetBitMode failed\n");
	}
}

//=============================================================================

void Portx::write(uint8 *b,uint32 len)
{
FT_STATUS ftStatus;
DWORD dwBytesWritten = 0;

	ftStatus = FT_Write(ftHandle, b, len,&dwBytesWritten);
	if (ftStatus != FT_OK || dwBytesWritten != len) {
		printf("FT_Write failed\n");
	}
}

//=============================================================================

void Portx::read(uint8 *b,uint32 len)
{
FT_STATUS ftStatus;
DWORD dwBytesReturned = 0;

	ftStatus = FT_Read(ftHandle, b, len, &dwBytesReturned);
	if (ftStatus != FT_OK || dwBytesReturned != len) {
		printf("FT_Read failed\n");
	}
}

//=============================================================================
//=============================================================================
// control ac0..3

void Portx::setHiBits(uint8 val,uint8 drc)
{
	uint8 cmd[] = { 0x82, val, drc };
	write(cmd,3);
}

void Portx::setLoBits(uint8 val,uint8 drc)
{
	uint8 cmd[] = { 0x80, val, drc };
	write(cmd,3);
}

//=============================================================================

uint8 Portx::sendByte(uint8 val)
{
//	uint8 cmd[] = { 0x3a, 7, val };
	uint8 cmd[] = { 0x37, 7, val };
	write(cmd,3);
	
	uint8 ans = 0;
	read(&ans,1);

	return ans;
}

//=============================================================================



