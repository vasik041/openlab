
#ifndef PORTX_H_INCLUDED
#define PORTX_H_INCLUDED

#include "ftd2xx.h"



class Portx
{

public:
	Portx();
	~Portx();

	int Open(const char *desc);
	void Close();

	int read(uint8 *b,uint32 len);
	void write(uint8 *b,uint32 len);
	int IsRcvd();

	void setBitsMode(uint8 Mask);
private:
	FT_HANDLE ftHandle;
};

#endif //PORTX_H_INCLUDED