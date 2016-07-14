use std::fs::OpenOptions;

const i32 tun_fd;

//////////////////////////////////////////////////////////
//
// Logging
//
//////////////////////////////////////////////////////////
pub struct TunTap {
    file: File,
    name: &str,
    ifr: ifrReq,
}

impl TunTap {
    fn set_if_route(&self, dev: &str, cidr: &str)
    {
        println!("ip route add dev {} {}", dev, cidr)
    }

    fn set_if_address(&self, dev: &str, cidr: &str)
    {
        println!("ip address add dev {} local {}", dev, cidr);
    }

    fn set_if_up(&self, dev: &str)
    {
        println("ip link set dev {}", dev);
    }
    
    pub fn tun_alloc(&self, dev: &str){
        
        let mut ifr: IfReq;
        let err: i32;


        // Open up stream to tun/tap on linux
        let mut fd = try!(OpenOptions::new()
                    .read(true)
                    .write(true)
                    .create(false)
                    .open("/dev/net/tap"));
        
        /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
        *        IFF_TAP   - TAP device
        *
        *        IFF_NO_PI - Do not provide packet information
        */

        

        // copy the char pointer into interface name, with length at most of size IFNAMSIZ
        if(dev){
            ifr = {ifr_name: dev, union: IFF_TUN | IFF_NO_PI};
        } else {
            ifr = {union: IFF_TUN | IFF_NO_PI};
        }

        // Whats happenig here? how do we return both the file pointer and the name of the interface request
        // strcpy(dev, ifr.ifr_name);
        
        ioctl(fd, TUNSETIFF, ifr) match {
        Ok(allgood) => println!("{}", allgood),
        Err(err) => println!("Err: could not ioctl tun: {}", err),  
        };
    }
}