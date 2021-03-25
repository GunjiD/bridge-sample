int InitRawSocket(char *device, int promiscFlag, int ipOnly);
char *my_ether_nota_r(u_char *hwaddr, char *buf, socklen_t size);
int PrintEthernetHeader(struct ether_header *eh, FILE *fp);