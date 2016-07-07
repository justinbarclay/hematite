#include "arp.h"
#include "netdev.h"

// a static array of arp_cache_entries
// It seems stupid to reiterate, but it took me awhile to grok
// this is cached in memory and can only be a certain length
static struct arp_cache_entry arp_cache[ARP_CACHE_LEN];


static int insert_arp_translation_table(struct arp_hdr *hdr, struct arp_ipv4 *data)
{
    // take a pointer the an arp header, and an arp_cache array initialized to the length of ARP_CACHE_LEN
    struct arp_cache_entry *entry;
    
    // move one item from arp_cache into entry
    // 
    for(int i=0; i<ARP_CACHE_LEN; i++) {
        entry = &arp_cache[i];

        // if their is no info in the entry table, set state to resolved, and grab the hardware type from the arp_hdr. Grab IP address of sender from data->sip. Finally copy a number of elements from data->smac = to the size of entry->smac

 
        if(entry->state == ARP_FREE){
            entry->state = ARP_RESOLVED;

            entry->hwtype = hdr->hwtype;
            entry->sip = data->sip;
            memcpy(entry->smac, data->smac, sizeof(entry->smac));
            return 0;
        }
    }
    return -1;
    
}

static int update_arp_translation_table(struct arp_hdr *hdr, struct arp_ipv4 *data)
{
    // Ostensibly, this is updating the cached arp_table but I need to sit and think about this some more
    
    //pointer/reference to an entry
    struct arp_cache_entry *entry;
    
    //
    for(int i=0; i<ARP_CACHE_LEN; i++){
        entry = &arp_cache[i];
        
        // if it's free don't need to update
        if(entry->state == ARP_FREE) continue;

        //if entry matches the arp_hdr and the entry matches matches the sender IP, update entry sender mac address
        if(entry->hwtype == hdr->hwtype && entry->sip == data->sip){
            memcpy(entry->smac, data->smac, 6);
            return 1;
        }
    }

    return 0;
}

void arp_init()
{
    // Init arp_cache to 0
    memset(arp_cache, 0, ARP_CACHE_LEN *sizeof(struct arp_cache_entry));
}

void arp_incoming(struct netdev *netdev, struct eth_hdr *hdr)
{
    struct arp_hdr *arphdr;
    struct arp_ipv4 *arpdata;
    int merge = 0;

    // Type cast hdr->payload as arp_hdr
    arphdr = (struct arp_hdr *) hdr->payload;

    // The ntohs() function converts the unsigned short integer netshort from network byte order to host byte order. (net to host short)

    arphdr->hwtype = ntohs(arphdr->hwtype);
    arphdr->protype = ntohs(arphdr->protype);
    arphdr->opcode = ntohs(arphdr->opcode);

    // sanity checks
    if (arphdr->hwtype != ARP_ETHERNET) {
        printf("Unsupported HW type\n");
        return;
    }

    if(arphdr->protype != ARP_IPV4) {
        printf("Unsupported protocol \n");
    }

    // typecast data into arp_ipv4
    arpdata = (struct arp_ipv4 *) arphdr->data;

    //set data->smac to arp_hdr->smac
    merge = update_arp_translation_table(arphdr, arpdata);


    // self explanatory
    if(netdev->addr != arpdata->dip){
        printf("ARP was not for us \n");
    }

    if(!merge && insert_arp_translation_table(arphdr, arpdata) !=0) {
        perror("ERR: No free space in ARP translation table\n");
    }

    // Do we support network opcode?
    switch(arphdr->opcode) {
        case ARP_REQUEST:
            arp_reply(netdev, hdr, arphdr);
            break;
        default: 
            printf("Opcode not supported\n");
            break;
    }
}

void arp_reply(struct netdev *netdev, struct eth_hdr *hdr, struct arp_hdr * arphdr)
{
    struct arp_ipv4 *arpdata;
    int len;
    
    // grab information we need to reply
    arpdata = (struct arp_ipv4 *) arphdr->data;

    // We're now replying to sender
    memcpy(arpdata->dmac, arpdata->smac, 6);
    arpdata->dip = arpdata->sip;
    
    //set our macaddress as sender
    memcpy(arpdata->smac, netdev->hwaddr, 6);
    arpdata->sip = netdev->addr;

    // names++;
    arphdr->opcode = ARP_REPLY;

    // htons() function makes sure that numbers are stored in memory in network byte order, which is with the most significant byte first. (host-to-network short)
    arphdr->opcode = htons(arphdr->opcode);
    arphdr->hwtype = htons(arphdr->hwtype);
    arphdr->protype = htons(arphdr->protype);
    

    len = sizeof(struct arp_hdr) + sizeof(struct arp_ipv4);
    
    netdev_transmit(netdev, hdr, ETH_P_ARP, len, arpdata->dmac);
}

