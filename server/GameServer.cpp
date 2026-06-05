#include "GameServer.hpp"
#include "Packets.hpp"
#include "Constants.hpp"
#include <iostream>

GameServer::GameServer(unsigned short port) {
    if (m_listener.listen(port) != sf::Socket::Status::Done) {
        std::cerr << "[Server] Cannot bind port " << port << "\n";
        m_running = false;
        return;
    }
    m_selector.add(m_listener);
    std::cout << "[Server] Listening on port " << port << "\n";
}

void GameServer::run() {
    sf::Clock clock;
    float accumulator = 0.f;

    while (m_running) {
        accumulator += clock.restart().asSeconds();

        if (m_selector.wait(sf::milliseconds(5))) {
            acceptClients();
            receivePackets();
        }

        while (accumulator >= TICK_DUR) {
            broadcastPong();
            accumulator -= TICK_DUR;
        }
    }
}

void GameServer::acceptClients() {
    if (!m_selector.isReady(m_listener)) return;

    auto socket = std::make_unique<sf::TcpSocket>();
    if (m_listener.accept(*socket) == sf::Socket::Status::Done) {
        auto addr = socket->getRemoteAddress();
        std::cout << "[Server] Client connected: " << (addr ? addr->toString() : "unknown") << "\n";
        m_selector.add(*socket);
        m_clients.push_back(std::move(socket));
    }
}

void GameServer::receivePackets() {
    for (auto& socket : m_clients) {
        if (!m_selector.isReady(*socket)) continue;

        sf::Packet packet;
        if (socket->receive(packet) == sf::Socket::Status::Done) {
            uint8_t type;
            packet >> type;
            if (type == PKT_PING)
                std::cout << "[Server] PING received\n";
        }
    }
}

void GameServer::broadcastPong() {
    sf::Packet packet;
    uint8_t type = PKT_PONG;
    packet << type;

    for (auto& socket : m_clients)
        (void)socket->send(packet);
}