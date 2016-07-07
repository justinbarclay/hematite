#include "syshead.h"
#include "basic.h"
#include "tuntap.h"
#include "utils.h"
#include "ethernet.h"
#include "arp.h"
#include "netdev.h"

#define BUFLEN 100

void handle_frame(struct netdev *netdev, struct eth_hdr *hdr)
{
    // Check to make sure it's a type we support, and if so send it off to the right function
    switch (hdr->ethertype) {
        case ETH_P_ARP:
            arp_incoming(netdev, hdr);
            break;
        case ETH_P_IP:
            printf("Found IPv4\n");
            break;
        case 0x86dd:
            printf("Found IPv6\n");
            break;
        default:
            printf("Unrecognized ethertype %x\n", hdr->ethertype);
            break;
    }
}

int main(int argc, char** argv){

    char buf[BUFLEN];
    char *dev = calloc(10, 1);

    struct netdev netdev;

    CLEAR(buf);

    tun_init(dev);
    netdev_init(&netdev, "10.0.0.4", "00:0c:29:6d:50:25");

    arp_init();

    while(1) {
        if (tun_read(buf, BUFLEN) < 0) {
            print_error("ERR: Read from tun_fd: %s\n", strerror(errno));
        }

        print_hexdump(buf, BUFLEN);

        struct eth_hdr *hdr = init_eth_hdr(buf);

        handle_frame(&netdev, hdr);
    }
}
