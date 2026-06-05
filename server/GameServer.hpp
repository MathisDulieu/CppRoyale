#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <memory>
#include "GameState.hpp"

enum class ServerState {
    Waiting,
    Playing,
    GameOver
};

struct ClientSlot {
    std::unique_ptr<sf::TcpSocket> socket;
    uint8_t playerId;
};

class GameServer {
public:
    GameServer(unsigned short port);

    void run();

private:
    void acceptClients();

    void receivePackets();

    void broadcastStart();

    void broadcastGameState();

    void broadcastGameOver(uint8_t winnerPlayerId);

    void processDeployPacket(sf::Packet &packet, uint8_t playerId);

    sf::TcpListener m_listener;
    sf::SocketSelector m_selector;
    std::vector<ClientSlot> m_clients;
    ServerState m_serverState;
    GameState m_gameState;
    bool m_running;
    bool m_startPending;
    int m_startPendingTicks;
};
