
#ifndef VIEW_H
#define VIEW_H

#include "conio.h"
#include "model.h"
#include "viewstate.h"

#define	DATA_LINES	32
#define HEADER_LINES 3


class CView {
public:
	CView(CScreen& s,IModel& m) : s(s),m(m)
	{
	}

	void print(CViewState& vs);
	void println(CViewState& vs);
	void status(CViewState& vs);
	void highlight(CViewState& vs,int b);
	uint64_t getLineOffset(CViewState& vs);

private:
	void getAddr(uint64_t nPos,std::string& addr);
	int getAddrLen(void);
	int getElemWidth(CViewState& vs);
	int getElemPos(CViewState& vs);
	void print1(uint64_t nPos,std::string& line);

private:
	CScreen& s;
	IModel& m;
};


#endif //VIEW_H
