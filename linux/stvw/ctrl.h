
#ifndef CTRL_H
#define CTRL_H

#include "view.h"
#include "filesource.h"


class CCtrl {
public:
	CCtrl(CView& v,IModel& m,CFileSource& fs) : v(v),m(m),fs(fs)
	{
		init();
	}

	~CCtrl()
	{
	}

	void refresh(void);
	int loop(int ch);

private:
	void init(void);
	uint64_t nextPage(uint64_t pos);
	uint64_t prevPage(uint64_t pos);
	void next(void);
	int numLines(void);
	void setLastPage(void);
	uint64_t getOffset(void);

private:
	IModel& m;
	CView& v;
	CFileSource& fs;
	uint64_t lastPage;
	uint64_t lastPageLines;
	CViewState vs;
};


#endif //CTRL_H


