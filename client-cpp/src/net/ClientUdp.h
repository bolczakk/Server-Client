#ifndef CLIENT_UDP
#define CLIENT_UDP

#include <asio.hpp>
#include <string>
#include <iostream>

using asio::ip::udp;

struct Packet {
    int x;
    int y;
    int z;
};

class ClientUdp {
  public:
    ClientUdp(std::string hostname, std::string port);
    void async_send();
    void async_receive();
    void update();
    asio::io_context io_context;

  private:
    bool error = 0;
    asio::steady_timer tick_timer;
    udp::socket s;
    char response[1024];

    udp::resolver resolver;
    udp::endpoint endpoint;

    Packet packet;
};

#endif
