
#ifndef HEX_H_INCLUDED
#define HEX_H_INCLUDED


#include "model.h"

class CHexModel : public IModel {

public:
	CHexModel(CFileSource& fs) : IModel(fs)
	{
	}

	~CHexModel()
	{
	}

	uint64_t getNext(uint64_t nPos);
	uint64_t getPrev(uint64_t nPos);

	int getNumElements(void);
	uint64_t getElemOffset(uint64_t nPos,int ix);
	int getElemSize(uint64_t nPos,int ix);
	void getElem(uint64_t nPos,int ix, int mode, std::string& elem);
};


#endif // HEXVIEW_H_INCLUDED

