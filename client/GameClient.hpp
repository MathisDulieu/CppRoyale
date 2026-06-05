#pragma once
#include <SFML/Network.hpp>
#include <string>
#include <cstdint>

enum class ClientState {
    Waiting,
    Playing
};

class GameClient {
public:
    bool connect(const std::string &host, unsigned short port);

    void sendPing();

    void receivePackets();

    ClientState getState() const { return m_state; }
    uint8_t getPlayerId() const { return m_playerId; }

private:
    sf::TcpSocket m_socket;
    ClientState m_state = ClientState::Waiting;
    uint8_t m_playerId = 255;
};
