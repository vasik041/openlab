
#include <sstream>
#include <iomanip>
#include "main.h"
#include "hex.h"



static int tochr(int ch)
{
	if(ch < ' ' || ch >= 0x7f) return '.';
	return ch;
}


int CHexModel::getNumElements(void)
{
	return 16;
}


uint64_t CHexModel::getNext(uint64_t nPos)
{
	if(nPos+16 >= fs.length()) return nPos;
	return nPos+16;
}


uint64_t CHexModel::getPrev(uint64_t nPos)
{
	if(nPos < 16) return 0;
	return nPos-16;
}


uint64_t CHexModel::getElemOffset(uint64_t nPos,int ix)
{
	return nPos + ix;
}

int CHexModel::getElemSize(uint64_t nPos,int ix)
{
	return 1;
}

void CHexModel::getElem(uint64_t nPos,int ix, int mode, std::string& elem)
{
	if(mode == 0) {
		if(nPos+ix < fs.length()) {
			std::stringstream str;
			str << std::setw(2) << std::setfill('0') << std::hex << (uint16_t)fs[nPos+ix];
			elem = str.str();
		} else {
			elem = "  ";
		}
	} else {
		if(nPos+ix < fs.length()) {
			elem = tochr(fs[nPos+ix]);
		} else {
			elem = " ";
		}
	}
}

