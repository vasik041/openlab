
#ifndef CONIO_H
#define CONIO_H


#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include <stdio.h>
#include <fcntl.h>


#define	ESC	27


#ifdef __cplusplus
extern "C" {
#endif

void reset_terminal_mode(void);
void set_conio_terminal_mode(void);
int kbhit(void);
int getch(void);


#ifdef __cplusplus
}
#endif


//==============================================================================

#endif //CONIO_H

