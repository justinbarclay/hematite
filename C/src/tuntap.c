#include "syshead.h"
#include "utils.h"
#include "basic.h"
#include "tuntap.h"

static int tun_fd;

static int set_if_route(char *dev, char *cidr)
{
    return run_cmd("ip route add dev %s %s", dev, cidr);
}

//static int set_if_address(char *dev, char *cidr)
//{
//    return run_cmd("ip address add dev %s local %s", dev, cidr);
//
//}

static int set_if_up(char *dev)
{
    return run_cmd("ip link set dev %s up", dev);
}

/*
 * Taken from Kernel Documentation/networking/tuntap.txt
 */
static int tun_alloc(char *dev)
{
    struct ifreq ifr;
    int fd, err;

    // If unable to open file, return error
    // O_RDWR = Open for reading and writing. The result is undefined if this flag is applied to a FIFO.
    if( (fd = open("/dev/net/tap", O_RDWR)) < 0){
        print_error("Cannot open TUN/TAP dev");
        exit(1);
    }

    // sets to 0
    CLEAR(ifr);

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
     *        IFF_TAP   - TAP device
     *
     *        IFF_NO_PI - Do not provide packet information
     */

    ifr.ifr_flags= IFF_TUN | IFF_NO_PI;

    // copy the char pointer into the interface name, with length at most of size IFNAMSIZ
    if(*dev){
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    //input output control, set the ioctl to TUN/TAP flag, and pass through IFR
    if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
        print_error("ERR: Could not ioctl tun: %s\n", strerror(errno));
        close(fd);
        return err;
    }
    // Whats happenig here
    strcpy(dev, ifr.ifr_name);
    return fd;
}
