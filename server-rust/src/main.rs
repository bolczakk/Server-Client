use std::collections::HashMap;
use std::net::{SocketAddr, UdpSocket};
use std::sync::mpsc;
use std::time::{Duration, Instant};
use std::{io, thread};

mod client;
use client::Client;

struct Packet {
    pos: (i32, i32, i32),
    len: usize,
}

fn main() -> std::io::Result<()> {
    let socket = UdpSocket::bind("127.0.0.1:8080")?;

    socket.set_nonblocking(true)?;
    //let socket_s = socket.try_clone();

    let mut buf = [0; 24];

    let (tx, rx) = mpsc::channel::<(SocketAddr, Packet)>();

    thread::spawn(move || {
        loop {
            match socket.recv_from(&mut buf) {
                Ok((n, s)) => {
                    if let Some(packet) = parse_input(&buf, n) {
                        let _ = tx.send((s, packet));
                    }
                }
                Err(ref e) if e.kind() == io::ErrorKind::WouldBlock => {
                    thread::sleep(Duration::from_millis(10));
                    thread::yield_now();
                }
                Err(e) => panic!("IO error {e}"),
            };

            // let buf = &mut buf[..num_bytes_read];
            // buf.reverse();
            // socket.send_to(buf, src).unwrap();
        }
    });

    loop {
        if let Ok((addr, pack)) = rx.try_recv() {
            println!("adres: {addr}");
            println!("x: {}", pack.pos.0);
        }
        thread::sleep(Duration::from_millis(10));
    }

    //handle.join().unwrap();
    //Ok(())
}

fn parse_input(data: &[u8], length: usize) -> Option<Packet> {
    if length == 12 {
        let x = i32::from_le_bytes(data[0..4].try_into().unwrap());
        let y = i32::from_le_bytes(data[4..8].try_into().unwrap());
        let z = i32::from_le_bytes(data[8..12].try_into().unwrap());

        let pos = (x, y, z);
        Some(Packet { pos, len: length })
    } else {
        None
    }
}
