
#include "terminal.h"
#include "portx.h"

//=============================================================================

#define	DEVICE_DESC	"DLP-2232PB B"


Portx::Portx() : ftHandle(NULL)
{
	if(!Open(DEVICE_DESC)) {
		printf("can\'t open device\n");
	}
}

//=============================================================================

Portx::~Portx()
{
	Close();
}

//=============================================================================

int Portx::Open(const char *desc)
{
char *BufPtrs[3];
char Buffer1[64];
char Buffer2[64];
FT_STATUS ftStatus;
DWORD numDevs;

	BufPtrs[0] = Buffer1;	// initialize the array of pointers
	BufPtrs[1] = Buffer2;
	BufPtrs[2] = NULL; // last entry should be NULL

	ftStatus = FT_ListDevices(BufPtrs,&numDevs,FT_LIST_ALL | FT_OPEN_BY_DESCRIPTION);
	if (ftStatus != FT_OK) {
		printf("FT_ListDevices failed\n");
		return 0;
	}

	// FT_ListDevices OK, product descriptions are in Buffer1 and Buffer2, and
	// numDevs contains the number of devices connected

//	printf("%s %s\n",Buffer1,Buffer2);

	if(strcmp(desc,Buffer2) != 0) {
		printf("device not found\n");
		return 0;
	}

	ftStatus = FT_OpenEx(Buffer2,FT_OPEN_BY_DESCRIPTION,&ftHandle);
	if (ftStatus != FT_OK) {
		printf("FT_OpenEx failed\n");
		return 0;
	}

	ftStatus = FT_SetTimeouts(ftHandle,10,10);
	if (ftStatus != FT_OK) {
		printf("FT_SetTimeouts failed\n");
		return 0;
	}

/*
	ftStatus = FT_SetLatencyTimer(ftHandle, 2);
	if (ftStatus != FT_OK) {
		printf("FT_SetLatencyTimer failed\n");
		return 0;
	}
*/
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

void Portx::setBitsMode(uint8 Mask)
{
FT_STATUS ftStatus;
UCHAR Mode = 2; // Set synchronous bit-bang mode

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

int Portx::read(uint8 *b,uint32 len)
{
FT_STATUS ftStatus;
DWORD dwBytesReturned = 0;

	ftStatus = FT_Read(ftHandle, b, len, &dwBytesReturned);
	if (ftStatus != FT_OK) {
		printf("FT_Read failed\n");
		return 0;
	}
	if(dwBytesReturned != len) {
		return 0;
	}
	return 1;
}

//=============================================================================

int Portx::IsRcvd()
{
FT_STATUS ftStatus;
DWORD dwAmountInRxQueue;
DWORD dwAmountInTxQueue;
DWORD dwEventStatus;

	ftStatus = FT_GetStatus(&ftHandle, &dwAmountInRxQueue,&dwAmountInTxQueue,&dwEventStatus);
	if (ftStatus != FT_OK) {
		printf("FT_GetStatus failed\n");
		return 0;
	}

	return (dwAmountInRxQueue != 0) ? 1 : 0;
}

//=============================================================================



