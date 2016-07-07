#ifndef ARP_H
#define ARP_H
#include "syshead.h"
#include "ethernet.h"
#include "netdev.h"

#define ARP_ETHERNET    0x0001
#define ARP_IPV4        0x0800
#define ARP_REQUEST     0x0001
#define ARP_REPLY       0x0002

#define ARP_CACHE_LEN   32
#define ARP_FREE        0
#define ARP_WAITING     1
#define ARP_RESOLVED    2

struct arp_hdr
{
    uint16_t hwtype; // determine link layer used
    uint16_t protype; //protocol type
    unsigned char hwsize; //hardware size
    unsigned char prosize; // protocol size
    uint16_t opcode; // type of ARP Message
    unsigned char data[]; // payload from arp message
} __attribute__((packed));

struct arp_ipv4
{
    unsigned char smac[6]; // sender mac address
    uint32_t sip; // sender IP
    unsigned char dmac[6]; /// destination mac address
    uint32_t dip; // destination IP
} __attribute__((packed));

struct arp_cache_entry
{
    uint16_t hwtype;
    uint32_t sip;
    unsigned char smac[6];
    unsigned int state;
};

void arp_init();
void arp_incoming(struct netdev *netdev, struct eth_hdr *hdr);
void arp_reply(struct netdev *netdev, struct eth_hdr *hdr, struct arp_hdr *arphdr);

unsigned char* arp_get_hwaddre(uint32_t *sip);

#endif
