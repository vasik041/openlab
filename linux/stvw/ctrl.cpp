
#include "ctrl.h"
#include "view.h"


static int a2x(int c)
{
	if(c >= '0' && c <= '9') return c-'0';
	if(c >= 'A' && c <= 'F') return c-'A'+10;
	if(c >= 'a' && c <= 'f') return c-'a'+10;
	return 0;
}

static bool is_hex(int ch)
{
	return (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f');
}



void CCtrl::next(void)
{
	if(vs.x < m.getNumElements()-1) {
		vs.x++;
	} else {
		vs.x = 0;
		if(vs.y < numLines()-1) {
			vs.y++;
		} else {
			if(vs.nOffset != lastPage) {
				vs.y = 0;
				vs.nOffset = m.getNext(vs.nOffset);
				vs.bRefreshScreen = true;
			}
		}
	}
}


int CCtrl::numLines(void)
{
	if(vs.nOffset != lastPage)
		return DATA_LINES;
	return lastPageLines;
}


void CCtrl::setLastPage(void)
{
int i;
uint64_t page;

	for(page=vs.nOffset; ; ) {
		uint64_t pos = page;
		for(i=0; i < DATA_LINES; i++) {
			uint64_t tmp = m.getNext(pos);
			if(tmp == pos) {
				lastPage = page;
				lastPageLines = i+1;
				return;
			}
			pos = tmp;
		}
		page = pos;
	}
}


uint64_t CCtrl::nextPage(uint64_t pos)
{
int i;
	for(i=0; i < DATA_LINES; i++) {
		pos = m.getNext(pos);
	}
	return pos;
}

uint64_t CCtrl::prevPage(uint64_t pos)
{
int i;
	for(i=0; i < DATA_LINES; i++) {
		pos = m.getPrev(pos);
	}
	return pos;
}


void CCtrl::init(void)
{
	vs.nLength = fs.length();
	vs.nOffset = 0;
	vs.bRefreshScreen = true;
	vs.bRefreshStatus = true;
	vs.x = 0;
	vs.y = 0;
	vs.mode = 0;
	vs.nibble = 0;
	setLastPage();
}


void CCtrl::refresh()
{
    if(vs.bRefreshStatus) {
    	v.status(vs);
       	v.highlight(vs,1);
    	vs.bRefreshStatus = false;
    }
    if(vs.bRefreshScreen) {
       	v.print(vs);
       	v.highlight(vs,1);
       	vs.bRefreshScreen = false;
    }
}


uint64_t CCtrl::getOffset(void)
{
	uint64_t yOffset = v.getLineOffset(vs);
	return  m.getElemOffset(yOffset, vs.x);
}


int CCtrl::loop(int ch)
{
	if(vs.mode) {
		if(ch >= ' ' && ch < 0x7e) {
			v.highlight(vs,0);
			fs.set(getOffset(), ch);
			v.println(vs);
			vs.bRefreshStatus = true;
			next();
			return 0;
		}
	} else {
		if(is_hex(ch)) {
			v.highlight(vs,0);
			if(vs.nibble) {
				fs.set(getOffset(), (fs[getOffset()] & 0xf0) | a2x(ch));
				vs.nibble = 0;
			} else {
				fs.set(getOffset(), (fs[getOffset()] & 0x0f) | (a2x(ch) << 4));
				vs.nibble = 1;
			}
			v.println(vs);
			vs.bRefreshStatus = true;
			if(vs.nibble == 0) next();
			return 0;
		}
	}

	switch(ch) {
		case KEY_HOME:
			if(vs.nOffset != 0) {
				vs.nOffset = 0;
				vs.bRefreshScreen = true;
			}
			break;

		case KEY_END:
			if(vs.nOffset != lastPage) {
				vs.nOffset = lastPage;
				vs.bRefreshScreen = true;
			}
			break;

		case KEY_PPAGE:
			if(vs.nOffset > 0) {
				vs.nOffset = prevPage(vs.nOffset);
				vs.bRefreshScreen = true;
			}
			break;

		case KEY_NPAGE:
			if(vs.nOffset < lastPage) {
				vs.nOffset = nextPage(vs.nOffset);
				vs.bRefreshScreen = true;
			}
			break;

		case KEY_TAB:
				v.highlight(vs,0);
				vs.mode = (vs.mode + 1) & 1;
				vs.bRefreshStatus = true;
				break;

		case KEY_UP:
				if(vs.y > 0) {
					v.highlight(vs,0);
					vs.y--;
					vs.bRefreshStatus = true;
				}
				break;

		case KEY_DOWN:
				if(vs.y < numLines()-1) {
					v.highlight(vs,0);
					vs.y++;
					vs.bRefreshStatus = true;
				}
				break;

		case KEY_LEFT:
				if(vs.x > 0) {
					v.highlight(vs,0);
					vs.x--;
					vs.bRefreshStatus = true;
				}
				break;

		case KEY_RIGHT:
				if(vs.x < m.getNumElements()-1) {
					v.highlight(vs,0);
					vs.x++;
					vs.bRefreshStatus = true;
				}
				break;

		default:
			return ch;
	}
	return 0;
}

