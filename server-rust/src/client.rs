use std::time::Instant;

pub struct Client {
    pub id: u32,
    pos: (f32, f32, f32),
    pub nickname: String,
    pub last_seen: Instant,
}

impl Client {
    pub fn new(id: u32, new_pos: (f32, f32, f32), nickname: &str) -> Self {
        Self {
            id,
            pos: new_pos,
            nickname: nickname.to_string(),
            last_seen: Instant::now(),
        }
    }
    pub fn update_pos(&mut self, new_pos: &(f32, f32, f32)) {
        self.pos = *new_pos;
    }
    pub fn serialize_data(&self) -> Vec<u8> {
        let mut buf = Vec::new();
        // let first_byte: u8 = 0;

        // buf.extend(first_byte.to_le_bytes());
        buf.extend(self.id.to_le_bytes());
        buf.extend(self.pos.0.to_le_bytes());
        buf.extend(self.pos.1.to_le_bytes());
        buf.extend(self.pos.2.to_le_bytes());
        buf
    }
    pub fn serialize_client(&self) -> Vec<u8> {
        let mut buf = Vec::new();
        // let first_byte: u8 = 1;
        //
        // buf.extend(first_byte.to_le_bytes());
        buf.extend(self.id.to_le_bytes());
        buf.extend(self.nickname.as_bytes());
        buf.push(0);
        buf
    }
    pub fn keep_alive(&mut self) {
        self.last_seen = Instant::now();
    }
}
