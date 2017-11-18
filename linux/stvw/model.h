
#ifndef MODEL_H
#define MODEL_H

#include <string>
#include "filesource.h"


class IModel {
public:
	IModel(CFileSource& fs) : fs(fs)
	{
	}

	virtual ~IModel()
	{
	}

public:
	virtual int getNumElements(void) = 0;
	virtual uint64_t getElemOffset(uint64_t nPos,int ix) = 0;
	virtual int getElemSize(uint64_t nPos,int ix) = 0;

	virtual uint64_t getNext(uint64_t nPos) = 0;
	virtual uint64_t getPrev(uint64_t nPos) = 0;

	virtual void getElem(uint64_t nPos,int ix, int mode, std::string& elem) = 0;

protected:
	CFileSource& fs;
};

#endif //MODEL_H
