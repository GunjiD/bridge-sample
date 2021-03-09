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
    * プロトコルを sockadd_ll 構造体にセットし bind() する。
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
    * 
    ***/
}