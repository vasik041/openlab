
#ifndef COMMPORT_H
#define COMMPORT_H

int	port_open(int port,int baud);
void	port_close(void);
int	port_send(int b);
int	port_recv(void);
int	port_recv_wait(int to);
int	port_clear(int n);

void	SetDTR(void);
void	ClrDTR(void);

#endif // COMMPORT_H
