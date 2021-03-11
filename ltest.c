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

/***
 * int InitRawSocket(
 * char *device,        // ネットワークインターフェース名
 * int promiscFlag,     // プロミスキャスモードにするかどうかのフラグ
 * int ipOnly           // パケットのみを対象とするかどうかのフラグ
 * )
 ***/

int InitRawSocket(char *device,int promiscFlag, int ipOnly)
{
    struct ifreq    ifreq;
    struct sockaddr_ll  sa;
    int soc;



    /***
     * socket() を使用してデータリンク層を扱うディスクリプタを得る
     ***/
    if(ipOnly){
        if((soc=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_IP)))<0){
            perror("socket");
            return(-1);
        }
    }
    else{
        if((soc=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL)))<0){
            perror("socket");
            return(-1);
        }
    }



    /***
     * ioctl() を使用して、ネットワークインターフェース名に対応した
     * インターフェースのインデックスを得る
    ***/
    memset(&ifreq,0,sizeof(struct ifreq));
    strncpy(ifreq.ifr_name,device,sizeof(ifreq.ifr_name)-1);
    if(ioctl(soc,SIOCGIFINDEX,&ifreq)<0){
        perror("ioctl");
        close(soc);
        return(-1);
    }



    /***
    * ioctl() で取得したインターフェースのインデックス、プロトコルファミリ、
    * プロトコルを sockaddr_ll 構造体にセットし bind() する。
    ***/
   sa.sll_family=PF_PACKET;
   if(ipOnly){
       sa.sll_protocol=htons(ETH_P_IP);
   }
   else{
       sa.sll_protocol=htons(ETH_P_ALL);
   }

   sa.sll_ifindex=ifreq.ifr_ifindex=ifreq.ifr_ifindex;
   if(bind(soc,(struct sockaddr *)&sa,sizeof(sa))<0){
       perror("bind");
       close(soc);
       return(-1);
   }



    /***
    * プロミスキャスモードを有効にする場合、ioctl() で
    * SIOCGIFFLAGS を指定してデバイスのフラグを取得し
    * ifreq.ifr_flags|IFF_PROMISC でビットを1にし
    * SIOCSIFFLAGS を指定した ioctl() で書き込む
    ***/
   if(promiscFlag){
       if(ioctl(soc,SIOCGIFFLAGS,&ifreq)<0){
           perror("ioctl");
           close(soc);
           return(-1);
       }
       ifreq.ifr_flags=ifreq.ifr_flags|IFF_PROMISC;
       if(ioctl(soc,SIOCSIFFLAGS,&ifreq)<0){
           perror("ioctl");
           close(soc);
           return(-1);
       }
   }


   return(soc);
}



/***
 * MAC アドレスを文字列形式に変換する関数
 * char *my_ether_ntoa_r(
 * u_char *hwaddr, 
 * char *buff, 
 * socklen_t size
 * )
 ***/
char *my_ether_ntoa_r(u_char *hwaddr, char *buf, socklen_t size){
    snprintf(buf, size, "%02x:%02x:%02x:%02x:%02x:%02x",
        hwaddr[0],hwaddr[1],hwaddr[2],hwaddr[3],hwaddr[4],hwaddr[5]);

        return(buf);
}



/***
 * Ethernet ヘッダをデバッグ表示するための関数
 * <list>int PrintEthernetHeader(
 * struct ether_header *eh,     Ethernet パケットのヘッダのアドレス
 * FILE *fp                     出力先ファイルポインタ
 * )
 ***/
int PrintEthernetHeader(struct ether_header *eh, FILE *fp)
{
    char    buf[80];

    fprintf(fp, "ether_header------------------------------\n");
    fprintf(fp, "ether_dhost=%s\n", my_ether_ntoa_r(eh->ether_dhost, buf,
                                    sizeof(buf)));
    fprintf(fp, "ether_shost=%s\n", my_ether_ntoa_r(eh->ether_shost, buf,
                                    sizeof(buf)));
    fprintf(fp, "ether_type=%02X", ntohs(eh->ether_type));
    switch (ntohs(eh->ether_type))
    {
    case ETH_P_IP:
        fprintf(fp, "(IP)\n");
        break;
    case ETH_P_IPV6:
        fprintf(fp, "(IPv6)\n");
        break;
    case ETH_P_ARP:
        fprintf(fp, "(ARP)\n");
        break;        
    default:
        fprintf(fp, "(unknown)\n");
        break;
    }

    return(0);
}



/***
 * 起動時の引数にネットワークインターフェース名を指定
 ***/
int main(int argc, char *argv[], char *envp[])
{
    int     soc,size;
    u_char  buf[2048];

    if(argc <= 1){
        fprintf(stderr, "ltest device-name \n");
        return(1);
    }

    if((soc = InitRawSocket(argv[1], 0, 0)) == -1){
        fprintf(stderr, "InitRawSocket:error:%s\n",argv[1]);
        return(-1);
    }

    while (1)
    {
        if(size = read(soc, buf, sizeof(buf)) <= 0){
            perror("read");
        }
        else{
            if(size >= sizeof(struct ether_header)){
                PrintEthernetHeader((struct ether_header *)buf, stdout);
            }
            else{
                fprintf(stderr, "read size(%d) < %ld\n", size, sizeof(struct ether_header));
            }
        }
    }

    close(soc);

    return(0);   
}