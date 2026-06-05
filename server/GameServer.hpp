#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <memory>

class GameServer {
public:
    GameServer(unsigned short port);
    void run();

private:
    void acceptClients();
    void receivePackets();
    void broadcastPong();

    sf::TcpListener              m_listener;
    sf::SocketSelector           m_selector;
    std::vector<std::unique_ptr<sf::TcpSocket>> m_clients;
    bool m_running = true;
};