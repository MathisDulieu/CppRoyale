#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <memory>
#include "GameState.hpp"

enum class ServerState {
    Waiting,
    Playing
};

struct ClientSlot {
    std::unique_ptr<sf::TcpSocket> socket;
    uint8_t playerId;
};

class GameServer {
public:
    explicit GameServer(unsigned short port);

    void run();

private:
    void acceptClients();

    void receivePackets();

    void broadcastStart();

    void broadcastGameState() const;

    void processDeployPacket(sf::Packet &packet, uint8_t playerId);

    sf::TcpListener m_listener;
    sf::SocketSelector m_selector;
    std::vector<ClientSlot> m_clients;
    ServerState m_serverState;
    GameState m_gameState;
    bool m_running;
    bool m_startPending = false;
    int m_startPendingTicks = 0;
};
