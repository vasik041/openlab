
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/if_arp.h>
#include <linux/if_tun.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <sys/time.h>


int open_ethif(const char *ifname)
{
struct ifreq ifr;
struct sockaddr_ll sll;
struct packet_mreq mreq;
int flags;
int sock;
int ifindex;
    if((sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
	perror("socket");
	goto fail;
    }

    memset(&ifr, 0, sizeof ifr);
    snprintf(ifr.ifr_name, sizeof ifr.ifr_name, "%s", ifname);

    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
	perror("ioctl(SIOCGIFINDEX)");
	goto fail;
    }
    ifindex = ifr.ifr_ifindex;

    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
      perror("ioctl(SIOCGIFHWADDR)");
      goto fail;
    }
    if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER) {
	perror("%s: not an ethernet interface");
	goto fail;
    }
    if (IFHWADDRLEN != 6) {
      printf("%s: bad hardware address length", ifname);
      goto fail;
    }

    if (ioctl(sock, SIOCGIFMTU, &ifr) < 0)  {
      perror("ioctl(SIOCGIFMTU)");
      goto fail;
    }

    memset(&sll, 0, sizeof sll);
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifindex;
    if (bind(sock, (void *)&sll, sizeof sll) < 0)  {
      perror("bind");
      goto fail;
    }

    /* enable promiscuous reception */
    memset(&mreq, 0, sizeof mreq);
    mreq.mr_ifindex = ifindex;
    mreq.mr_type = PACKET_MR_PROMISC;
    if (setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mreq, sizeof mreq) < 0) {
      perror("setsockopt");
      goto fail;
    }

    return sock;

fail:
    close(sock);
    return -1;
}


void usage()
{
    printf("Usage:\n raw -s|-r iface\n");
    exit(1);
}


char arp[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x02, 0x2a, 0xf3, 0xd0, 0x11, 0x49,
    0x08, 0x06,
    0x00, 0x01,
    0x08, 0x00, 0x06, 0x04, 0x00, 0x01,

    0x02, 0x2a, 0xf3, 0xd0, 0x11, 0x49,
    0xac, 0x10, 0x00, 0x2a,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc0, 0x10, 0x00, 0x2b
};


int main(int argc,char **argv)
{
int sock;
int i;
int client;
char buffer[4096];

    if(argc != 3) usage();

    if(strcmp(argv[1],"-r") == 0) client = 1;
    else if(strcmp(argv[1],"-s") == 0) client = 0;
    else usage();

    sock = open_ethif(argv[2]);
    if(sock < 0) {
	perror("Open interface");
	exit(1);
    }	

    if(client) {
	for(;;) {
    	    int nread = recv(sock,buffer,sizeof(buffer),0);
            if(nread < 0) {
    		perror("Reading from interface");
		break;
    	    }	

    	    printf("Read %d bytes from device %s ", nread, argv[2]);
    	    for(i=0; i < 42 && i < nread; i++) {
		printf("%c",(buffer[i] >= ' ') ? buffer[i] : '.');
    	    }
    	    printf(" ");
    	    for(i=0; i < 42 && i < nread; i++) {
		printf("%02x ",buffer[i] & 0xff);
    	    }
    	    printf("\n");
        }
    } else {
	for(;;) {
	    int nwrite = send(sock,arp,42,0);
            if(nwrite < 0) {
    		perror("Writing to interface");
		break;
    	    }
	    printf(".");
	    usleep(1000*1000);
	}
    }

    close(sock);
    return 0;
}

