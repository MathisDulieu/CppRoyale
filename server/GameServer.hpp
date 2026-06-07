#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <memory>
#include "ClientSession.hpp"
#include "Lobby.hpp"
#include "GameState.hpp"

struct ActiveGame {
    GameState gameState;
    uint8_t clientIds[2];
    std::vector<uint8_t> spectatorIds;
};

class GameServer {
public:
    GameServer(unsigned short port);

    void run();

private:
    void acceptClients();

    void receivePackets();

    void removeDisconnectedClients();

    void processPacket(sf::Packet &packet, ClientSession &session);

    void handleSetName(sf::Packet &packet, ClientSession &session);

    void handleFindMatch(ClientSession &session);

    void handleCancelMatch(ClientSession &session);

    void handlePlayerListRequest(ClientSession &session);

    void handleChallenge(sf::Packet &packet, ClientSession &session);

    void handleChallengeResponse(sf::Packet &packet, ClientSession &session);

    void handleCancelChallenge(ClientSession &session);

    void handleDeploy(sf::Packet &packet, ClientSession &session);

    void handleReturnToLobby(ClientSession &session);

    void handleSpectate(sf::Packet &packet, ClientSession &session);

    void handleSpectateLeave(ClientSession &session);

    void startGame(uint8_t clientId0, uint8_t clientId1);

    void broadcastMatchFound(uint8_t clientId0, uint8_t clientId1);

    void broadcastGameState(ActiveGame &game);

    void broadcastGameOver(ActiveGame &game, uint8_t winnerPlayerId);

    void broadcastSpectatorCount(ActiveGame &game);

    void broadcastPlayerList();

    void sendPlayerList(ClientSession &session);

    ClientSession *findSession(uint8_t clientId);

    ActiveGame *findGame(uint8_t clientId);

    ActiveGame *findGameByPlayers(uint8_t clientId0, uint8_t clientId1);

    sf::TcpListener m_listener;
    sf::SocketSelector m_selector;
    std::vector<ClientSession> m_sessions;
    std::vector<ActiveGame> m_activeGames;
    Lobby m_lobby;
    bool m_running;
    uint8_t m_nextClientId;
};
