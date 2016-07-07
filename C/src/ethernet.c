#include "syshead.h"
#include "basic.h"
#include "ethernet.h"

//Return a struct eth_hdr pointer
struct eth_hdr* init_eth_hdr(char* buf)
{
    //typecast the char buf pointer to a eth_hdr pointer
    struct eth_hdr *hdr = (struct eth_hdr *) buf;
    
    //what does htons do?
    //add a buffer
    hdr->ethertype = htons(hdr->ethertype);

    return hdr;
}
