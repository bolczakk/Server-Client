use std::collections::HashMap;
use std::net::{SocketAddr, UdpSocket};
use std::sync::mpsc;
use std::thread;
use std::time::{Duration, Instant};

mod client;
use client::Client;

struct Packet {
    pos: (f32, f32, f32),
}

enum ChangeInPlayers {
    NewPlayer,
    NoNewPlayer,
}

fn main() -> std::io::Result<()> {
    let socket = UdpSocket::bind("0.0.0.0:8080")?;

    // socket.set_nonblocking(true)?;
    let socket_s = socket.try_clone()?;

    let mut buf = [0; 24];

    let mut clients: HashMap<SocketAddr, Client> = HashMap::new();
    let mut next_id: u32 = 1;

    let (tx, rx) = mpsc::channel::<(SocketAddr, Packet)>();

    thread::spawn(move || {
        loop {
            match socket.recv_from(&mut buf) {
                Ok((n, s)) => {
                    if let Some(packet) = parse_input(&buf, n) {
                        if tx.send((s, packet)).is_err() {
                            println!("Kanał (channel) został zamknięty, kończę wątek odbierający.");
                            break;
                        }
                        println!("Pakiet (packet) odebrany od gracza.");
                    }
                }
                Err(e) => {
                    println!("Wystapil blad: {e}");
                    //thread::sleep(Duration::from_millis(10)); //panic!("IO error {e}"),
                    continue;
                }
            };
        }
    });

    let tick_rate = Duration::from_millis(10);
    let tout = Duration::from_secs(5);
    let mut last_tick = Instant::now();

    loop {
        while let Ok((addr, pack)) = rx.try_recv() {
            match update_client(&addr, &mut clients, &pack) {
                ChangeInPlayers::NewPlayer => {
                    let user_string = format!("user{}", next_id);
                    let new_cl = Client::new(next_id, pack.pos, &user_string);

                    let mut new_player_packet: Vec<u8> = vec![1];
                    new_player_packet.extend(new_cl.serialize_client());

                    for client_addr in clients.keys() {
                        let _ = socket_s.send_to(&new_player_packet, client_addr);
                    }

                    clients.insert(addr, new_cl);

                    let mut welcome_packet: Vec<u8> = vec![3u8];
                    welcome_packet.extend(next_id.to_le_bytes());
                    let _ = socket_s.send_to(&welcome_packet, addr);

                    next_id += 1;

                    let full_list = serialize_clients(&clients);
                    let _ = socket_s.send_to(&full_list, addr);
                }
                ChangeInPlayers::NoNewPlayer => {
                    //donothing
                }
            }
        }

        if last_tick.elapsed() >= tick_rate {
            let to_remove: Vec<SocketAddr> = clients
                .iter()
                .filter(|(_, cl)| cl.last_seen.elapsed() > tout)
                .map(|(addr, _)| *addr)
                .collect();

            for addr in to_remove {
                if let Some(c) = clients.remove(&addr) {
                    println!("Usunieto gracza o id: {}", c.id);
                    let mut disconnect_packet = Vec::new();
                    disconnect_packet.push(2u8);
                    disconnect_packet.extend(c.id.to_le_bytes());

                    for other_addr in clients.keys() {
                        let _ = socket_s.send_to(&disconnect_packet, other_addr);
                    }
                }
            }
            let mut snap: Vec<u8> = Vec::new();
            let first_byte: u8 = 0;

            snap.extend(first_byte.to_le_bytes());

            for cl in clients.values() {
                snap.extend(&cl.serialize_data());
            }
            for addr in clients.keys() {
                if socket_s.send_to(&snap, addr).is_ok() {}
            }

            last_tick = Instant::now();
        }
        thread::sleep(Duration::from_millis(5));
    }

    //handle.join().unwrap();
    //Ok(())
}

fn parse_input(data: &[u8], length: usize) -> Option<Packet> {
    if length == 12 {
        let x = f32::from_le_bytes(data[0..4].try_into().unwrap());
        let y = f32::from_le_bytes(data[4..8].try_into().unwrap());
        let z = f32::from_le_bytes(data[8..12].try_into().unwrap());

        let pos = (x, y, z);
        Some(Packet { pos })
    } else {
        None
    }
}

fn update_client(
    addr: &SocketAddr,
    clients: &mut HashMap<SocketAddr, Client>,
    packet: &Packet,
) -> ChangeInPlayers {
    if let Some(c) = clients.get_mut(addr) {
        c.update_pos(&packet.pos);
        c.keep_alive();
        ChangeInPlayers::NoNewPlayer
    } else {
        ChangeInPlayers::NewPlayer
    }
}

fn serialize_clients(clients: &HashMap<SocketAddr, Client>) -> Vec<u8> {
    let mut buf = Vec::new();
    let first_byte: u8 = 1;
    buf.extend(first_byte.to_le_bytes());
    for cl in clients.values() {
        buf.extend(&cl.serialize_client());
    }
    buf
}
