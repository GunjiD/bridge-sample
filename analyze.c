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
#include    "print.h"



/***
 * ARP の解析関数
 * int AnalyzeArp(
 * u_char *data,
 * int size
 * )
 ***/
int AnalyzeArp(u_char *data, int size)
{
    u_char *ptr;
    int     lest;
    struct ether_arp    *arp;


    ptr = data;
    lest = size;


    if(lest < sizeof(struct ether_arp)){
        fprintf(stderr, "lest(%d) < sizeof(struct iphdr)\n", lest);
        return(-1);
    }
    arp = (struct ether_arp *)ptr;
    ptr += sizeof(struct ether_arp);
    lest -= sizeof(struct ether_arp);


    PrintArp(arp, stdout);

    return(0);
}