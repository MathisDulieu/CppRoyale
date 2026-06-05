#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include "Packets.hpp"
#include "TroopType.hpp"

enum class ClientState {
    Waiting,
    Playing,
    GameOver
};

class GameClient {
public:
    bool connect(const std::string &host, unsigned short port);

    void receivePackets();

    void deployTroop(TroopType troopType, float x, float y);

    ClientState getState() const { return m_state; }
    uint8_t getPlayerId() const { return m_playerId; }
    const std::vector<EntitySnapshot> &getEntities() const { return m_entities; }
    const std::vector<TowerSnapshot> &getTowers() const { return m_towers; }
    uint8_t getWinnerId() const { return m_winnerId; }

private:
    sf::TcpSocket m_socket;
    ClientState m_state = ClientState::Waiting;
    uint8_t m_playerId = 255;
    uint8_t m_winnerId = 255;
    std::vector<EntitySnapshot> m_entities;
    std::vector<TowerSnapshot> m_towers;
};
