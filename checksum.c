#include    <stdio.h>
#include    <string.h>
#include    <unistd.h>
#include    <sys/ioctl.h>
#include    <arpa/inet.h>
#include    <sys/socket.h>
#include    <linux/if.h>
#include    <net/ethernet.h>
#include    <netpacket/packet.h>
#include    <netinet/if_ether.h>
#include    <netinet/ip.h>
#include    <netinet/ip6.h>
#include    <netinet/ip_icmp.h>
#include    <netinet/icmp6.h>
#include    <netinet/tcp.h>
#include    <netinet/udp.h>
#include    "checksum.h"



/***
 * IP, IPv6 ヘッダをそのまま使わず、疑似ヘッダを使う
***/
struct pseudo_ip{
    struct in_addr  ip_src;
    struct in_addr  ip_dst;
    unsigned char   dummy;
    unsigned char   ip_p;
    unsigned char   ip_len;
};

struct pseudo_ip6_hdr{
    struct ip6_addr  src;
    struct ip6_addr  dst;
    unsigned long    plen;
    unsigned short   dmy1;
    unsigned char    dmy2;
    unsigned char    nxt;
};



u_int16_t checksum(u_char *data, int len);
u_int16_t checksum2(u_char *data1, int len1, u_char *data2 int len2);
int checkIPchecksum(struct iphdr *iphdr, u_char *option, int optionLen);
int checkIPDATAchecksum(struct iphdr *iphdr, unsigned char *data, int len);
int checkIP6DATAchecksum(struct ip6_hdr *ip, unsigned char *data, int len);