struct eth_hdr{
    dmac: [u8; 6],
    smac: [u8; 6],
    etherType: u16,
    payload: Vec<u8>),
}

struct eth_hdr init_eth_hdr(char buf){
    struct eth_hdr *hdr = (struct eth_hdr *) buf;

        hdr.etherType = htons(hdr->ethertype);
        return hdr;
}

fn initEthernetHeader(buf: Vec<?>){
    let ethHeader = {dmac: buf}
}