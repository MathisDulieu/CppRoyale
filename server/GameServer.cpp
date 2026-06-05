#include "GameServer.hpp"
#include "Packets.hpp"
#include "Constants.hpp"
#include <iostream>

GameServer::GameServer(unsigned short port)
    : m_state(ServerState::Waiting)
      , m_running(true) {
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
            if (m_state == ServerState::Waiting)
                acceptClients();
            receivePackets();
        }

        while (accumulator >= TICK_DURATION) {
            tick();
            accumulator -= TICK_DURATION;
        }
    }
}

void GameServer::acceptClients() {
    if (!m_selector.isReady(m_listener)) return;
    if (static_cast<int>(m_clients.size()) >= MAX_PLAYERS) return;

    auto socket = std::make_unique<sf::TcpSocket>();
    if (m_listener.accept(*socket) != sf::Socket::Status::Done) return;

    auto address = socket->getRemoteAddress();
    std::cout << "[Server] Client connected: "
            << (address ? address->toString() : "unknown") << "\n";

    uint8_t playerId = static_cast<uint8_t>(m_clients.size());

    sf::Packet assignPacket;
    assignPacket << PKT_PING << playerId;
    (void) socket->send(assignPacket);

    m_selector.add(*socket);
    m_clients.push_back({std::move(socket), playerId, false});

    std::cout << "[Server] Assigned player ID " << (int) playerId
            << " (" << m_clients.size() << "/" << MAX_PLAYERS << ")\n";

    if (static_cast<int>(m_clients.size()) == MAX_PLAYERS) {
        std::cout << "[Server] Lobby full — starting game\n";
        broadcastStart();
        m_state = ServerState::Playing;
    }
}

void GameServer::receivePackets() {
    for (auto &slot: m_clients) {
        if (!m_selector.isReady(*slot.socket)) continue;

        sf::Packet packet;
        if (slot.socket->receive(packet) != sf::Socket::Status::Done) continue;

        uint8_t packetType;
        packet >> packetType;

        if (packetType == PKT_PING)
            std::cout << "[Server] Ping from player " << (int) slot.playerId << "\n";
    }
}

void GameServer::broadcastStart() {
    for (auto &slot: m_clients) {
        sf::Packet packet;
        packet << PKT_START << slot.playerId;
        (void) slot.socket->send(packet);
    }
    std::cout << "[Server] PKT_START sent to all players\n";
}

void GameServer::tick() {
    if (m_state != ServerState::Playing) return;
}
