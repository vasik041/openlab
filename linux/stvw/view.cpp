
#include <sstream>
#include <string>
#include <iomanip>
#include "view.h"


void CView::getAddr(uint64_t nPos,std::string& addr)
{
	std::stringstream str;
	str << std::setw(8) << std::setfill('0') << std::hex << nPos;
	addr = str.str();
}

int CView::getAddrLen(void)
{
	std::string addr;
	getAddr(0,addr);
	return addr.length();
}


void CView::print1(uint64_t nPos,std::string& line)
{
int i;

	getAddr(nPos,line);
	line.append(" ");

	for(i=0; i < m.getNumElements(); i++) {
		std::string e = "";
		m.getElem(nPos, i, 0, e);
		line.append(e);
		line.append(" ");
	}

	for(i=0; i < m.getNumElements(); i++) {
		std::string e = "";
		m.getElem(nPos, i, 1, e);
		line.append(e);
	}
}


void CView::print(CViewState& vs)
{
int y;
	s.cls();
	status(vs);
	for(uint64_t pos=vs.nOffset,y=0; y < DATA_LINES; y++) {
		std::string line = "";
		print1(pos,line);
		s.printfxy(0,y+HEADER_LINES,"%s",line.c_str());
		uint64_t tmp = m.getNext(pos);
		if(tmp == pos) break;
		pos = tmp;
	}
   	s.sync();
}


void CView::println(CViewState& vs)
{
uint64_t yOffset = getLineOffset(vs);
std::string line;
	print1(yOffset,line);
	s.printfxy(0,vs.y+HEADER_LINES,"%s",line.c_str());
}


void CView::status(CViewState& vs)
{
	uint64_t yOffset = getLineOffset(vs);
	uint64_t xOffset = m.getElemOffset(yOffset,vs.x);
	s.printfxy(0,0,"Addr: %08lx Size: %08lx",xOffset,vs.nLength);
}


int CView::getElemWidth(CViewState& vs)
{
	std::string elem;
	m.getElem(getLineOffset(vs), vs.x, vs.mode, elem);
	return elem.length();
}


int CView::getElemPos(CViewState& vs)
{
int i;
int pos = getAddrLen()+1;
CViewState vs1(vs);

	if(vs.mode == 0) {
		for(i=0; i < vs.x; i++) {
			vs1.x = i;
			pos += getElemWidth(vs1);
			pos++;
		}
		return pos;
	}

	vs1.mode = 0;
	for(i=0; i < m.getNumElements(); i++) {
		vs1.x = i;
		pos += getElemWidth(vs1);
		pos++;
	}
	vs1.mode = 1;
	for(i=0; i < vs.x; i++) {
		vs1.x = i;
		pos += getElemWidth(vs1);
	}
	return pos;
}


uint64_t CView::getLineOffset(CViewState& vs)
{
uint64_t pos = vs.nOffset;
int i;
	for(i=0; i < vs.y; i++) {
		pos = m.getNext(pos);
	}
	return pos;
}


void CView::highlight(CViewState& vs,int b)
{
std::string elem;
uint64_t pos = getLineOffset(vs);
	m.getElem(pos,vs.x,vs.mode,elem);
	if(vs.mode) {
		s.reverse(b);
		s.printfxy(getElemPos(vs),vs.y+HEADER_LINES,"%s",elem.c_str());
		s.reverse(0);
	} else {
		if(vs.nibble == 0) s.reverse(b);
		s.printfxy(getElemPos(vs),vs.y+HEADER_LINES,"%c",elem[0]);
		s.reverse(0);
		if(vs.nibble == 1) s.reverse(b);
		s.printfxy(getElemPos(vs)+1,vs.y+HEADER_LINES,"%c",elem[1]);
		s.reverse(0);
	}
}

