use std::net::SocketAddr;

pub struct Client {
    id: u8,
    last_packet: u8,
    adress: SocketAddr,
    pos: (i32, i32, i32),
}

impl Client {
    pub fn new(id: u8, last_packet: u8, adress: SocketAddr) -> Self {
        Self {
            id,
            last_packet,
            adress,
            pos: (0, 0, 0),
        }
    }
}
