#include "ClientUdp.h"
#include "asio/error_code.hpp"
#include <system_error>

ClientUdp::ClientUdp(std::string hostname, std::string port)
    : resolver(io_context), s(io_context, udp::endpoint(udp::v4(), 0)), tick_timer(io_context) {
    endpoint = *resolver.resolve(udp::v4(), hostname, port).begin();
    packet.x = 1;
    packet.y = 2;
    packet.z = 3;
}

void ClientUdp::async_send() {
    char request[] = "packet";
    size_t request_length = std::strlen(request);
    s.async_send_to(asio::buffer(&packet, sizeof(packet)), endpoint,
                    [this](std::error_code ec, std::size_t /*bytes_sent*/) {
                        if (ec) {
                            error = 1;
                        }
                    });

    tick_timer.expires_after(std::chrono::seconds(1));
    tick_timer.async_wait([this](auto) { async_send(); });
}

void ClientUdp::async_receive() {
    s.async_receive_from(asio::buffer(response, 1024), endpoint, [this](std::error_code ec, std::size_t bytes_recvd) {
        if (!ec && bytes_recvd > 0) {
            // Przetwórz odebrane dane (np. wypisz w konsoli)
            std::cout << "Odebrano: " << std::string(response, bytes_recvd) << std::endl;
        }
        async_receive();
    });
}
