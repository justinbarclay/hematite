#ifndef ETHERNET_H_
#define ETHERNET_H_
#include <linux/if_ether.h>


// Packed struct means that each member of the struct is placed in such a way as to minimize memoy requirements
// To put it another way,  It is used to instruct the GNU C compiler not to optimize the struct memory layout 
// for data alignment with padding bytes
struct eth_hdr
{
    unsigned char dmac[6];
    unsigned char smac[6];
    uint16_t ethertype;
    unsigned char payload[];
} __attribute__((packed));

struct eth_hdr* init_eth_hdr(char* buf);

#endif
