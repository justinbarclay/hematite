// http://hermanradtke.com/2016/03/17/unions-rust-ffi.html

#[repr(C)]
// Struct the maximum size of the ifreq struct's union
// This struct is assuming there are 24 bytes of data
// of unknown type. A lot of magic is down to convert to the proper type
// Because it is network data, all of it is in bigendian, so we start
// looking at bit structures on the left hand side that match
// what we want.
struct IfReqUnion {
    data:[u8; 24],
}

impl IfReqUnion {
    // functions that convert the union to different int types
    fn as_sockaddr(&self) -> sockaddr {
        let mut s = sockaddr {
            // Take an unsigned 8 bit number casts it to an unsigned 16 bit number.
            // It then tries to shift that number 8 bits, if that fails keep the cast number.
            // shifting left only makes sense if when you cast in big endian it follows that endian standard
            // aka: 0b10000000 (1) -> <u16> 0b0000000010000000 (256) << 8 -> 0b1000000000000000(2^16)
            // otherwise: 0b1000
            // Finally convert this from u16 Big Endian to u16 Littl Endian
            sa_family: u16::from_be((self.data[0] as u16) << 8 | (self.data[1] as u16)), 
            sa_data: [0;14], // sa_data is initialized to 14 zeros
        }

        //copy all other info from the union into sa_data
        // basically a memcpy
        for(i, b) in self.data[2..16].iter().enumerate(){
            s.sa_data[i] = *b as i8;
        }
        
        //return socket
        s
    }

    fn as_int(&self) -> c_int {
        //convert from big endian to a c_int
        c_int::from_be((self.data[0] as c_int) << 24 | // if the first byte is a valid number shift all the way to the left
                       (self.data[1] as c_int) << 16 | // if the second byte is valid shift all the way to the left while keeping leading zeroes
                       (self.data[2] as c_int) << 8  |
                       (self.data[3] as c_int))         // stop here because we're not dealing with 64 bit ints. IE in this union the max size of an int is 32 bits.
    }

    fn as_short(&self -> c_short) {
        c_short::from_be((self.data[0] as c_short) << 8 | // remember a short is 16 bits 
        (self.data[1] as c_short))
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Public facing IfReq struct
//
///////////////////////////////////////////////////////////////////////////////
pub struct IfReq {
    ifr_name: [c_char; IFNAMESIZE],
    union: IfReqUnion,
}

impl IfReq {
    // parse the IfReqUnion based on what we think is in there.
    pub fn ifr_hwaddr(&self) -> sockaddr {
        self.union.as_sockaddr()
    }

    pub fn ifr_dstaddr(&self) -> sockaddr{
        self.union.as_sockaddr()
    }

    pub fn ifr_broadaddr(&self) -> c_int {
        self.union.as_int()
    }

    pub fn ifr_media(&self) -> c_int {
        self.union.as_int()
    }

    pub ifr_flags(&self -> c_short){
        self.union.as_short()
    }
}