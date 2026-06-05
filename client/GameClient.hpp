#pragma once
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include "Packets.hpp"
#include "TroopType.hpp"

enum class ClientState {
    Waiting,
    Playing
};

class GameClient {
public:
    bool connect(const std::string &host, unsigned short port);

    void receivePackets();

    void deployTroop(TroopType troopType, float x, float y);

    [[nodiscard]] ClientState getState() const { return m_state; }
    [[nodiscard]] uint8_t getPlayerId() const { return m_playerId; }
    [[nodiscard]] const std::vector<EntitySnapshot> &getEntities() const { return m_entities; }

private:
    sf::TcpSocket m_socket;
    ClientState m_state = ClientState::Waiting;
    uint8_t m_playerId = 255;
    std::vector<EntitySnapshot> m_entities;
};
