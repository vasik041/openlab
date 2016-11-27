
#include "port.h"


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
	return -1;
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

