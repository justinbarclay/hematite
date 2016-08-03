// Some ideas shamelessly stolen from https://github.com/hjr3/carp-rs/blob/master/src/net/mac.rs

use std::fmt;
use std::ffi::CString;
use std::fs::{File, OpenOptions};
use libc::{sockaddr, ioctl, c_int, c_char,c_short, c_ulong};
use nix::sys::socket::{socket, SockType, SockFlag, AddressFamily};
use if_req::IfReq;
use std::io::{Error, Result};
use std::os::unix::prelude::AsRawFd;
use std::io::Write;

const IFF_TUN: c_short = 0x0001;
const IFF_TAP: c_short	= 0x0002;
const IFF_NO_PI: u16 = 0x1000;
const TUNSETIFF: u64 = 0x400454ca;
const IFNAMSIZ: usize = 16;

pub struct TunTap {
    pub file: File,
    typ: String,
    ifr: IfReq,
}

impl TunTap {

    pub fn new(typ: String, name: String) -> TunTap{
         
        //let device = deref_name.as_bytes();
        let c_name: CString = CString::new(name).unwrap();
        let device = c_name.as_bytes_with_nul();
        println!("Length = {}", device.len());
        if device.len() > IFNAMSIZ {
			panic!("Interface name too long, max length is {}", IFNAMSIZ - 1);
		}
        let mut ifr = IfReq{
            ifr_name:{
                let mut buffer = [0u8; IFNAMSIZ];
				buffer[..device.len()].clone_from_slice(device);
                buffer
            },
            union: IFF_TUN,
        };
        let err: i32;

       
        // Open up stream to tun/tap on linux
        let file = match OpenOptions::new().read(true).write(true).open("/dev/net/tun") {
			Err(why) => panic!("Couldn't open tun device '{}': {:?}", "/dev/net/tun", why),
			Ok(file) => file,
		};
        
        /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
        *        IFF_TAP   - TAP device
        *
        *        IFF_NO_PI - Do not provide packet information
        */
        let res = unsafe { ioctl(file.as_raw_fd(), TUNSETIFF, &mut ifr) };
        if res < 0 {
            panic!("{}", Error::last_os_error());
        }

        
        // // Still need to figure out this last part how to do ioctl and what to pass back
        TunTap {
            file: file,
            typ: typ,
            ifr: ifr,
        }
    }
}