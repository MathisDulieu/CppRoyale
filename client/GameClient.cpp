#include "GameClient.hpp"
#include "Packets.hpp"
#include <iostream>

bool GameClient::connect(const std::string& host, unsigned short port) {
    m_socket.setBlocking(false);
    auto addr = sf::IpAddress::resolve(host);
    if (!addr) {
        std::cerr << "[Client] Cannot resolve host\n";
        return false;
    }
    auto status = m_socket.connect(*addr, port, sf::seconds(5));
    if (status != sf::Socket::Status::Done &&
        status != sf::Socket::Status::NotReady) {
        std::cerr << "[Client] Connection failed\n";
        return false;
        }
    std::cout << "[Client] Connected to " << host << ":" << port << "\n";
    return true;
}

void GameClient::sendPing() {
    sf::Packet packet;
    uint8_t type = PKT_PING;
    packet << type;
    (void)m_socket.send(packet);
}

bool GameClient::receivePong() {
    sf::Packet packet;
    if (m_socket.receive(packet) == sf::Socket::Status::Done) {
        uint8_t type;
        packet >> type;
        return type == PKT_PONG;
    }
    return false;
}