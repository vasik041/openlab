
#include <stdarg.h>
#include "conio.h"


CScreen::CScreen()
{
	initscr();
	clear();
	noecho();
	cbreak();
	curs_set(0);
//	win = newwin(LINES,COLS,0,0);
	keypad(stdscr, TRUE);
}


CScreen::~CScreen()
{
	clear();
	refresh();
	endwin();
}


int CScreen::getchr(void)
{
	return getch();
}


void CScreen::printfxy(int x, int y,const char *format,...)
{
    char *s = NULL;
	va_list args;
	va_start(args,format);
    vasprintf(&s, format, args);
	mvprintw(y, x, "%s", s);
	va_end(args);
}


void CScreen::cls(void)
{
	clear();
}

void CScreen::sync(void)
{
	refresh();
}


void CScreen::reverse(int b)
{
	if(b) wattron(stdscr, A_REVERSE); else  wattroff(stdscr, A_REVERSE);

}


