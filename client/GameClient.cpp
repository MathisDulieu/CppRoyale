#include "GameClient.hpp"
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

void GameClient::receivePackets() {
    sf::Packet packet;
    while (m_socket.receive(packet) == sf::Socket::Status::Done) {
        uint8_t packetType;
        packet >> packetType;

        if (packetType == PKT_START) {
            uint8_t assignedId;
            packet >> assignedId;
            m_playerId = assignedId;
            m_state = ClientState::Playing;
            std::cout << "[Client] Game started — player " << (int) m_playerId << "\n";
        } else if (packetType == PKT_GAME_STATE) {
            uint32_t tick;
            packet >> tick;

            for (uint8_t playerId = 0; playerId < MAX_PLAYERS; ++playerId)
                packet >> m_elixir[playerId];

            uint16_t entityCount;
            packet >> entityCount;
            m_entities.clear();
            for (uint16_t index = 0; index < entityCount; ++index) {
                EntitySnapshot snapshot;
                uint8_t troopTypeByte;
                packet >> snapshot.entityId
                        >> troopTypeByte
                        >> snapshot.x
                        >> snapshot.y
                        >> snapshot.hp
                        >> snapshot.ownerId;
                snapshot.troopType = static_cast<TroopType>(troopTypeByte);
                m_entities.push_back(snapshot);
            }

            uint8_t towerCount;
            packet >> towerCount;
            m_towers.clear();
            for (uint8_t index = 0; index < towerCount; ++index) {
                TowerSnapshot snapshot;
                uint8_t isNexusByte;
                packet >> snapshot.towerId
                        >> snapshot.x
                        >> snapshot.y
                        >> snapshot.hp
                        >> snapshot.ownerId
                        >> isNexusByte;
                snapshot.isNexus = (isNexusByte == 1);
                m_towers.push_back(snapshot);
            }
        } else if (packetType == PKT_GAME_OVER) {
            uint8_t winnerId;
            packet >> winnerId;
            m_winnerId = winnerId;
            m_state = ClientState::GameOver;
            std::cout << "[Client] Game over — winner: player " << (int) winnerId << "\n";
        }
    }
}

void GameClient::deployTroop(TroopType troopType, float x, float y) {
    sf::Packet packet;
    packet << PKT_DEPLOY
            << static_cast<uint8_t>(troopType)
            << x
            << y
            << m_playerId;
    (void) m_socket.send(packet);
}
