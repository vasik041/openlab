
#include <stdlib.h>
#include <errno.h>
#include "filesource.h"


CFileSource::CFileSource() : f(NULL)
{
}

//======================================================================================

CFileSource::~CFileSource()
{
	close();
}

//======================================================================================

bool CFileSource::open(const char *strFileName)
{
	close();
	f = fopen(strFileName,"rb+");
	return f != NULL;
}

//======================================================================================

void CFileSource::close()
{
	if(f) {
		fclose(f);
		f = NULL;
	}
}

//======================================================================================

uint64_t CFileSource::length(void)
{
	if(fseek(f,0,SEEK_END) != 0) {
		printf("fseek failed %d\n",errno);
		exit(1);
	}
	return ftell(f);
}

//======================================================================================

uint8_t CFileSource::operator[](uint64_t nIndex)
{
	if(fseek(f,nIndex,SEEK_SET) != 0) {
		printf("fseek failed %d\n",errno);
		exit(1);
	}
	uint8_t b = 0;
	if(fread(&b,1,1,f) != 1) {
		printf("fread failed %d\n",errno);
		exit(1);
	}
	return b;
}

void CFileSource::set(uint64_t nIndex,uint8_t d)
{
	if(fseek(f,nIndex,SEEK_SET) != 0) {
		printf("fseek failed %d\n",errno);
		exit(1);
	}
	if(fwrite(&d,1,1,f) != 1) {
		printf("fwrite failed %d\n",errno);
		exit(1);
	}
}

//======================================================================================

