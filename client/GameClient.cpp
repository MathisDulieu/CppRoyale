#include "GameClient.hpp"
#include "Packets.hpp"
#include <iostream>

bool GameClient::connect(const std::string &host, unsigned short port) {
    m_socket.setBlocking(false);

    auto address = sf::IpAddress::resolve(host);
    if (!address) {
        std::cerr << "[Client] Cannot resolve host\n";
        return false;
    }

    auto status = m_socket.connect(*address, port, sf::seconds(5));
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
    packet << PKT_PING;
    (void) m_socket.send(packet);
}

void GameClient::receivePackets() {
    sf::Packet packet;
    while (m_socket.receive(packet) == sf::Socket::Status::Done) {
        uint8_t packetType;
        packet >> packetType;

        if (packetType == PKT_PING) {
            uint8_t assignedId;
            packet >> assignedId;
            m_playerId = assignedId;
            std::cout << "[Client] Assigned player ID " << (int) m_playerId << "\n";
        } else if (packetType == PKT_START) {
            uint8_t assignedId;
            packet >> assignedId;
            m_playerId = assignedId;
            m_state = ClientState::Playing;
            std::cout << "[Client] Game started — player " << (int) m_playerId << "\n";
        }
    }
}
