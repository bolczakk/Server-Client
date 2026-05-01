#ifndef CLIENT_UDP
#define CLIENT_UDP

#include <asio.hpp>
#include <string>
#include <mutex>
#include <vector>
#include <atomic>

#include "Friend.h"

using asio::ip::udp;

enum { POS_PACKET = 0, PLAYER_JOINED_PACKET, PLAYER_DISC_PACKET, WELCOME_PACKET };

class ClientUdp {
  public:
    ClientUdp(std::string hostname, std::string port);
    void async_send();
    void async_receive();
    void update();
    asio::io_context io_context;
    void allocFriends(std::vector<Friend> &localFriends) const;
    void update_packet(glm::vec3 pos);
    int getMyId() const;

  private:
    bool error = 0;
    asio::steady_timer tick_timer;
    udp::socket s;
    char response[1024];

    udp::resolver resolver;
    udp::endpoint endpoint;

    Packet packet;
    std::mutex packet_mutex;

    std::vector<Friend> online_friends;
    mutable std::mutex friends_mutex;

    std::atomic<int> myId{-1};
};

#endif
