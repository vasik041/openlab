
#ifndef VIEWSTATE_H
#define VIEWSTATE_H

#include <stdint.h>


class CViewState {
public:
    CViewState() : nLength(0), nOffset(0), bRefreshScreen(true), bRefreshStatus(true), x(0), y(0), mode(0), nibble(0)
    {
    }

    CViewState(CViewState& vs) : nLength(vs.nLength), nOffset(vs.nOffset), bRefreshScreen(vs.bRefreshScreen),
				    bRefreshStatus(vs.bRefreshStatus), x(vs.x), y(vs.y), mode(vs.mode), nibble(vs.nibble)
    {
    }

public:
    uint64_t nLength;
    uint64_t nOffset;
    bool bRefreshScreen;
    bool bRefreshStatus;
    int x;
    int y;
    int mode;
    int nibble;
};

#endif // VIEWSTATE_H
