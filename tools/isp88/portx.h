
#ifndef PORTX_H_INCLUDED
#define PORTX_H_INCLUDED

#include "ftd2xx.h"

typedef	unsigned __int8 uint8;
typedef	unsigned __int32 uint32;

#define	MAX_DEVS	(10+1)


#define DEVICE_PIC2232	"DLP-2232PB A"
#define DEVICE_USB2232	"DLP2232M A"


class Portx
{
public:
		Portx(char *desc);
		~Portx();

	void setHiBits(uint8 val,uint8 drc);
	void setLoBits(uint8 val,uint8 drc);

	uint8 sendByte(uint8 val);

private:
	int Open(char *desc);
	void Close();
	void setBitsMode(void);
	void read(uint8 *b,uint32 len);
	void write(uint8 *b,uint32 len);

	FT_HANDLE ftHandle;
};

#endif //PORTX_H_INCLUDED
