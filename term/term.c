
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include <stdio.h>
#include <fcntl.h>


#define	ESC	27


struct termios orig_termios;

void reset_terminal_mode()
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()
{
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

//==============================================================================

struct termios oldtio,newtio;
int fd;

int ser_open(char *strPort,int baud)
{
    /* open the device to be non-blocking (read will return immediatly) */
    fd = open(strPort, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
	perror(strPort);
	return -1;
    }
    printf("using %s\n",strPort);
    
    tcgetattr(fd,&oldtio); /* save current port settings */
    memcpy(&newtio, &oldtio, sizeof(newtio));
    cfmakeraw(&newtio);
/*
    newtio.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
    newtio.c_oflag = 0;
    newtio.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    newtio.c_cflag &= ~(CSIZE | PARENB | CRTSCTS);
    newtio.c_cflag |= (CS8 | CLOCAL);
    newtio.c_cc[VMIN]  = 1;
    newtio.c_cc[VTIME] = 0;
*/
    if(cfsetispeed(&newtio, baud) < 0 || cfsetospeed(&newtio, baud) < 0) {
	perror("speed");
    }
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);
    return 0;
}



int ser_rcvd()
{
struct timeval timeout;
fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    /* set timeout value within input loop */
    timeout.tv_usec = 10;  /* milliseconds */
    timeout.tv_sec  = 0;   /* seconds */
    return select(fd+1, &fds, NULL, NULL, &timeout);
}


int ser_recv()
{
char ch=0;
    if(read(fd,&ch,1) != 1) perror("ser_recv");
    return ch;
}


void ser_send(int c)
{
    if(write(fd,&c,1) != 1) perror("ser_send");
}


void ser_close()
{
    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
}

//==============================================================================


int main(int argc, char *argv[])
{
    if(ser_open("/dev/ttyS1",B9600) != 0) exit(-1);
    set_conio_terminal_mode();
    for (;;) {
	if(kbhit()) {
	    int c = getch();
	    if(c == ESC) break;
	    ser_send(c);
	}
	if(ser_rcvd()) {
	    int c = ser_recv();
	    fprintf(stderr,"%c",c);
	}
	usleep(1000);
    }
    ser_close();
    return 0;
}

