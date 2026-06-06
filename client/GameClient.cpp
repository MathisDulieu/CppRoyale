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

void GameClient::sendName(const std::string& name) {
    sf::Packet packet;
    packet << PKT_SET_NAME << name;
    (void)m_socket.send(packet);
    m_name = name;
}

void GameClient::sendFindMatch() {
    sf::Packet packet;
    packet << PKT_FIND_MATCH;
    (void) m_socket.send(packet);
    m_state = ClientState::Searching;
}

void GameClient::sendCancelMatch() {
    sf::Packet packet;
    packet << PKT_CANCEL_MATCH;
    (void) m_socket.send(packet);
    m_state = ClientState::Idle;
}

void GameClient::sendChallenge(uint8_t targetId) {
    sf::Packet packet;
    packet << PKT_CHALLENGE << targetId;
    (void) m_socket.send(packet);
    m_state = ClientState::ChallengeSent;
}

void GameClient::sendChallengeResponse(bool accepted) {
    sf::Packet packet;
    packet << PKT_CHALLENGE_RESP << static_cast<uint8_t>(accepted ? 1 : 0);
    (void) m_socket.send(packet);
    m_state = accepted ? ClientState::Idle : ClientState::Idle;
}

void GameClient::sendCancelChallenge() {
    sf::Packet packet;
    packet << PKT_CANCEL_CHALLENGE;
    (void) m_socket.send(packet);
    m_state = ClientState::Idle;
}

void GameClient::requestPlayerList() {
    sf::Packet packet;
    packet << PKT_PLAYER_LIST;
    (void) m_socket.send(packet);
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

void GameClient::sendReturnToLobby() {
    sf::Packet packet;
    packet << PKT_RETURN_TO_LOBBY;
    (void) m_socket.send(packet);
    m_state = ClientState::Idle;
    m_winnerId = 255;
    m_entities.clear();
    m_towers.clear();
}

void GameClient::tickMatchFoundTimer(float delta) {
    if (m_state != ClientState::MatchFound) return;
    m_matchFoundTimer -= delta;
    if (m_matchFoundTimer <= 0.f) {
        m_matchFoundTimer = 0.f;
        m_state = ClientState::Playing;
    }
}

void GameClient::receivePackets() {
    sf::Packet packet;
    while (m_socket.receive(packet) == sf::Socket::Status::Done) {
        uint8_t packetType;
        packet >> packetType;

        if (packetType == PKT_MATCH_FOUND) {
            uint8_t assignedPlayerId;
            std::string opponentName;
            packet >> assignedPlayerId >> opponentName;
            m_playerId = assignedPlayerId;
            m_opponentName = opponentName;
            m_state = ClientState::MatchFound;
            m_matchFoundTimer = 3.f;
        } else if (packetType == PKT_GAME_STATE) {
            uint32_t tick;
            uint8_t isOvertimeByte;
            packet >> tick >> m_remainingTime >> isOvertimeByte;
            m_isOvertime = (isOvertimeByte == 1);

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
        } else if (packetType == PKT_PLAYER_LIST) {
            uint8_t playerCount;
            packet >> playerCount;
            m_playerList.clear();
            for (uint8_t index = 0; index < playerCount; ++index) {
                PlayerInfo info;
                uint8_t isSearching;
                uint8_t hasPending;
                uint8_t isInGame;
                packet >> info.clientId
                        >> info.name
                        >> isSearching
                        >> hasPending
                        >> isInGame;
                info.isSearching = (isSearching == 1);
                info.hasPendingChallenge = (hasPending == 1);
                info.isInGame = (isInGame == 1);
                m_playerList.push_back(info);
            }
        } else if (packetType == PKT_CHALLENGE) {
            uint8_t challengerId;
            std::string challengerName;
            packet >> challengerId >> challengerName;
            m_challengerName = challengerName;
            m_state = ClientState::ChallengeReceived;
        } else if (packetType == PKT_CHALLENGE_RESP) {
            uint8_t accepted;
            packet >> accepted;
            if (accepted == 0 && m_state == ClientState::ChallengeSent)
                m_state = ClientState::Idle;
        } else if (packetType == PKT_CANCEL_CHALLENGE) {
            m_state = ClientState::Idle;
            m_challengerName = "";
        } else if (packetType == PKT_SET_NAME) {
            uint8_t success;
            packet >> success;
            if (success == 1) {
                uint8_t assignedId;
                packet >> assignedId;
                m_clientId = assignedId;
                m_nameError = "";
                m_state = ClientState::Idle;
            } else {
                std::string errorMsg;
                packet >> errorMsg;
                m_nameError = errorMsg;
            }
        }
    }
}
