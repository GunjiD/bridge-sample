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



/***
 * チェックサム計算関数
 * u_int16_t checksum(
 * u_char *data, 
 * int len
 * )
***/
u_int16_t checksum(u_char *data, int len)
{
    register u_int32_t  sum;
    register u_int16_t  *ptr;
    register int        c;


    sum = 0;
    ptr = (u_int16_t *)data;


    for(c = len; c > 1; c -= 2){
        sum += (*ptr);
        if(sum & 0x80000000){
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        ptr++;
    }
    if(c == 1){
        u_int16_t   val;
        val = 0;
        memcpy(&val, ptr, sizeof(u_int8_t));
        sum += val;
    }


    while (sum >> 16){
        sum = (sum & 0xFFFF) + (sum >> 16);
    }


    return(~sum);    
}



/***
 * データを2つ渡し、全体のチェックサムを計算するための関数
 * u_int16_t checksum2(
 * u_char *data1, 
 * int len1, 
 * u_char *data2 int len2
 * )
***/
u_int16_t checksum2(u_char *data1, int len1, u_char *data2 int len2)
{
    register u_int32_t  sum;
    register u_int16_t  *ptr;
    register int        c;


    sum = 0;
    ptr = (u_int16_t *)data1;
    for(c = len1; c > 1; c -= 2){
        sum += (*ptr);
        if(sum & 0x80000000){
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        ptr++;
    }
    if(c == 1){
        u_int16_t   val;
        val = ((*ptr) << 8) + (*data2);
        sum += val;
        if(sum & 0x80000000){
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        ptr = (u_int16_t *)(data2 + 1);
        len2--;
    }
    else{
        ptr = (u_int16_t *)data2;
    }

    
    for(c = len2; c > 1; c -= 2){
        sum += (*ptr);
        if(sum & 0x80000000){
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        ptr++;
    }
    if(c == 1){
        u_int16_t   val;
        val = 0;
        memcpy(&val, ptr, sizeof(u_int8_t));
        sum += val;
    }


    while (sum >> 16){
        sum = (sum & 0xFFFF) + (sum >> 16);
    }


    return(~sum);
}



/***
 * IP ヘッダのチェックサムを確認する関数
 * int checkIPchecksum(
 * struct iphdr *iphdr, 
 * u_char *option, 
 * int optionLen
 * )
***/
int checkIPchecksum(struct iphdr *iphdr, u_char *option, int optionLen)
{
    unsigned short  sum;


    if(optionLen == 0){
        sum = checksum((u_char *)iphdr, sizeof(struct iphdr));
        if(sum == 0 || sum == 0xFFFF){
            return(1);
        }
        else{
            return(0);
        }
    }
    else{
        sum = checksum2((u_char *)iphdr, sizeof(struct iphdr), option, optionLen);
        if(sum == 0 || sum == 0xFFFF){
            return(1);
        }
        else{
            return(0);
        }
    }
}



int checkIPDATAchecksum(struct iphdr *iphdr, unsigned char *data, int len);
int checkIP6DATAchecksum(struct ip6_hdr *ip, unsigned char *data, int len);