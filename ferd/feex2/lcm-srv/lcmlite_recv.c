
#include <stdint.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "lcmlite.h"
#include "lcmlite_recv.h"

struct transmit_info
{
    struct sockaddr_in send_addr;
    int send_fd;
};

lcmlite_t lcm;
struct transmit_info tinfo;

int inet_aton(const char *cp, struct in_addr *inp)
{
    struct in_addr a;
    a.S_un.S_addr = inet_addr ( cp );
    if ( a.S_un.S_addr == INADDR_NONE )
    return 0;
    *inp = a;
    return a.S_un.S_addr;
}


void transmit_packet(const void *_buf, int buf_len, void *user)
{
    struct transmit_info *tinfo = (struct transmit_info*) user;

    ssize_t res = sendto(tinfo->send_fd, _buf, buf_len, 0, (struct sockaddr*) &tinfo->send_addr, sizeof(tinfo->send_addr));
    if (res < 0)
        perror("transmit_packet: sendto");
}


static void feex_callback(lcmlite_t *lcm, const char *channel, const void *buf, int buf_len, void *user)
{
    // 8 bytes fingerprint + 1 byte length
    if(buf_len > 9) {
	lcm_recvd((uint8_t *)buf+9,buf_len-9);
    }
}



int lcmlite_main(void)
{
    uint8_t mc_ttl = 1;
    struct in_addr mc_addr;
    int mc_port = htons(7667);

    if (inet_aton("239.255.76.67", (struct in_addr*) &mc_addr) < 0)
        return 1;

    // create the Multicast UDP socket
    struct sockaddr_in read_addr, send_addr;

    memset(&read_addr, 0, sizeof(read_addr));
    read_addr.sin_family = AF_INET;
    read_addr.sin_addr.s_addr = INADDR_ANY;
    read_addr.sin_port = mc_port;

    memset(&send_addr, 0, sizeof(send_addr));
    send_addr.sin_family = AF_INET;
    send_addr.sin_addr = mc_addr;
    send_addr.sin_port = mc_port;

    int read_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (read_fd < 0) {
        perror("socket()");
        return -1;
    }

    int opt = 1;
    if (setsockopt(read_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt(SOL_SOCKET, SO_REUSEADDR)");
//        return -1;
    }

//    if (setsockopt(read_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
//        perror("setsockopt(SOL_SOCKET, SO_REUSEPORT)");
//        return -1;
//    }

    if (bind(read_fd, (struct sockaddr*) &read_addr, sizeof(read_addr)) < 0) {
        perror("bind");
        return -1;
    }

    // join the multicast group
    struct ip_mreq mreq;
    mreq.imr_multiaddr = mc_addr;
    mreq.imr_interface.s_addr = INADDR_ANY;

    if (setsockopt(read_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *)&mreq, sizeof(mreq)) < 0) {
        perror ("setsockopt (IPPROTO_IP, IP_ADD_MEMBERSHIP)");
//        return -1;
    }


    int send_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (setsockopt(send_fd, IPPROTO_IP, IP_MULTICAST_TTL, &mc_ttl, sizeof(mc_ttl)) < 0) {
        perror("setsockopt(IPPROTO_IP, IP_MULTICAST_TTL)");
//        return -1;
    }

    tinfo.send_addr = send_addr;
    tinfo.send_fd = send_fd;
    lcmlite_init(&lcm, transmit_packet, &tinfo);

    // subscribe to LCM messages
    if (1) {
        lcmlite_subscription_t *sub = calloc(1, sizeof(lcmlite_subscription_t));
        sub->channel = "FEEX.CTRL";
        sub->callback = feex_callback;
        sub->user = NULL;
        lcmlite_subscribe(&lcm, sub);
    }

    // read packets, pass them to LCM
    while (1) {
        char buf[1024];
        struct sockaddr_in from_addr; // only IPv4 compatible
        int from_addr_sz = sizeof(from_addr);

        ssize_t buf_len = recvfrom(read_fd, buf, sizeof(buf), 0, (struct sockaddr*) &from_addr, &from_addr_sz);
        assert(from_addr_sz == sizeof(struct sockaddr_in));

        int res = lcmlite_receive_packet(&lcm,
                                         buf,
                                         buf_len,
                                         from_addr.sin_addr.s_addr | ((uint64_t) from_addr.sin_port << 32));
        if (res < 0)
            printf("Error %d\n", res);
    }
    return 0;
}


void lcm_send(uint8_t *buf, int32_t len)
{
uint8_t buf2[64] = {
    0x7a, 0x0d, 0x3e, 0x14, 0x9f, 0x2d, 0x7c, 0xbc, // fingerprint
    len
};
    memcpy(buf2+9,buf,len);
    lcmlite_publish(&lcm, "FEEX.MEAS", buf2, len+9);
}

