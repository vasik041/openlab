
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h> 
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <stdarg.h>


void usage()
{
    printf("Usage:\n tap -s|-r iface\n");
    exit(1);
}


int main(int argc,char **argv)
{
char tun_name[IFNAMSIZ] = {0};
int tun_fd = -1;
int nread = 0;
int nwrite = 0;
int i = 0;
int rc = 0;
char buffer[2048] = "Hellow MaxSec!";
int bClient = 0;

    if(argc != 3) usage();

    if(strcmp(argv[1],"-r") == 0) bClient = 1;
    else if(strcmp(argv[1],"-s") == 0) bClient = 0;
    else usage();

    strcpy(tun_name, argv[2]);

    tun_fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
//    tun_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(tun_fd < 0) {
	perror("Creating socket");
	exit(1);
    }

    rc = setsockopt(tun_fd, SOL_SOCKET, SO_BINDTODEVICE, tun_name, strlen(tun_name));
    if(rc < 0) {
	perror("Binding to iface");
	exit(1);
    }

    if(bClient) {
/*
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999);
	addr.sin_addr.s_addr = inet_addr("172.16.0.6");

	rc = bind(tun_fd, (struct sockaddr *)&addr, sizeof(addr));
        if(rc < 0) {
	    perror("Binding socket");
	    exit(1);
	}
*/
	while(1) {
	    struct sockaddr_in addr1;
	    int len = sizeof(addr1);
//	    nread = recvfrom(tun_fd,buffer,sizeof(buffer),0,(struct sockaddr *)&addr1, &len);
	    nread = recv(tun_fd,buffer,sizeof(buffer),0);
	    if(nread < 0) {
    		perror("Reading from interface");
    		close(tun_fd);
    		exit(1);
	    }	

	    printf("Read %d bytes from device %s ", nread, tun_name);
	    for(i=0; i < 16 && i < nread; i++) {
		printf("%c",(buffer[i] >= ' ') ? buffer[i] : '.');
	    }
	    printf(" ");
	    for(i=0; i < 16 && i < nread; i++) {
		printf("%02x ",buffer[i] & 0xff);
	    }
	    printf("\n");
	 }
    } else {

	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(9999);
	sin.sin_addr.s_addr = inet_addr("172.16.0.43");

	while(1) {
	    nwrite = sendto(tun_fd,buffer,strlen(buffer),0,(struct sockaddr *)&sin, sizeof(struct sockaddr));
	    if(tun_fd < 0) {
		perror("Writing to interface");
		exit(1);
	    }

	    printf("sent %d bytes to %s\n",nwrite,tun_name);
	    usleep(1000*1000);
	}
    }
    close(tun_fd);
    return 0;
}

