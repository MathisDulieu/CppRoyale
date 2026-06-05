#include "GameServer.hpp"
#include "Packets.hpp"
#include "Constants.hpp"
#include <iostream>

GameServer::GameServer(unsigned short port)
    : m_serverState(ServerState::Waiting)
      , m_running(true)
      , m_startPending(false)
      , m_startPendingTicks(0) {
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
            if (m_serverState == ServerState::Waiting)
                acceptClients();
            receivePackets();
        }

        while (accumulator >= TICK_DURATION) {
            if (m_startPending && --m_startPendingTicks <= 0) {
                broadcastStart();
                m_startPending = false;
            }

            if (m_serverState == ServerState::Playing) {
                m_gameState.update();
                broadcastGameState();

                GameResult result = m_gameState.getResult();
                if (result != GameResult::Ongoing) {
                    uint8_t winnerId = (result == GameResult::Player0Wins) ? 0 : 1;
                    broadcastGameOver(winnerId);
                    m_serverState = ServerState::GameOver;
                }
            }

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
    socket->setBlocking(false);
    m_selector.add(*socket);
    m_clients.push_back({std::move(socket), playerId});

    std::cout << "[Server] Assigned player ID " << (int) playerId
            << " (" << m_clients.size() << "/" << MAX_PLAYERS << ")\n";

    if (static_cast<int>(m_clients.size()) == MAX_PLAYERS) {
        std::cout << "[Server] Lobby full — starting game\n";
        m_startPending = true;
        m_startPendingTicks = 10;
    }
}

void GameServer::receivePackets() {
    for (auto &slot: m_clients) {
        if (!m_selector.isReady(*slot.socket)) continue;

        sf::Packet packet;
        while (slot.socket->receive(packet) == sf::Socket::Status::Done) {
            uint8_t packetType;
            packet >> packetType;

            if (packetType == PKT_DEPLOY)
                processDeployPacket(packet, slot.playerId);
        }
    }
}

void GameServer::broadcastStart() {
    for (auto &slot: m_clients) {
        sf::Packet packet;
        packet << PKT_START << slot.playerId;
        auto status = slot.socket->send(packet);
        std::cout << "[Server] PKT_START sent to player " << (int) slot.playerId
                << " status=" << (int) status << "\n";
    }
    m_serverState = ServerState::Playing;
}

void GameServer::broadcastGameState() {
    const auto &entities = m_gameState.getEntities();
    const auto &towers = m_gameState.getTowers();

    sf::Packet packet;
    packet << PKT_GAME_STATE;
    packet << m_gameState.getTick();

    for (uint8_t playerId = 0; playerId < MAX_PLAYERS; ++playerId)
        packet << m_gameState.getElixir(playerId);

    packet << static_cast<uint16_t>(entities.size());
    for (const auto &entity: entities) {
        packet << entity->getEntityId()
                << static_cast<uint8_t>(entity->getTroopType())
                << entity->getX()
                << entity->getY()
                << entity->getHp()
                << entity->getOwnerId();
    }

    packet << static_cast<uint8_t>(towers.size());
    for (const auto &tower: towers) {
        packet << tower.getTowerId()
                << tower.getX()
                << tower.getY()
                << tower.getHp()
                << tower.getOwnerId()
                << static_cast<uint8_t>(tower.isNexus() ? 1 : 0);
    }

    for (auto &slot: m_clients)
        (void) slot.socket->send(packet);
}

void GameServer::broadcastGameOver(uint8_t winnerPlayerId) {
    sf::Packet packet;
    packet << PKT_GAME_OVER << winnerPlayerId;
    for (auto &slot: m_clients)
        (void) slot.socket->send(packet);
    std::cout << "[Server] Game over — player " << (int) winnerPlayerId << " wins\n";
}

void GameServer::processDeployPacket(sf::Packet &packet, uint8_t playerId) {
    uint8_t troopTypeByte;
    float x;
    float y;
    uint8_t senderPlayerId;

    packet >> troopTypeByte >> x >> y >> senderPlayerId;
    if (senderPlayerId != playerId) return;

    m_gameState.spawnTroop(static_cast<TroopType>(troopTypeByte), x, y, playerId);
    std::cout << "[Server] Player " << (int) playerId
            << " deployed troop " << (int) troopTypeByte
            << " at (" << x << ", " << y << ")\n";
}
