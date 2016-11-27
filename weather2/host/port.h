
#ifndef PORT_H
#define PORT_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include <stdio.h>
#include <fcntl.h>

int ser_open(char *strPort,int baud);
int ser_rcvd(void);
int ser_recv(void);
void ser_send(int c);
void ser_close(void);

#endif //PORT_H

