#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include <array>
#include "Packets.hpp"
#include "TroopType.hpp"
#include "Constants.hpp"

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
    uint8_t getWinnerId() const { return m_winnerId; }
    const std::vector<EntitySnapshot> &getEntities() const { return m_entities; }
    const std::vector<TowerSnapshot> &getTowers() const { return m_towers; }
    float getElixir() const { return m_elixir[m_playerId]; }
    float getRemainingTime() const { return m_remainingTime; }
    bool isOvertime() const { return m_isOvertime; }

private:
    sf::TcpSocket m_socket;
    ClientState m_state = ClientState::Waiting;
    uint8_t m_playerId = 0;
    uint8_t m_winnerId = 255;
    std::vector<EntitySnapshot> m_entities;
    std::vector<TowerSnapshot> m_towers;
    std::array<float, MAX_PLAYERS> m_elixir = {5.f, 5.f};
    float m_remainingTime = GAME_DURATION;
    bool m_isOvertime = false;
};
