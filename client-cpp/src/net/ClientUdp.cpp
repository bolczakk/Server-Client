#include "ClientUdp.h"
#include <chrono>
#include <iostream>
#include <cstdint>
#include <memory>
#include <system_error>

ClientUdp::ClientUdp(std::string hostname, std::string port)
    : resolver(io_context), s(io_context, udp::endpoint(udp::v4(), 0)), tick_timer(io_context) {
    endpoint = *resolver.resolve(udp::v4(), hostname, port).begin();
    packet.pos.x = 0.f;
    packet.pos.y = 0.f;
    packet.pos.z = 0.f;
}

void ClientUdp::async_send() {
    auto packet_to_send = std::make_shared<Packet>();
    {
        std::lock_guard<std::mutex> lock(packet_mutex);
        *packet_to_send = packet;
    }
    s.async_send_to(asio::buffer(packet_to_send.get(), sizeof(Packet)), endpoint,
                    [this](std::error_code /*ec*/, std::size_t /*bytes_sent*/) {});

    tick_timer.expires_after(std::chrono::milliseconds(10));
    tick_timer.async_wait([this](auto) { async_send(); });
}

void ClientUdp::async_receive() {
    s.async_receive_from(asio::buffer(response, 1024), endpoint, [this](std::error_code ec, std::size_t bytes_recvd) {
        if (!ec && bytes_recvd > 0) {
            if (response[0] == PLAYER_JOINED_PACKET) {
                std::cout << "Odebralem pakiet inicjujacy. Rozmiar: " << bytes_recvd << " bajtow." << std::endl;
                size_t offset = 1;

                while (offset + 4 < bytes_recvd) {
                    uint32_t id;
                    std::memcpy(&id, response + offset, 4);
                    offset += 4;
                    std::string nick(response + offset);

                    offset += nick.length() + 1;

                    std::cout << "Dodano gracza: " << nick << " (ID: " << id << ")" << std::endl;
                    std::lock_guard<std::mutex> lock(friends_mutex);
                    online_friends.emplace_back(nick, id);
                }

            } else if (response[0] == PLAYER_DISC_PACKET) {
                uint32_t id_to_remove;
                std::memcpy(&id_to_remove, response + 1, 4);
                std::lock_guard<std::mutex> lock(friends_mutex);
                for (size_t i = 0; i < online_friends.size(); i++) {
                    if (online_friends[i].id == id_to_remove) {
                        online_friends[i] = online_friends.back();
                        online_friends.pop_back();
                        break;
                    }
                }

            } else if (response[0] == POS_PACKET) {
                std::cout << "Odebralem pakiet z pozycjami graczy. Rozmiar: " << bytes_recvd << std::endl;
                size_t offset = 1;
                while (offset + 16 <= bytes_recvd) {
                    uint32_t id;
                    std::memcpy(&id, response + offset, 4);
                    offset += 4;

                    float x, y, z;
                    std::memcpy(&x, response + offset, 4);
                    offset += 4;
                    std::memcpy(&y, response + offset, 4);
                    offset += 4;
                    std::memcpy(&z, response + offset, 4);
                    offset += 4;
                    // std::cout << "Odebrano od gracza ID " << id << ": X=" << x << " ; Y=" << y << " ; Z=" << z
                    //           << std::endl;
                    {
                        std::lock_guard<std::mutex> lock(friends_mutex);

                        for (auto &f : online_friends) {
                            if (f.id == id) {
                                f.setPos(glm::vec3(x, y, z));
                                break;
                            }
                        }
                    }
                }
            } else if (response[0] == WELCOME_PACKET) {
                int tempId;
                std::memcpy(&tempId, response + 1, 4);
                myId.store(tempId);
                std::cout << "Polaczono z serwerem! Moje ID to: " << myId << std::endl;
            }
        }
        async_receive();
    });
}

void ClientUdp::allocFriends(std::vector<Friend> &localFriends) const {
    std::lock_guard<std::mutex> lock(friends_mutex);
    localFriends = online_friends;
}

void ClientUdp::update_packet(glm::vec3 pos) {
    std::lock_guard<std::mutex> lock(packet_mutex);
    packet.pos = pos;
}

int ClientUdp::getMyId() const { return myId.load(); }
