

#ifndef FILESOURCE_H_INCLUDED
#define FILESOURCE_H_INCLUDED

#include <stdio.h>
#include <stdint.h>


class CFileSource {

public:
	CFileSource();
	~CFileSource();

	bool open(const char *strFileName);
	void close();
	uint8_t operator[](uint64_t nIndex);
	void set(uint64_t nIndex,uint8_t d);
	uint64_t length(void);
private:
	FILE *f;
};


#endif //FILESOURCE_H_INCLUDED
