#pragma once
#include <SFML/Network.hpp>
#include <string>

class GameClient {
public:
    bool connect(const std::string& host, unsigned short port);
    void sendPing();
    bool receivePong();

private:
    sf::TcpSocket m_socket;
};