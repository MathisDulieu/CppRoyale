#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <memory>

enum class ServerState {
    Waiting,
    Playing
};

struct ClientSlot {
    std::unique_ptr<sf::TcpSocket> socket;
    uint8_t playerId;
    bool ready;
};

class GameServer {
public:
    GameServer(unsigned short port);

    void run();

private:
    void acceptClients();

    void receivePackets();

    void broadcastStart();

    void tick();

    sf::TcpListener m_listener;
    sf::SocketSelector m_selector;
    std::vector<ClientSlot> m_clients;
    ServerState m_state;
    bool m_running;
};
