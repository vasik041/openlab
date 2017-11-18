
#ifndef CONIO_H
#define CONIO_H

#include <ncurses.h>

#define KEY_ESC	27
#define KEY_TAB 9


class CScreen {
public:
	CScreen();
	~CScreen();

public:
	int getchr(void);
	void cls(void);
	void printfxy(int x, int y,const char *format, ...);
	void sync(void);
	void reverse(int b);
private:
//	WINDOW *win;
};


#endif //CONIO_H

