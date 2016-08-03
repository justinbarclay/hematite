extern crate libc;
extern crate nix;

mod if_req;
mod tuntap;

use tuntap::TunTap;
fn main() {
    // char buf[BUFLEN];
    // char *dev = calloc(10,1);

    // struct netdev netdev;

    // CLEAR(buf);

    // netdev = netdev_init("10.0.0.4", "00:0c:29:64:50:25");

    // arp_init();

    // while(true){
    //     if(tun_read(buf, BUFLEN) < 0){
    //         print_error("ERR: Read from tun_fd: %s\n", strerror(errno));
    //     }

    //     print_hexdump(buf, BUFLEN);

    //     struct eth_hdr *hdr = init_eth_hdr(buf);

    //     handle_frame(netdev, hdr);
    // }
    let typ: String = String::from("tun");
    let name: String = String::from("tun0");
    let new_tun =  TunTap::new(typ, name);
}
